#include "MeshRendererComponent.h"

void MeshRendererComponent::SetMesh(RawModel* mesh) { meshToRender = mesh; }

MeshRendererComponent::MeshRendererComponent(RawModel* mesh) { SetMesh(mesh); }

RawModel* MeshRendererComponent::GetMesh() { return meshToRender; }
