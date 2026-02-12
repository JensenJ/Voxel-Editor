#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Log/Profiler.h>
#include <Voxel/UI/UIPanel.h>

class ProfilingPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Profiling"; }

  private:
    void RenderInternal() override {
        ScopedTimer timer(Profiler::uiProfiling);

        float frameFPS = Profiler::frame > 0.0 ? 1000.0f / Profiler::frame : 0.0f;
        float frameFPSAvg = Profiler::frameAvg.GetAverage() > 0.0
                                ? 1000.0f / Profiler::frameAvg.GetAverage()
                                : 0.0f;

        ImGui::Text("Frame: %.2f ms (%.1f FPS) : %.2f ms (%.1f FPS)", Profiler::frame, frameFPS,
                    Profiler::frameAvg.GetAverage(), frameFPSAvg);

        ImGui::Indent();
        ImGui::Text("UI: %.2f ms : %.2f ms", Profiler::ui, Profiler::uiAvg.GetAverage());
        ImGui::Indent();
        ImGui::Text("Viewport: %.2f ms : %.2f ms", Profiler::uiViewport,
                    Profiler::uiViewportAvg.GetAverage());
        ImGui::Text("Component: %.2f ms : %.2f ms", Profiler::uiComponent,
                    Profiler::uiComponentAvg.GetAverage());
        ImGui::Text("Hierarchy: %.2f ms : %.2f ms", Profiler::uiHierarchy,
                    Profiler::uiHierarchyAvg.GetAverage());
        ImGui::Text("Logging: %.2f ms : %.2f ms", Profiler::uiLogging,
                    Profiler::uiLoggingAvg.GetAverage());
        ImGui::Text("Profiling: %.2f ms : %.2f ms", Profiler::uiProfiling,
                    Profiler::uiProfilingAvg.GetAverage());
        ImGui::Unindent();

        ImGui::Text("System: %.2f ms : %.2f ms", Profiler::system,
                    Profiler::systemAvg.GetAverage());
        ImGui::Indent();
        ImGui::Text("Render: %.2f ms : %.2f ms", Profiler::systemRender,
                    Profiler::systemRenderAvg.GetAverage());
        ImGui::Indent();
        ImGui::Text("Batching: %.2f ms : %.2f ms", Profiler::systemRenderBatching,
                    Profiler::systemRenderBatchingAvg.GetAverage());
        ImGui::Text("Drawing: %.2f ms : %.2f ms", Profiler::systemRenderDraw,
                    Profiler::systemRenderDrawAvg.GetAverage());
        ImGui::Unindent();
        ImGui::Text("Transform: %.2f ms : %.2f ms", Profiler::systemTransformAvg.GetAverage());
        ImGui::Text("Visibility: %.2f ms : %.2f ms", Profiler::systemVisibilityAvg.GetAverage());
        ImGui::Unindent();
        ImGui::Unindent();
    }
    int LoadStyles() override { return 0; }
};