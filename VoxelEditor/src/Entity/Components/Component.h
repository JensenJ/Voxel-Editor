#pragma once
class Component
{
public:
	virtual void OnStart() {}
	virtual void OnUpdate() {}
	virtual void OnDestroy() {}

	void SetOwningEntityID(unsigned int entityID) { owningEntityID = entityID; }
	unsigned int GetOwningEntityID() { return owningEntityID; }
private:
	unsigned int owningEntityID = -1;
};

