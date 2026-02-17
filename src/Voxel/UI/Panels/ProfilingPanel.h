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
    ImVec4 GetColor(bool isRoot, float frameTime) {
        float ratio = glm::clamp(frameTime / frameBudget, 0.0f, 1.0f);

        if (isRoot) {
            if (frameTime <= frameBudget)
                return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            ratio = glm::clamp((frameTime - frameBudget) / frameBudget, 0.0f, 1.0f);
        }

        float r = 1.0f;
        float g = (ratio < 0.5f) ? 1.0f : 1.0f - 2.0f * (ratio - 0.5f);
        float b = 1.0f - ratio;

        return ImVec4(r, g, b, 1.0f);
    }

    void DrawProfilerNode(const ProfilerNode& node, bool isRoot = false) {
        const char* name = node.name;
        double avgTime = node.timer->GetAverage();
        double rawTime = node.timer->previousFrame;
        double maxTime = node.timer->GetMax();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
                                   ImGuiTreeNodeFlags_DefaultOpen |
                                   ImGuiTreeNodeFlags_DrawLinesFull;

        if (node.childCount == 0)
            flags |= ImGuiTreeNodeFlags_Leaf;

        bool opened = ImGui::TreeNodeEx((void*)&node, flags, "%s", name);

        ImGui::PushStyleColor(ImGuiCol_Text, GetColor(isRoot, rawTime));
        ImGui::SameLine(250.0f);
        ImGui::Text("%.3f ms", rawTime);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text, GetColor(isRoot, avgTime));
        ImGui::SameLine(400.0f);
        ImGui::Text("%.3f ms", avgTime);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text, GetColor(isRoot, maxTime));
        ImGui::SameLine(550.0f);
        ImGui::Text("%.3f ms", maxTime);
        ImGui::PopStyleColor();

        if (opened) {
            for (size_t i = 0; i < node.childCount; ++i)
                DrawProfilerNode(node.children[i]);
            ImGui::TreePop();
        }
    }

    void RenderInternal() override {
        ScopedTimer timer(Profiler::ui_profiling);

        float frameFPS =
            Profiler::frame.previousFrame > 0.0 ? 1000.0f / Profiler::frame.previousFrame : 0.0f;
        float frameFPSAvg =
            Profiler::frame.GetAverage() > 0.0 ? 1000.0f / Profiler::frame.GetAverage() : 0.0f;

        ImGui::Text("%.2f FPS | %.2f Average FPS", frameFPS, frameFPSAvg);
        float budget = 1000 / targetFPS;
        ImGui::PlotLines("Frame time (ms)", Profiler::frame.GetBuffer(), Profiler::frame.GetCount(),
                         Profiler::frame.GetOffset(), nullptr, 0.0f, budget * 5.0, ImVec2(0, 140));

        ImGui::Text("System");
        ImGui::SameLine(250.0f);
        ImGui::Text("Prev ms");
        ImGui::SameLine(400.0f);
        ImGui::Text("Avg ms");
        ImGui::SameLine(550.0f);
        ImGui::Text("Max ms");

        DrawProfilerNode(root, true);
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

    float targetFPS = 60.0f;
    float frameBudget = 1000.0f / targetFPS;
};