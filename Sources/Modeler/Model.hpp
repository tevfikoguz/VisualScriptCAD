#ifndef MODELER_MODEL_HPP
#define MODELER_MODEL_HPP

#include "Checksum.hpp"
#include "IncludeGLM.hpp"
#include "Mesh.hpp"
#include "UserData.hpp"
#include "SharedData.hpp"
#include "BoundingShapes.hpp"

#include <vector>
#include <unordered_map>
#include <memory>

namespace Modeler
{

using MeshGeometryId = int;
using MeshMaterialsId = int;
using MeshId = int;

class MeshRef
{
public:
	MeshRef (MeshGeometryId geometryId, MeshMaterialsId& materialsId, const glm::dmat4& transformation);

	MeshGeometryId		GetGeometryId () const;
	MeshMaterialsId		GetMaterialsId () const;
	const glm::dmat4&	GetTransformation () const;

	void				SetUserData (const std::string& key, const UserDataConstPtr& data);
	UserDataConstPtr	GetUserData (const std::string& key) const;

private:
	MeshGeometryId		geometryId;
	MeshMaterialsId		materialsId;
	glm::dmat4			transformation;
	UserDataCollection	userData;
};

class ModelInfo
{
public:
	unsigned int meshGeometryCount = 0;
	unsigned int meshMaterialsCount = 0;
	unsigned int meshCount = 0;
	unsigned int vertexCount = 0;
	unsigned int triangleCount = 0;
};

class Model
{
public:
	Model ();
	Model (const Model& rhs) = delete;
	Model (Model&& rhs) = delete;
	
	Model&						operator= (const Model& rhs) = delete;
	Model&						operator= (Model&& rhs) = delete;

	const MeshGeometry&			GetMeshGeometry (const MeshRef& meshRef) const;
	const MeshMaterials&		GetMeshMaterials (const MeshRef& meshRef) const;

	const MeshRef&				GetMesh (MeshId meshId) const;
	void						EnumerateMeshes (const std::function<void (MeshId, const MeshRef&)>& processor) const;

	MeshId						AddMesh (const Mesh& mesh);
	void						SetMeshUserData (MeshId meshId, const std::string& key, const UserDataConstPtr& data);
	void						RemoveMesh (MeshId meshId);
	void						Clear ();

	ModelInfo					GetInfo () const;
	Geometry::BoundingBox		GetBoundingBox () const;
	Geometry::BoundingSphere	GetBoundingSphere () const;

private:
	SharedData<MeshGeometryId, MeshGeometry>	geometries;
	SharedData<MeshMaterialsId, MeshMaterials>	materials;
	std::unordered_map<MeshId, MeshRef>			meshRefs;

	MeshId		nextMeshId;
	MaterialId	nextMaterialId;
};

}

#endif
