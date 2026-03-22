#pragma once
#include <defines.hpp>

#include <engine/resources/resource.hpp>
#include <engine/resources/graphics/geometry.hpp>
#include <math/vector.hpp>
#include <vector>

#define SM_DEFAULT_MESH_NAME "DEFAULT_MESH"

BEGIN_NAMESPACE

struct Mesh : Resource
{
    using Resource::Resource;

    void OnUnload() override;
    void OnLoad() override;

    std::vector<Geometry*> m_Geometries;
};

END_NAMESPACE