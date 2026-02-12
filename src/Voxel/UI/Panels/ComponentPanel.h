#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/MeshComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/ECS/Components/TransformComponent.h>
#include <Voxel/ECS/EditorRenderable.h>
#include <Voxel/UI/UIPanel.h>

class ComponentPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Components"; }

  private:
    template <EditorRenderable T>
    void RenderComponentIfPresent(EntityRegistry& registry, Entity e) {
        if (auto* comp = registry.GetComponent<T>(e)) {
            ImGui::PushID(&comp);
            if (ImGui::CollapsingHeader(T::ComponentName, ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent();
                comp->RenderComponentPanel();
                ImGui::Unindent();
            }
            ImGui::PopID();
        }
    }

    void RenderInternal() override {
        ScopedTimer timer(Profiler::uiComponent);
        EntityRegistry* registry = EntityRegistry::GetInstance();
        if (!registry)
            return;

        Entity selected = registry->GetSelectedEntity();
        if (selected == InvalidEntity)
            return;

        RenderComponentIfPresent<MetaComponent>(*registry, selected);
        RenderComponentIfPresent<HierarchyComponent>(*registry, selected);
        RenderComponentIfPresent<TransformComponent>(*registry, selected);
        RenderComponentIfPresent<MeshComponent>(*registry, selected);
    }

    int LoadStyles() override { return 0; }
};
