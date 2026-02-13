#include "MainUI.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <mutex>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <Voxel/Rendering/FrameBuffer.h>
#include <Voxel/UI/MenuBar.h>
#include <Voxel/UI/Panels/ComponentPanel.h>
#include <Voxel/UI/Panels/HierarchyPanel.h>
#include <Voxel/UI/Panels/LogPanel.h>
#include <Voxel/UI/Panels/ProfilingPanel.h>
#include <Voxel/UI/Panels/ViewportPanel.h>
#include <Voxel/UI/UIPanel.h>
#include <Voxel/UI/UIStyle.h>

void MainUI::RegisterPanels() {
    {
        auto panel = std::make_unique<ViewportPanel>();
        viewportPanel = panel.get();
        panels.push_back(std::move(panel));
    }
    {
        auto panel = std::make_unique<ComponentPanel>();
        componentPanel = panel.get();
        panels.push_back(std::move(panel));
    }
    {
        auto panel = std::make_unique<HierarchyPanel>();
        hierarchyPanel = panel.get();
        panels.push_back(std::move(panel));
    }
    {
        auto panel = std::make_unique<LogPanel>();
        logPanel = panel.get();
        panels.push_back(std::move(panel));
    }
    {
        auto panel = std::make_unique<ProfilingPanel>();
        profilingPanel = panel.get();
        panels.push_back(std::move(panel));
    }
}

void MainUI::RenderUI() {
    ScopedTimer timer(Profiler::ui);
    SetupFrame();

    // Render panels
    for (auto& panel : panels) {
        if (panel) {
            panel->Render();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Render other viewports
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void MainUI::Initialise() {
    Application* application = Application::GetInstance();
    if (application == nullptr) {
        return;
    }
    // TODO: Load from settings, if it doesn't exist, try get from OS scale
    float uiScale = 1.25f;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    std::string fontPath =
        std::filesystem::current_path().string() + "\\resources\\fonts\\Roboto-Regular.ttf";
    io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 18.0f * uiScale);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    io.ConfigDragClickToInputText = true; // Single click to modify slider value directly

    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(uiScale);

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look
    // identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    UIStyle::SetStyle();
    RegisterPanels();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(application->GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");
    LOG_INFO("Initialised ImGui");
}

void MainUI::SetupFrame() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Dockspace", nullptr,
                 ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("Dockspace");
    if (ShouldBuildDefaultDockLayout()) {
        BuildDefaultDockLayout(dockspace_id);
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    MenuBar::Render();
    ImGui::End();
}

void MainUI::BuildDefaultDockLayout(ImGuiID dockspaceID) {
    ImGui::DockBuilderRemoveNode(dockspaceID);
    ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_PassthruCentralNode |
                                               ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetMainViewport()->WorkSize);

    ImGuiID center = dockspaceID;
    ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.2f, nullptr, &center);
    ImGuiID rightTop = ImGui::DockBuilderSplitNode(right, ImGuiDir_Up, 0.25f, nullptr, &right);
    ImGuiID rightDown = ImGui::DockBuilderSplitNode(right, ImGuiDir_Down, 0.25f, nullptr, &right);
    ImGuiID down = ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.2f, nullptr, &center);

    // Dock the windows in the correct place
    ImGui::DockBuilderDockWindow(viewportPanel->GetPanelName(), center);
    ImGui::DockBuilderDockWindow(componentPanel->GetPanelName(), right);
    ImGui::DockBuilderDockWindow(hierarchyPanel->GetPanelName(), rightTop);
    ImGui::DockBuilderDockWindow(logPanel->GetPanelName(), down);
    ImGui::DockBuilderDockWindow(profilingPanel->GetPanelName(), rightDown);
    ImGui::DockBuilderFinish(dockspaceID);

    dockLayoutBuilt = true;
    LOG_INFO("Built default dockspace");
}

bool MainUI::ShouldBuildDefaultDockLayout() {
    const ImGuiIO& io = ImGui::GetIO();

    if (dockLayoutBuilt)
        return false;

    if (!io.IniFilename)
        return true;

    return !std::filesystem::exists(io.IniFilename);
}

MenuBar* MainUI::GetMenuBar() { return menuBar; }

ViewportPanel* MainUI::GetViewportPanel() { return viewportPanel; }

LogPanel* MainUI::GetLogPanel() { return logPanel; }

HierarchyPanel* MainUI::GetHierarchyPanel() { return hierarchyPanel; }

ComponentPanel* MainUI::GetComponentPanel() { return componentPanel; }