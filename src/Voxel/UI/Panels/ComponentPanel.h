#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/UI/UIPanel.h>

class ComponentPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Components"; }

  private:
    void RenderInternal() override {
        /*EntityRegistry* registry = EntityRegistry::GetInstance();
        if (!registry)
            return;

        if (registry->selectedEntities.empty()) {
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
        }*/
    }

    int LoadStyles() override { return 0; }
};
