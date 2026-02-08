#include "EntityRegistry.h"

EntityRegistry* EntityRegistry::instance = nullptr;

EntityRegistry* EntityRegistry::GetInstance() {
    if (instance == nullptr) {
        instance = new EntityRegistry();
    }
    return instance;
}
