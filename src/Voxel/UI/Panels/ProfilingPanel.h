#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Log/Profiler.h>
#include <Voxel/UI/UIPanel.h>

#define PROF_NODE(name, value, ...)                                                                \
    {                                                                                              \
        name, []() { return (float)(value); }, { __VA_ARGS__ }                                     \
    }

struct ProfilerNode {
    const char* name;
    std::function<double()> getValue;
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
        if (ratio < 1.01f)
            return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // white if within budget or slightly over
        if (ratio < 1.25f)
            return ImVec4(1.0f, 1.0f, 0.6f, 1.0f); // yellow
        if (ratio < 1.5f)
            return ImVec4(1.0f, 0.7f, 0.4f, 1.0f); // orange
        return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);     // red
    }

    void DrawProfilerNode(ProfilerNode& node, double parentTime, bool isRoot = false) {
        const char* name = node.name;
        double time = node.getValue();
        float ratio = 0.0f;
        if (!isRoot && parentTime > 0.0f)
            ratio = time / parentTime;
        std::vector<ProfilerNode> children = node.children;

        ImVec4 col;
        if (isRoot) {
            float budget = 1000 / targetFPS;
            col = GetColourForTotalFrame(time / budget);
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
            flags, "%s: %.2f ms", name, time);
        ImGui::PopStyleColor();

        if (opened) {
            for (ProfilerNode child : children) {
                DrawProfilerNode(child, time);
            }
            ImGui::TreePop();
        }
    }

    void RenderInternal() override {
        ScopedTimer timer(Profiler::uiProfiling);

        float frameFPS = Profiler::frame > 0.0 ? 1000.0f / Profiler::frame : 0.0f;
        float frameFPSAvg = Profiler::frameAvg.GetAverage() > 0.0
                                ? 1000.0f / Profiler::frameAvg.GetAverage()
                                : 0.0f;

        ImGui::Text("%.2f FPS | %.2f Average FPS", frameFPS, frameFPSAvg);
        float budget = 1000 / targetFPS;
        ImGui::PlotLines("Frame time (ms)", GraphFrameHistory::values,
                         GraphFrameHistory::GRAPH_FRAME_HISTORY_COUNT, GraphFrameHistory::index,
                         nullptr, 0.0f, budget * 5.0, ImVec2(0, 140));
        DrawProfilerNode(root, root.getValue(), true);
    }

    ProfilerNode root = PROF_NODE(
        "Frame", Profiler::frameAvg.GetAverage(),
        PROF_NODE("UI", Profiler::uiAvg.GetAverage(),
                  PROF_NODE("Viewport", Profiler::uiViewportAvg.GetAverage()),
                  PROF_NODE("Components", Profiler::uiComponentAvg.GetAverage()),
                  PROF_NODE("Hierarchy", Profiler::uiHierarchyAvg.GetAverage()),
                  PROF_NODE("Logging", Profiler::uiLoggingAvg.GetAverage(),
                            PROF_NODE("Copy Buffer", Profiler::uiLoggingCopyBufferAvg.GetAverage()),
                            PROF_NODE("Render", Profiler::uiLoggingRenderAvg.GetAverage())),
                  PROF_NODE("Profiling", Profiler::uiProfilingAvg.GetAverage())),
        PROF_NODE("System", Profiler::systemAvg.GetAverage(),
                  PROF_NODE("Viewport", Profiler::uiViewportAvg.GetAverage()),
                  PROF_NODE("Render", Profiler::systemRenderAvg.GetAverage(),
                            PROF_NODE("Batching", Profiler::systemRenderBatchingAvg.GetAverage()),
                            PROF_NODE("Drawing", Profiler::systemRenderDrawAvg.GetAverage())),
                  PROF_NODE("Transform", Profiler::systemTransformAvg.GetAverage()),
                  PROF_NODE("Visibility", Profiler::systemVisibilityAvg.GetAverage())));

    int LoadStyles() override { return 0; }

    double targetFPS = 60.0f;
};