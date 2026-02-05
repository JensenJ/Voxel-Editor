#pragma once
#include <Voxel/pch.h>
#include <Voxel/UI/UIPanel.h>

class LogPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Log"; }

  private:
    void RenderInternal() override {
        static bool scrollToBottom = true;

        std::shared_ptr<ImGuiSinkMT> logSink = Log::GetImGuiLogSink();
        if (!logSink) {
            ImGui::End();
            return;
        }

        auto logs = logSink->GetBufferCopy();

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
                                       ImGuiTableFlags_Reorderable;

        if (ImGui::BeginTable("LogTable", 5, flags)) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, 140.0f);
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("Thread", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (const auto& entry : logs) {
                ImGui::TableNextRow();

                auto tp = entry.time;
                auto secs = std::chrono::time_point_cast<std::chrono::seconds>(tp);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp - secs).count();

                // Convert timestamp once per row
                auto tt = std::chrono::system_clock::to_time_t(secs);
                std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
                localtime_s(&tm, &tt);
#else
                localtime_r(&tt, &tm);
#endif

                ImGui::TableNextColumn();
                ImGui::Text("%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

                ImGui::TableNextColumn();
                ImGui::Text("%02d:%02d:%02d.%03ld", tm.tm_hour, tm.tm_min, tm.tm_sec, ms);

                ImGui::TableNextColumn();
                ImGui::Text("%lu", entry.thread_id);

                ImGui::TableNextColumn();
                DrawSeverity(entry.level);

                ImGui::TableNextColumn();
                ImGui::PushTextWrapPos(0.0f);
                ImGui::TextUnformatted(entry.message.c_str());
                ImGui::PopTextWrapPos();
            }

            ImGui::EndTable();
        }
    };

    static void DrawSeverity(spdlog::level::level_enum level) {
        const char* text = "";
        ImVec4 color;

        switch (level) {
        case spdlog::level::trace:
            text = "TRACE";
            color = {1.0f, 1.0f, 1.0f, 1.0f};
            break;
        case spdlog::level::debug:
            text = "DEBUG";
            color = {1.0f, 0.5f, 1.0f, 1};
            break;
        case spdlog::level::info:
            text = "INFO ";
            color = {0.1f, 0.65f, 0.1f, 1};
            break;
        case spdlog::level::warn:
            text = "WARN ";
            color = {1.0f, 0.8f, 0.3f, 1};
            break;
        case spdlog::level::err:
            text = "ERROR";
            color = {1.0f, 0.3f, 0.3f, 1};
            break;
        case spdlog::level::critical:
            text = "FATAL";
            color = {1.0f, 0.0f, 0.0f, 1};
            break;
        default:
            text = "UNKN ";
            color = {1, 1, 1, 1};
            break;
        }

        ImGui::TextColored(color, "%s", text);
    }

    int LoadStyles() override {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        return 1;
    }
};