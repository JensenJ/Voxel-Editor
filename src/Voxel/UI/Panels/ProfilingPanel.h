#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Log/Profiler.h>
#include <Voxel/UI/UIPanel.h>

#define PROF_NODE(name, timerPtr, ...)                                                             \
    {                                                                                              \
        name, timerPtr, { __VA_ARGS__ }                                                            \
    }

struct ProfilerNode {
    const char* name;
    FrameTimer<>* timer;
    std::vector<ProfilerNode> children;
};

class ProfilingPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Profiling"; }

  private:
    ImVec4 GetColourForRatio(float ratio) {
        if (ratio < 0.25f)
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // white
        if (ratio < 0.5f)
            return ImVec4(1.0f, 1.0f, 0.6f, 1.0f); // yellow
        if (ratio < 0.75f)
            return ImVec4(1.0f, 0.7f, 0.4f, 1.0f); // orange
        return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);     // red
    }

    ImVec4 GetColourForTotalFrame(float ratio) {
        if (ratio < 1.02f)
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // white if within budget or slightly over
        if (ratio < 1.25f)
            return ImVec4(1.0f, 1.0f, 0.6f, 1.0f); // yellow
        if (ratio < 1.5f)
            return ImVec4(1.0f, 0.7f, 0.4f, 1.0f); // orange
        return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);     // red
    }

    void DrawProfilerNode(ProfilerNode& node, double parentTime, bool isRoot = false) {
        const char* name = node.name;
        double rawTime = node.timer->lastFrame;
        double avgTime = node.timer->GetAverage();
        double maxTime = std::max(rawTime, avgTime);
        float ratio = 0.0f;
        if (!isRoot && parentTime > 0.0f)
            ratio = maxTime / parentTime;
        std::vector<ProfilerNode> children = node.children;

        ImVec4 col;
        if (isRoot) {
            float budget = 1000 / targetFPS;
            col = GetColourForTotalFrame(maxTime / budget);
        } else {
            col = GetColourForRatio(ratio);
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
                                   ImGuiTreeNodeFlags_DefaultOpen |
                                   ImGuiTreeNodeFlags_DrawLinesFull;

        if (children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        ImGui::PushStyleColor(ImGuiCol_Text, col);
        bool opened = ImGui::TreeNodeEx(
            (std::string("ProfilerNode-") + "##" + name + std::to_string((uintptr_t)&node)).c_str(),
            flags, "%s", name);

        ImGui::SameLine(250.0f);
        ImGui::Text("%.3f ms", rawTime);
        ImGui::SameLine(400.0f);
        ImGui::Text("%.3f ms", avgTime);

        ImGui::PopStyleColor();
        if (opened) {
            for (ProfilerNode child : children) {
                DrawProfilerNode(child, maxTime);
            }
            ImGui::TreePop();
        }
    }

    void RenderInternal() override {
        ScopedTimer timer(Profiler::ui_profiling.lastFrame);

        float frameFPS =
            Profiler::frame.lastFrame > 0.0 ? 1000.0f / Profiler::frame.lastFrame : 0.0f;
        float frameFPSAvg =
            Profiler::frame.GetAverage() > 0.0 ? 1000.0f / Profiler::frame.GetAverage() : 0.0f;

        ImGui::Text("%.2f FPS | %.2f Average FPS", frameFPS, frameFPSAvg);
        float budget = 1000 / targetFPS;
        ImGui::PlotLines("Frame time (ms)", GraphFrameHistory::values,
                         GraphFrameHistory::GRAPH_FRAME_HISTORY_COUNT, GraphFrameHistory::index,
                         nullptr, 0.0f, budget * 5.0, ImVec2(0, 140));

        ImGui::Text("System");
        ImGui::SameLine(250.0f);
        ImGui::Text("Last ms");
        ImGui::SameLine(400.0f);
        ImGui::Text("Avg ms");

        DrawProfilerNode(root, std::max(root.timer->lastFrame, root.timer->GetAverage()), true);
    }

    ProfilerNode root =
        PROF_NODE("Frame", &Profiler::frame,
                  PROF_NODE("UI", &Profiler::ui, PROF_NODE("Viewport", &Profiler::ui_viewport),
                            PROF_NODE("Components", &Profiler::ui_component),
                            PROF_NODE("Hierarchy", &Profiler::ui_hierarchy),
                            PROF_NODE("Logging", &Profiler::ui_logging,
                                      PROF_NODE("Copy Buffer", &Profiler::ui_logging_copyBuffer),
                                      PROF_NODE("Render", &Profiler::ui_logging_render)),
                            PROF_NODE("Profiling", &Profiler::ui_profiling)),
                  PROF_NODE("System", &Profiler::system,
                            PROF_NODE("Render", &Profiler::system_render,
                                      PROF_NODE("Batching", &Profiler::system_render_batching),
                                      PROF_NODE("Drawing", &Profiler::system_render_draw)),
                            PROF_NODE("Transform", &Profiler::system_transform),
                            PROF_NODE("Visibility", &Profiler::system_visibility)));

    int LoadStyles() override { return 0; }

    double targetFPS = 60.0f;
};