#include "MainUI.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "../Application.h"
#include "../Entity/Entity.h"
#include "../Entity/EntityRegistry.h"
#include "../Rendering/FrameBuffer.h"

// Render ImGui UI
void MainUI::RenderUI() {
    Application* application = Application::GetInstance();
    if (application == nullptr) {
        return;
    }

    EntityRegistry* entityRegistry = EntityRegistry::GetInstance();
    if (entityRegistry == nullptr) {
        return;
    }

    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
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
    // check if this layout exists (if defined in imgui.ini)
    static bool firstTimeLoading = ImGui::DockBuilderGetNode(dockspace_id) == nullptr;
    // Create new dockspace with this id (loads layout if one exists)
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);

    // Specify layout of viewports if this is the first time loading
    if (firstTimeLoading) {
        // Prevent layout being reapplied next frame
        firstTimeLoading = false;

        // Clear previous dockspace layout
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        // Create new node on the right of this dockspace
        auto dockIdRight =
            ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, nullptr, &dockspace_id);
        auto dockIdRightTop =
            ImGui::DockBuilderSplitNode(dockIdRight, ImGuiDir_Up, 0.25f, nullptr, &dockIdRight);

        // Dock the windows in the correct place
        ImGui::DockBuilderDockWindow("Properties", dockIdRight);
        ImGui::DockBuilderDockWindow("Hierarchy", dockIdRightTop);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    // Menu bars (menus at top)
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
    ImGui::End();

    // Dock main viewport to dockspace
    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);

    // Render viewport with openGL frame buffer
    ImGui::Begin("Viewport");
    {
        ImGui::BeginChild("View");
        application->SetSceneViewportWidth((int)ImGui::GetContentRegionAvail().x);
        application->SetSceneViewportWidth((int)ImGui::GetContentRegionAvail().y);

// Disable a warning which cant be resolved
#pragma warning(push)
#pragma warning(disable : 4312)
        ImGui::Image(ImTextureID(application->GetSceneBuffer()->GetFrameTexture()),
                     ImGui::GetContentRegionMax(), ImVec2(0, 1), ImVec2(1, 0));
#pragma warning(pop)
        ImGui::EndChild();
    }

    ImGui::End();

    ImGui::Begin("Hierarchy");
    {
        // Get all entities and list them
        std::map<unsigned int, Entity*> entities = entityRegistry->GetAllEntities();

        static ImGuiTableFlags entityHierarchyFlags =
            ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

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
        }
        ImGui::End();

        // Properties window for selected entities
        ImGui::Begin("Properties");

        // ImGui::Text("TestLabel");
        // For every selected entity
        for (auto iter = entityRegistry->selectedEntities.begin();
             iter != entityRegistry->selectedEntities.end(); ++iter) {
            // Create tree node for this entity
            if (ImGui::TreeNode((*iter)->GetEntityName().c_str())) {
                // Get its components and create nodes for all of them
                std::vector<Component*> components = (*iter)->GetComponents();
                for (int i = 0; i < components.size(); i++) {
                    // If this component has a properties panel
                    if (components[i]->ShouldRenderProperties()) {
                        // Render it with the name of this component
                        if (ImGui::TreeNode(components[i]->GetComponentName().c_str())) {
                            components[i]->RenderPropertiesPanel();
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Render other viewports
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}