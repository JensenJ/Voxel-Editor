#pragma once
#include <concepts>

template <typename T>
concept EditorRenderable = requires(T t) {
    { t.RenderComponentPanel() } -> std::same_as<void>;
};
