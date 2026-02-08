#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/ComponentStorage.h>

template <typename... Ts> class View {
  public:
    View(ComponentStorage<std::remove_const_t<Ts>>&... storages) : m_storages(&storages...) {
        m_primary = FindSmallestStorage();
    }

    class Iterator {
      public:
        Iterator(View* view, size_t index) : m_view(view), m_index(index) { AdvanceToValid(); }

        Iterator& operator++() {
            ++m_index;
            AdvanceToValid();
            return *this;
        }

        bool operator!=(const Iterator& other) const { return m_index != other.m_index; }

        auto operator*() {
            if (m_index >= m_view->PrimarySize())
                throw std::out_of_range("Iterator out of range in View::operator*");

            Entity e = m_view->GetPrimaryEntity(m_index);
            return std::tuple<Entity, Ts&...>(
                e, *std::get<ComponentStorage<std::remove_const_t<Ts>>*>(m_view->m_storages)
                        ->Get(e)...);
        }

      private:
        void AdvanceToValid() {
            while (m_index < m_view->PrimarySize()) {
                Entity e = m_view->GetPrimaryEntity(m_index);
                if (m_view->HasAll(e))
                    return;
                ++m_index;
            }
        }

        View* m_view;
        size_t m_index;
    };

    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, PrimarySize()); }

  private:
    // ALWAYS store non-const storage pointers
    std::tuple<ComponentStorage<std::remove_const_t<Ts>>*...> m_storages;
    size_t m_primary = 0;

    size_t PrimarySize() const {
        return std::apply([&](auto*... s) { return std::min({s->Size()...}); }, m_storages);
    }

    Entity GetPrimaryEntity(size_t index) const {
        size_t i = 0;
        Entity result = 0;

        std::apply(
            [&](auto*... s) {
                ((i == m_primary ? (result = (*s)[index].entity, 0) : 0, ++i), ...);
            },
            m_storages);

        return result;
    }

    bool HasAll(Entity e) const {
        return std::apply([&](auto*... s) { return (s->Has(e) && ...); }, m_storages);
    }

    size_t FindSmallestStorage() const {
        size_t index = 0;
        size_t minSize = SIZE_MAX;
        size_t i = 0;

        std::apply(
            [&](auto*... s) {
                ((s->Size() < minSize ? (minSize = s->Size(), index = i) : 0, ++i), ...);
            },
            m_storages);

        return index;
    }
};