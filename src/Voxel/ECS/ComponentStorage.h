#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>

template <typename T> class ComponentStorage {
  public:
    template <typename... Args> T& Add(Entity e, Args&&... args) {
        auto it = Find(e);
        if (it != components.end())
            return *it;

        it = components.insert(it, T(std::forward<Args>(args)...));
        return *it;
    }

    bool Remove(Entity e) {
        auto it = Find(e);
        if (it == components.end())
            return false;
        components.erase(it);
        return true;
    }

    T* Get(Entity e) {
        auto it = Find(e);
        return it != components.end() ? &*it : nullptr;
    }

    const T* Get(Entity e) const {
        auto it = Find(e);
        return it != components.end() ? &*it : nullptr;
    }

    bool Has(Entity e) const { return Find(e) != components.end(); }
    const std::vector<T>& All() const { return components; }

    T& operator[](size_t index) { return components[index]; }
    const T& operator[](size_t index) const { return components[index]; }

    size_t Size() const { return components.size(); }

  private:
    std::vector<T> components;

    auto Find(Entity e) {
        return std::lower_bound(components.begin(), components.end(), e,
                                [](const T& c, Entity e) { return c.entity < e; });
    }

    auto Find(Entity e) const {
        return std::lower_bound(components.begin(), components.end(), e,
                                [](const T& c, Entity e) { return c.entity < e; });
    }
};