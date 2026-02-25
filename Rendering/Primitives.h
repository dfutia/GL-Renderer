#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "Mesh.h"

namespace Primitives
{
    Mesh CreateCube();
    Mesh CreatePlane(float size = 1.0f);
    Mesh CreateSphere(int segments = 32, int rings = 16);
    Mesh CreateCylinder(float radius = 1.0f, float height = 2.0f, int segments = 32);
}

#endif