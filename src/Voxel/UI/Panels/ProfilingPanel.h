#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Log/Profiler.h>
#include <Voxel/UI/UIPanel.h>

struct ProfilerNode {
    const char* name;
    FrameTimer<>* timer;
    const ProfilerNode* children;
    size_t childCount;
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

    void DrawProfilerNode(const ProfilerNode& node, double parentTime, bool isRoot = false) {
        const char* name = node.name;
        double avgTime = node.timer->GetAverage();
        double rawTime = node.timer->lastFrame;
        double maxTime = node.timer->GetMax();
        double colourTime = std::max(maxTime, std::max(rawTime, avgTime));
        float ratio = maxTime / parentTime;

        ImVec4 col = isRoot ? GetColourForTotalFrame((float)(colourTime / frameBudget))
                            : GetColourForRatio(ratio);

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
                                   ImGuiTreeNodeFlags_DefaultOpen |
                                   ImGuiTreeNodeFlags_DrawLinesFull;

        if (node.childCount == 0)
            flags |= ImGuiTreeNodeFlags_Leaf;

        ImGui::PushStyleColor(ImGuiCol_Text, col);
        bool opened = ImGui::TreeNodeEx((void*)&node, flags, "%s", name);

        ImGui::SameLine(250.0f);
        ImGui::Text("%.3f ms", rawTime);
        ImGui::SameLine(400.0f);
        ImGui::Text("%.3f ms", avgTime);
        ImGui::SameLine(550.0f);
        ImGui::Text("%.3f ms", maxTime);

        ImGui::PopStyleColor();
        if (opened) {
            for (size_t i = 0; i < node.childCount; ++i)
                DrawProfilerNode(node.children[i], colourTime);
            ImGui::TreePop();
        }
    }

    void RenderInternal() override {
        ScopedTimer timer(Profiler::ui_profiling);

        float frameFPS =
            Profiler::frame.lastFrame > 0.0 ? 1000.0f / Profiler::frame.lastFrame : 0.0f;
        float frameFPSAvg =
            Profiler::frame.GetAverage() > 0.0 ? 1000.0f / Profiler::frame.GetAverage() : 0.0f;

        ImGui::Text("%.2f FPS | %.2f Average FPS", frameFPS, frameFPSAvg);
        float budget = 1000 / targetFPS;
        ImGui::PlotLines("Frame time (ms)", Profiler::frame.GetBuffer(), Profiler::frame.GetCount(),
                         Profiler::frame.GetOffset(), nullptr, 0.0f, budget * 5.0, ImVec2(0, 140));

        ImGui::Text("System");
        ImGui::SameLine(250.0f);
        ImGui::Text("Last ms");
        ImGui::SameLine(400.0f);
        ImGui::Text("Avg ms");
        ImGui::SameLine(550.0f);
        ImGui::Text("Max ms");

        DrawProfilerNode(root,
                         std::max(root.timer->GetMax(),
                                  std::max(root.timer->lastFrame, root.timer->GetAverage())),
                         true);
    }

    static inline ProfilerNode uiLoggingChildren[] = {
        {"Copy Buffer", &Profiler::ui_logging_copyBuffer, nullptr, 0},
        {"Render", &Profiler::ui_logging_render, nullptr, 0}};

    static inline ProfilerNode uiHierarchyChildren[] = {
        {"Build List", &Profiler::ui_hierarchy_buildList, nullptr, 0},
        {"Render", &Profiler::ui_hierarchy_render, nullptr, 0}};

    static inline ProfilerNode uiChildren[] = {
        {"Viewport", &Profiler::ui_viewport, nullptr, 0},
        {"Components", &Profiler::ui_component, nullptr, 0},
        {"Hierarchy", &Profiler::ui_hierarchy, uiHierarchyChildren, 2},
        {"Logging", &Profiler::ui_logging, uiLoggingChildren, 2},
        {"Profiling", &Profiler::ui_profiling, nullptr, 0}};

    static inline ProfilerNode systemChildren[] = {
        {"Render", &Profiler::system_render, nullptr, 0},
        {"Transform", &Profiler::system_transform, nullptr, 0},
        {"Visibility", &Profiler::system_visibility, nullptr, 0}};

    static inline ProfilerNode frameChildren[] = {{"UI", &Profiler::ui, uiChildren, 5},
                                                  {"System", &Profiler::system, systemChildren, 3}};

    static inline ProfilerNode root = {"Frame", &Profiler::frame, frameChildren, 2};

    int LoadStyles() override { return 0; }

    double targetFPS = 60.0f;
    double frameBudget = 1000.0f / targetFPS;
};