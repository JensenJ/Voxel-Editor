#include "MainUI.h"
#include <Voxel/pch.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <Voxel/Entity/Entity.h>
#include <Voxel/Entity/EntityRegistry.h>
#include <Voxel/Rendering/FrameBuffer.h>

void MainUI::RenderUI() {
    SetupFrame();

    // Render panels
    RenderViewport();
    RenderHierarchyPanel();
    RenderObjectPropertiesPanel();

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

    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(uiScale);

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look
    // identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    SetStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(application->GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");
    LOG_TRACE("Initialised ImGui");
}

void MainUI::SetupFrame() {
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Dockspace", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    // Create dockspace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiID dockspace_id = ImGui::GetID("Dockspace");
    static bool firstTimeLoading = ImGui::DockBuilderGetNode(dockspace_id) == nullptr;
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);

    if (firstTimeLoading) {
        firstTimeLoading = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        auto dockIdRight =
            ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, nullptr, &dockspace_id);
        auto dockIdRightTop =
            ImGui::DockBuilderSplitNode(dockIdRight, ImGuiDir_Up, 0.25f, nullptr, &dockIdRight);

        // Dock the windows in the correct place
        ImGui::DockBuilderDockWindow("Properties", dockIdRight);
        ImGui::DockBuilderDockWindow("Hierarchy", dockIdRightTop);
        ImGui::DockBuilderFinish(dockspace_id);
    }
    RenderMenuBar();
    ImGui::End();
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);
}

void MainUI::SetStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Base
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.105f, 0.11f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.105f, 0.11f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);

    // Headers (Tree, Selectables)
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.30f, 0.34f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.35f, 0.40f, 1.00f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.30f, 0.34f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.35f, 0.40f, 1.00f);

    // Frame BG (sliders, inputs)
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.20f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.22f, 0.26f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.43f, 0.47f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.32f, 0.36f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

    style.WindowRounding = 6.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;

    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.IndentSpacing = 18.0f;
}

void MainUI::RenderMenuBar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));
    if (ImGui::BeginMenuBar()) {
        // File menu
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New", "Ctrl+N");
            ImGui::MenuItem("Open", "Ctrl+N");
            ImGui::Separator();
            ImGui::MenuItem("Save", "Ctrl+S");
            ImGui::MenuItem("Save As", "Ctrl+Shift+S");
            ImGui::EndMenu();
        }

        // Edit menu
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "Ctrl+Z");
            ImGui::MenuItem("Redo", "Ctrl+Shift+Z");
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    ImGui::PopStyleVar();
}

void MainUI::RenderHierarchyPanel() {
    EntityRegistry* entityRegistry = EntityRegistry::GetInstance();
    if (entityRegistry == nullptr) {
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin("Hierarchy");
    {
        // Get all entities and list them
        std::map<unsigned int, Entity*> entities = entityRegistry->GetAllEntities();

        static ImGuiTableFlags entityHierarchyFlags =
            ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
            ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 2));
        if (ImGui::BeginTable("Hierarchy Entity Table", 2, entityHierarchyFlags)) {
            // Print table headers
            ImGui::TableSetupColumn("Entity ID", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Entity Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            // Print Entities in panel and make them selectable
            for (std::map<unsigned int, Entity*>::iterator iter = entities.begin();
                 iter != entities.end(); ++iter) {
                bool selected = false;
                bool found = false;

                // If this entity is part of selected entities already, then set selected to true
                auto selectedEntity = entityRegistry->selectedEntities.find(iter->second);

                if (selectedEntity != entityRegistry->selectedEntities.end()) {
                    selected = true;
                    found = true;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Selectable(std::to_string(iter->second->GetEntityID()).c_str(),
                                      &selected, ImGuiSelectableFlags_SpanAllColumns)) {

                    if (!ImGui::GetIO().KeyCtrl) // If ctrl is not held
                    {
                        // Clear the existing entities
                        entityRegistry->selectedEntities.clear();

                        // Check if its false as clicking on a selected item would deselect it
                        if (selected == false) {
                            if (found == true) // If this entity was previously selected
                            {
                                // Add it back to selected entities
                                entityRegistry->selectedEntities.insert(iter->second);
                            }
                        }
                        // If this entity is a different selection from on the list
                        else {
                            if (found == true) // If this entity was found in selected entities
                            {
                                entityRegistry->selectedEntities.erase(iter->second);
                            } else // If this entity was not found in selected entities
                            {
                                entityRegistry->selectedEntities.insert(iter->second);
                            }
                        }
                    } else // If CTRL was held
                    {
                        // If this entity is selected
                        if (selected == true) {
                            if (found == false) // If entity is not on the list already
                            {
                                entityRegistry->selectedEntities.insert(iter->second);
                            }
                        } else {
                            if (found == true) // If entity is on the list already
                            {
                                entityRegistry->selectedEntities.erase(iter->second);
                            }
                        }
                    }
                }
                ImGui::TableNextColumn();
                ImGui::Text("%s", iter->second->GetEntityName().c_str());
            }
            ImGui::EndTable();
            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

void MainUI::RenderObjectPropertiesPanel() {
    EntityRegistry* registry = EntityRegistry::GetInstance();
    if (!registry)
        return;

    ImGui::Begin("Properties");

    if (registry->selectedEntities.empty()) {
        ImGui::End();
        return;
    }

    Entity* entity = *registry->selectedEntities.begin();
    ImGui::Text("%s", entity->GetEntityName().c_str());

    for (Component* comp : entity->GetComponents()) {
        if (!comp->ShouldRenderProperties())
            continue;

        ImGui::PushID(comp);
        if (ImGui::CollapsingHeader(comp->GetComponentName().c_str(),
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            comp->RenderPropertiesPanel();
            ImGui::Unindent();
        }
        ImGui::PopID();
    }
    ImGui::End();
}


void MainUI::RenderViewport() {
    Application* application = Application::GetInstance();
    if (application == nullptr) {
        return;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    {
        ImGui::BeginChild("View");
        application->SetSceneViewportWidth((int)ImGui::GetContentRegionAvail().x);
        application->SetSceneViewportHeight((int)ImGui::GetContentRegionAvail().y);
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImGui::Image(ImTextureID(application->GetSceneBuffer()->GetFrameTexture()), size,
                     ImVec2(0, 1), ImVec2(1, 0));
        viewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
        ImGui::EndChild();
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

bool MainUI::IsSceneViewportHovered() { return viewportHovered; }
