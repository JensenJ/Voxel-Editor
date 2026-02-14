#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Camera.h>
#include <Voxel/Rendering/RawModelRenderer.h>

class RenderSystem {
  public:
    static void Init(Application* app, Camera* cam, EntityRegistry* registry) {
        application = app;
        camera = cam;
        entityRegistry = registry;
        LOG_INFO("Initialised RenderSystem");
    }

    static void Run();

  private:
    static inline Camera* camera = nullptr;
    static inline Application* application = nullptr;
    static inline EntityRegistry* entityRegistry = nullptr;
    static inline RawModelRenderer rawModelRenderer = RawModelRenderer();
};