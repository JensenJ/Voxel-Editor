#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/HierarchyComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/ECS/Systems/TransformSystem.h>
#include <Voxel/UI/UIPanel.h>

struct VisibleNode {
    Entity entity;
    MetaComponent* meta;
    HierarchyComponent* hierarchy;
    int depth;
};

class HierarchyPanel : public UIPanel {
  public:
    const char* GetPanelName() override { return "Hierarchy"; }

    HierarchyPanel() {
        EntityRegistry::onAddEntity.AddObserver(
            [this](const EntityAddEvent& event) { visibleNodesDirty = true; });

        EntityRegistry::onRemoveEntity.AddObserver(
            [this](const EntityRemoveEvent& event) { visibleNodesDirty = true; });

        EntityRegistry::onClearEntities.AddObserver(
            [this](const EntityClearEvent& event) { visibleNodesDirty = true; });
    }

  private:
    void BuildVisibleList(EntityRegistry* registry) {
        VisibleNodes.clear();

        auto view = registry->MakeView<const MetaComponent, const HierarchyComponent>();

        for (auto&& [entity, meta, hierarchy] : view) {
            if (!hierarchy.HasParent()) {
                AddEntityRecursive(registry, entity, 0);
            }
        }
    }

    void AddEntityRecursive(EntityRegistry* registry, Entity entity, int depth) {
        MetaComponent* meta = registry->GetComponent<MetaComponent>(entity);
        HierarchyComponent* hierarchy = registry->GetComponent<HierarchyComponent>(entity);

        if (!meta || !hierarchy)
            return;

        VisibleNodes.push_back({entity, meta, hierarchy, depth});

        if (!IsNodeOpen(entity))
            return;

        for (Entity child : hierarchy->children) {
            AddEntityRecursive(registry, child, depth + 1);
        }
    }

    void DrawVisibleNode(EntityRegistry* registry, VisibleNode& node) {
        Entity entity = node.entity;
        MetaComponent* meta = node.meta;
        HierarchyComponent* hierarchy = node.hierarchy;

        ImGui::Indent(node.depth * 35.0f);

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (IsNodeOpen(entity))
            flags |= ImGuiTreeNodeFlags_DefaultOpen;

        if (hierarchy->children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        if (entity == registry->GetSelectedEntity())
            flags |= ImGuiTreeNodeFlags_Selected;

        if (!meta->effectiveVisibility) {
            ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            col.w *= 0.4f;
            ImGui::PushStyleColor(ImGuiCol_Text, col);
        }

        bool opened =
            ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", meta->name.c_str());

        if (!meta->effectiveVisibility)
            ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
            registry->SelectEntity(entity);

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("HIERARCHY_ENTITY", &entity, sizeof(Entity));
            ImGui::Text("Reparent %s", meta->name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY")) {
                Entity dropped = *(Entity*)payload->Data;

                if (dropped != entity && !TransformSystem::IsDescendant(dropped, entity)) {
                    TransformSystem::Reparent(dropped, entity);
                    visibleNodesDirty = true;
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (IsNodeOpen(entity) != opened) {
            expanded[entity] = opened;
            visibleNodesDirty = true;
        }

        if (opened)
            ImGui::TreePop();

        ImGui::Unindent(node.depth * 35.0f);
    }

    void RenderInternal() override {
        ScopedTimer timer(Profiler::ui_hierarchy);

        EntityRegistry* registry = EntityRegistry::GetInstance();
        if (!registry)
            return;

        {
            ScopedTimer timer(Profiler::ui_hierarchy_buildList);
            if (visibleNodesDirty) {
                BuildVisibleList(registry);
                visibleNodesDirty = false;
            }
        }

        {
            ScopedTimer timer(Profiler::ui_hierarchy_render);

            ImGuiListClipper clipper;
            clipper.Begin((int)VisibleNodes.size());

            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                    DrawVisibleNode(registry, VisibleNodes[i]);
                }
            }

            // Root drag target
            if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(),
                                                 ImGui::GetID("HierarchyRootDrop"))) {
                if (const ImGuiPayload* payload =
                        ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY")) {
                    Entity dropped = *(Entity*)payload->Data;
                    TransformSystem::Reparent(dropped, InvalidEntity);
                    visibleNodesDirty = true;
                }
                ImGui::EndDragDropTarget();
            }
        }
    }

    int LoadStyles() override {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 2));
        return 2;
    }

    bool IsNodeOpen(Entity entity) {
        if (!expanded.contains(entity))
            return true;
        return expanded[entity];
    }

    std::vector<VisibleNode> VisibleNodes;
    std::unordered_map<Entity, bool> expanded;
    bool visibleNodesDirty = true;
};