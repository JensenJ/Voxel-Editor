#pragma once
#include "Component.h"

class TransformComponent : public Component
{
public:
	void OnStart() override;
	void OnUpdate() override;
	void OnDestroy() override;
};

