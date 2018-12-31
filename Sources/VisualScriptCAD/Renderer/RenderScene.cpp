#include "RenderScene.hpp"
#include "ShaderProgram.hpp"
#include "IncludeGLM.hpp"

const char *vertexShaderSource = R"(
#version 330 core
precision mediump float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mediump mat4 modelMatrix;
uniform mediump mat4 viewMatrix;
uniform mediump mat4 projectionMatrix;
uniform mediump mat3 normalMatrix;

varying mediump vec3 fragmentNormal;
varying mediump vec3 fragmentPosition;

void main()
{
	fragmentNormal = normalize (normalMatrix * aNormal);
	fragmentPosition = vec3 (modelMatrix * vec4 (aPos, 1.0));
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4 (aPos, 1.0);
}
)";

const char *fragmentShaderSource = R"(
#version 330 core
precision mediump float;

uniform mediump vec3 viewPosition;
uniform mediump vec3 materialColor;

uniform mediump vec3 lightPosition;
uniform mediump vec3 lightColor;
uniform mediump float lightAmbientStrength;
uniform mediump float lightSpecularStrength;

varying mediump vec3 fragmentNormal;
varying mediump vec3 fragmentPosition;

out vec4 fragmentColor;

mediump float materialShininess = 32.0;

void main()
{
	mediump vec3 ambient = lightAmbientStrength * lightColor;
  	
    mediump vec3 norm = normalize (fragmentNormal);
    mediump vec3 lightDirection = normalize (lightPosition - fragmentPosition);
    mediump float diffuseValue = max (dot (norm, lightDirection), 0.0);
    mediump vec3 diffuse = diffuseValue * lightColor;
    
    mediump vec3 viewDirection = normalize (viewPosition - fragmentPosition);
    mediump vec3 reflectedDirection = reflect (-lightDirection, norm);  
    mediump float specularValue = pow (max (dot (viewDirection, reflectedDirection), 0.0), materialShininess);
    mediump vec3 specular = lightSpecularStrength * specularValue * lightColor;  

	mediump vec3 result = (ambient + diffuse + specular) * materialColor;
	fragmentColor = vec4 (result, 1.0f);
}
)";

static void EnumerateAllTransformedVertices (const RenderModel& renderModel, const std::function<void (const glm::vec3&)>& processor)
{
	renderModel.EnumerateRenderMeshes ([&] (const RenderMesh& renderMesh) {
		renderMesh.EnumerateInstances ([&] (const RenderMeshInstance& instance) {
			const glm::mat4& transformation = instance.GetTransformation ();
			renderMesh.EnumerateRenderGeometries ([&] (const RenderGeometry& renderGeometry) {
				renderGeometry.EnumerateVertices ([&] (const glm::vec3& vertex) {
					glm::vec4 vertex4 (vertex, 1.0);
					glm::vec4 transformed4 = transformation * vertex4;
					processor (glm::vec3 (transformed4.x, transformed4.y, transformed4.z));
				});
			});
		});
	});
}

RenderMaterial::RenderMaterial (const glm::vec3& color) :
	color (color)
{

}

RenderLight::RenderLight () :
	lightColor (0.7f, 0.7f, 0.7f),
	lightAmbientStrength (0.6f),
	lightSpecularStrength (0.3f)
{

}

RenderGeometry::RenderGeometry (const RenderMaterial& triangleMaterial) :
	triangleMaterial (triangleMaterial),
	vertexArrayObject ((unsigned int) -1),
	vertexBufferObject ((unsigned int) -1),
	normalBufferObject ((unsigned int) -1)
{

}

RenderGeometry::~RenderGeometry ()
{
	if (vertexArrayObject != -1) {
		glDeleteVertexArrays (1, &vertexArrayObject);
		glDeleteBuffers (1, &vertexBufferObject);
		glDeleteBuffers (1, &normalBufferObject);
	}
}

void RenderGeometry::AddTriangle (	const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
								const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3)
{
	AddVertex (v1);
	AddVertex (v2);
	AddVertex (v3);
	AddNormal (n1);
	AddNormal (n2);
	AddNormal (n3);
}

void RenderGeometry::AddVertex (const glm::vec3& v)
{
	triangleVertices.push_back (v.x);
	triangleVertices.push_back (v.y);
	triangleVertices.push_back (v.z);
}

void RenderGeometry::AddNormal (const glm::vec3& n)
{
	triangleNormals.push_back (n.x);
	triangleNormals.push_back (n.y);
	triangleNormals.push_back (n.z);
}

const RenderMaterial& RenderGeometry::GetMaterial () const
{
	return triangleMaterial;
}

void RenderGeometry::SetupBuffers () const
{
	if (vertexArrayObject != -1) {
		return;
	}

	if (triangleVertices.empty () || triangleNormals.empty ()) {
		return;
	}

	glGenVertexArrays (1, &vertexArrayObject);
	glGenBuffers (1, &vertexBufferObject);
	glGenBuffers (1, &normalBufferObject);

	glBindVertexArray (vertexArrayObject);

	const float* triangleVertexArray = &triangleVertices[0];
	glBindBuffer (GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData (GL_ARRAY_BUFFER, triangleVertices.size () * sizeof (float), triangleVertexArray, GL_STATIC_DRAW);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*) 0);
	glEnableVertexAttribArray (0);

	const float* triangleNormalArray = &triangleNormals[0];
	glBindBuffer (GL_ARRAY_BUFFER, normalBufferObject);
	glBufferData (GL_ARRAY_BUFFER, triangleNormals.size () * sizeof (float), triangleNormalArray, GL_STATIC_DRAW);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void*) 0);
	glEnableVertexAttribArray (1);

	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindVertexArray (0);
}

void RenderGeometry::DrawBuffers () const
{
	if (vertexArrayObject == -1) {
		return;
	}

	unsigned int triangleCount = (unsigned int) triangleVertices.size () / 3;
	glBindVertexArray (vertexArrayObject);
	glDrawArrays (GL_TRIANGLES, 0, triangleCount);
}

void RenderGeometry::EnumerateVertices (const std::function<void (const glm::vec3&)>& processor) const
{
	for (size_t i = 0; i < triangleVertices.size (); i += 3) {
		processor (glm::vec3 (triangleVertices[i], triangleVertices[i + 1], triangleVertices[i + 2]));
	}
}

RenderMeshInstance::RenderMeshInstance (const glm::mat4& transformation) :
	transformation (transformation)
{
}

const glm::mat4& RenderMeshInstance::GetTransformation () const
{
	return transformation;
}

RenderMesh::RenderMesh ()
{

}

void RenderMesh::AddRenderGeometry (const RenderGeometry& geometry)
{
	geometries.push_back (geometry);
}

size_t RenderMesh::RenderGeometryCount () const
{
	return geometries.size ();
}

RenderGeometry& RenderMesh::GetRenderGeometry (size_t index)
{
	return geometries[index];
}

bool RenderMesh::ContainsInstance (RenderMeshInstanceId instanceId) const
{
	return instances.find (instanceId) != instances.end ();
}

size_t RenderMesh::InstanceCount () const
{
	return instances.size ();
}

void RenderMesh::AddInstance (RenderMeshInstanceId instanceId, const RenderMeshInstance& instance)
{
	instances.insert ({ instanceId, instance });
}

void RenderMesh::RemoveInstance (RenderMeshInstanceId instanceId)
{
	instances.erase (instanceId);
}

void RenderMesh::EnumerateRenderGeometries (const std::function<void (const RenderGeometry&)>& processor) const
{
	for (const RenderGeometry& mesh : geometries) {
		processor (mesh);
	}
}

void RenderMesh::EnumerateInstances (const std::function<void (const RenderMeshInstance&)>& processor) const
{
	for (const auto& it : instances) {
		processor (it.second);
	}
}

RenderModel::RenderModel ()
{

}

void RenderModel::AddRenderMesh (RenderMeshId meshId, const RenderMesh& renderMesh)
{
	renderMeshes.insert ({ meshId, renderMesh });
}

void RenderModel::RemoveRenderMesh (RenderMeshId meshId)
{
	renderMeshes.erase (meshId);
}

void RenderModel::AddRenderMeshInstance (RenderMeshId meshId, RenderMeshInstanceId instanceId, const RenderMeshInstance& instance)
{
	RenderMesh& renderMesh = renderMeshes.at (meshId);
	renderMesh.AddInstance (instanceId, instance);
	instanceIdToMeshId.insert ({ instanceId, meshId });
}

void RenderModel::RemoveRenderMeshInstance (RenderMeshInstanceId instanceId)
{
	RenderMeshId renderMeshId = instanceIdToMeshId.at (instanceId);
	RenderMesh& renderMesh = renderMeshes.at (renderMeshId);
	renderMesh.RemoveInstance (instanceId);
	instanceIdToMeshId.erase (instanceId);
	if (renderMesh.InstanceCount () == 0) {
		RemoveRenderMesh (renderMeshId);
	}
}

RenderMeshId RenderModel::GetRenderMeshId (RenderMeshInstanceId instanceId) const
{
	return instanceIdToMeshId.at (instanceId);
}

const RenderMesh& RenderModel::GetRenderMesh (RenderMeshId meshId) const
{
	return renderMeshes.at (meshId);
}

bool RenderModel::ContainsRenderMesh (RenderMeshId meshId) const
{
	return renderMeshes.find (meshId) != renderMeshes.end ();
}

void RenderModel::EnumerateRenderMeshes (const std::function<void (const RenderMesh&)>& processor) const
{
	for (const auto& it : renderMeshes) {
		processor (it.second);
	}
}

void RenderModel::Clear ()
{
	renderMeshes.clear ();
	instanceIdToMeshId.clear ();
}

RenderScene::RenderScene () :
	shader (-1),
	model (),
	light (),
	camera (glm::vec3 (-2.0, -3.0, 1.5), glm::vec3 (0.0, 0.0, 0.0), glm::vec3 (0.0, 0.0, 1.0), 45.0f, 0.1f, 10000.0f)
{

}

bool RenderScene::Init ()
{
	if (!gladLoadGL ()) {
		return false;
	}

	shader = CreateShaderProgram (vertexShaderSource, fragmentShaderSource);
	if (shader < 0) {
		return false;
	}

	glEnable (GL_DEPTH_TEST);
	return true;
}

void RenderScene::Draw (int width, int height, ViewMode drawMode)
{
	glViewport (0, 0, width, height);

	glUseProgram (shader);

	glClearColor (0.9f, 0.9f, 0.9f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (drawMode == ViewMode::Lines) {
		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	} else if (drawMode == ViewMode::Polygons) {
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	} else {
		throw std::logic_error ("invalid view mode");
	}

	glm::mat4 viewMatrix = camera.GetViewMatrix ();
	glm::mat4 projectionMatrix = camera.GetProjectionMatrix (width, height);

	glm::vec3 lightPosition = camera.GetEye ();
	glUniform3fv (glGetUniformLocation (shader, "lightPosition"), 1, &lightPosition[0]);
	glUniform3fv (glGetUniformLocation (shader, "lightColor"), 1, &light.lightColor[0]);
	glUniform1f (glGetUniformLocation (shader, "lightAmbientStrength"), light.lightAmbientStrength);
	glUniform1f (glGetUniformLocation (shader, "lightSpecularStrength"), light.lightSpecularStrength);

	glm::vec3 viewPosition = camera.GetEye ();
	glUniform3fv (glGetUniformLocation (shader, "viewPosition"), 1, &viewPosition[0]);
	glUniformMatrix4fv (glGetUniformLocation (shader, "viewMatrix"), 1, GL_FALSE, glm::value_ptr (viewMatrix));
	glUniformMatrix4fv (glGetUniformLocation (shader, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr (projectionMatrix));

	GLint materialColorLocation = glGetUniformLocation (shader, "materialColor");
	GLint modelMatrixLocation = glGetUniformLocation (shader, "modelMatrix");
	GLint normalMatrixLocation = glGetUniformLocation (shader, "normalMatrix");
	model.EnumerateRenderMeshes ([&] (const RenderMesh& renderMesh) {
		renderMesh.EnumerateRenderGeometries ([&] (const RenderGeometry& renderGeometry) {
			const RenderMaterial& material = renderGeometry.GetMaterial ();
			glUniform3fv (materialColorLocation, 1, &material.color[0]);
			renderGeometry.SetupBuffers ();
			renderMesh.EnumerateInstances ([&] (const RenderMeshInstance& meshInstance) {
				glm::mat3 normalMatrix = glm::transpose (glm::inverse (glm::mat3 (meshInstance.GetTransformation ())));
				glUniformMatrix4fv (modelMatrixLocation, 1, GL_FALSE, glm::value_ptr (meshInstance.GetTransformation ()));
				glUniformMatrix3fv (normalMatrixLocation, 1, GL_FALSE, glm::value_ptr (normalMatrix));
				renderGeometry.DrawBuffers ();
			});
		});
	});
}

RenderModel& RenderScene::GetModel ()
{
	return model;
}

void RenderScene::OnMouseMove (MouseButton mouseButton, int diffX, int diffY)
{
	if (mouseButton == MouseButton::Left) {
		camera.Orbit ((float) -diffX, (float) -diffY);
	} else if (mouseButton == MouseButton::Right) {
		camera.Pan ((float) -diffX, (float) diffY);
	}
}

void RenderScene::OnMouseWheel (int rotation)
{
	float ratio = (rotation > 0 ? 0.1f : -0.1f);
	camera.Zoom (ratio);
}

void RenderScene::FitToWindow (int width, int height)
{
	static const float maxFloat = std::numeric_limits<float>::max ();
	
	bool hasVertex = false;
	
	glm::vec3 boxMin (maxFloat, maxFloat, maxFloat);
	glm::vec3 boxMax (-maxFloat, -maxFloat, -maxFloat);
	EnumerateAllTransformedVertices (model, [&] (const glm::vec3& vertex) {
		hasVertex = true;
		boxMin = glm::min (vertex, boxMin);
		boxMax = glm::max (vertex, boxMax);
	});
	
	if (!hasVertex) {
		return;
	}
	
	float radius = 0.0;
	glm::vec3 center = (boxMin + boxMax) / 2.0f;
	EnumerateAllTransformedVertices (model, [&] (const glm::vec3& vertex) {
		radius = glm::max (radius, glm::distance (center, vertex));
	});
	
	if (radius == 0.0) {
		return;
	}
	
	camera.ZoomToSphere (center, radius, width, height);
}