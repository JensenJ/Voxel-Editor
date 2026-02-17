#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/UI/UIPanel.h>

class LogPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Log"; }

  private:
    void RenderInternal() override {
        ScopedTimer timer(Profiler::ui_logging);
        static bool wasAtBottom = true;
        bool newEntries = false;

        std::shared_ptr<ImGuiSinkMT> logSink = Log::GetImGuiLogSink();
        if (!logSink) {
            ImGui::End();
            return;
        }

        {
            ScopedTimer timer(Profiler::ui_logging_copyBuffer);
            if (logSink->updated) {
                logBufferCopy = logSink->GetBufferCopy();
                logSink->updated = false;
                newEntries = true;
            }
        }

        {
            ScopedTimer timer(Profiler::ui_logging_render);
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                           ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
                                           ImGuiTableFlags_Reorderable;

            if (!ImGui::BeginTable("LogTable", 5, flags))
                return;

            float scrollY = ImGui::GetScrollY();
            float scrollMaxY = ImGui::GetScrollMaxY();
            wasAtBottom = (scrollY >= scrollMaxY - 2.0f);

            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, 140.0f);
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("Thread", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(logBufferCopy.size()));
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                    const auto& entry = logBufferCopy[i];
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text("%04d-%02d-%02d", entry.time.tm_year + 1900, entry.time.tm_mon + 1,
                                entry.time.tm_mday);

                    ImGui::TableNextColumn();
                    ImGui::Text("%02d:%02d:%02d.%03ld", entry.time.tm_hour, entry.time.tm_min,
                                entry.time.tm_sec, entry.ms);

                    ImGui::TableNextColumn();
                    ImGui::Text("%lu", entry.thread_id);

                    ImGui::TableNextColumn();
                    DrawSeverity(entry.level);

                    ImGui::TableNextColumn();
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextUnformatted(entry.message.c_str());
                    ImGui::PopTextWrapPos();
                }
            }
            if (newEntries && wasAtBottom)
                ImGui::SetScrollHereY(1.0f);

            ImGui::EndTable();
        }
    }

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

  private:
    std::vector<LogEntry> logBufferCopy = std::vector<LogEntry>();
};