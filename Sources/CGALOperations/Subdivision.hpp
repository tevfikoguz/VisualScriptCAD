#ifndef CGAL_SUBDIVISION_HPP
#define CGAL_SUBDIVISION_HPP

#include "Shape.hpp"
#include "Mesh.hpp"

namespace CGALOperations
{

bool					MeshSubdivision (const Modeler::Mesh& mesh, const Modeler::Material& material, int steps, Modeler::Mesh& resultMesh);
Modeler::ShapePtr		MeshSubdivision (const Modeler::ShapeConstPtr& shape, const Modeler::Material& material, int steps);

}

#endif
