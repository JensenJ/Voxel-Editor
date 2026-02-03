#pragma once
#include <string>

class Component {
  public:
    virtual void OnStart() {}
    virtual void OnUpdate(float deltaTime);
    virtual void OnDestroy() {}

    std::string GetComponentName() { return componentName; }
    bool ShouldRenderProperties() { return shouldRenderProperties; }
    virtual void RenderPropertiesPanel() {};

    void SetOwningEntityID(unsigned int entityID) { owningEntityID = entityID; }
    unsigned int GetOwningEntityID() { return owningEntityID; }

  private:
    unsigned int owningEntityID = -1;

  protected:
    float deltaTime = 0;
    std::string componentName = "";
    bool shouldRenderProperties = false;
};
