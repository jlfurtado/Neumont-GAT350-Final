#include "GraphicalObject.h"
#include "GameLogger.h"
#include "MyGL.h"

// Justin Furtado
// 7/6/2016
// GraphicalObject.cpp
// Represents something drawn to the screen

namespace Engine
{
	GraphicalObject::GraphicalObject()
		: m_pMesh(nullptr), m_rotation(0.0f), m_rotationAxis(Vec3(0.0f, 1.0f, 0.0f)), m_rotationMatrix(Mat4()),
		m_scaleMatrix(Mat4()), m_rotationRate(0.0f), m_scaleRate(0.0f), m_translationMatrix(Mat4()), m_velocity(Vec3()),
		m_enabled(true), m_pNextCollidingNode(nullptr), m_pNextDrawingNode(nullptr)
	{
	}

	GraphicalObject::~GraphicalObject()
	{
	}

	Mat4 GraphicalObject::GetScaleMat()
	{
		return m_scaleMatrix;
	}

	Mat4 GraphicalObject::GetTransMat()
	{
		return m_translationMatrix;
	}

	Vec3 GraphicalObject::GetPos()
	{
		return Vec3(m_translationMatrix.GetAddress()[12], m_translationMatrix.GetAddress()[13], m_translationMatrix.GetAddress()[14]);
	}

	Mat4 GraphicalObject::GetRotMat()
	{
		return m_rotationMatrix;
	}

	Vec3 GraphicalObject::GetRotationAxis()
	{
		return m_rotationAxis;
	}

	Vec3 GraphicalObject::GetVelocity()
	{
		return m_velocity;
	}

	float * GraphicalObject::GetLocPtr()
	{
		return m_pos.GetAddress();
	}

	GLfloat GraphicalObject::GetRotation()
	{
		return m_rotation;
	}

	GLfloat GraphicalObject::GetRotationRate()
	{
		return m_rotationRate;
	}

	Mesh * GraphicalObject::GetMeshPointer()
	{
		return m_pMesh;
	}

	GraphicalObject * GraphicalObject::GetNextDrawingNode()
	{
		return m_pNextDrawingNode;
	}

	GraphicalObject * GraphicalObject::GetNextCollidingNode()
	{
		return m_pNextCollidingNode;
	}

	Material * GraphicalObject::GetMatPtr()
	{
		return &m_material;
	}

	void ** GraphicalObject::GetUniformDataPtrPtrByLoc(int loc)
	{
		for (int i = 0; i < m_numUniformDatas; ++i)
		{
			if (m_uniformData[i].GetUniformDataLoc() == loc)
			{
				return m_uniformData[i].GetUniformDataPtrPtr();
			}
		}

		return nullptr;
	}

	bool GraphicalObject::IsEnabled()
	{
		return m_enabled;
	}

	void GraphicalObject::CallCallback()
	{
		// only call the callback if it has been set
		if (m_callback)
		{
			m_callback(this, m_classInstance);
		}
	}

	void GraphicalObject::SetCallback(GraphicalObjectCallback callback, void *classInstance)
	{
		m_callback = callback;
		m_classInstance = classInstance;
	}

	void GraphicalObject::PassUniforms()
	{
		for (int i = 0; i < m_numUniformDatas; ++i)
		{
			m_uniformData[i].PassUniform();
			if (MyGL::TestForError(MessageType::cError, "Uniform data [%d] for object at (%.3f, %.3f, %.3f) failed", i, GetPos().GetX(), GetPos().GetY(), GetPos().GetZ()))
			{ break; }
		}
	}

	void GraphicalObject::AddUniformData(UniformData dataToAdd)
	{
		if (m_numUniformDatas >= MAX_UNFIFORM_DATA) { GameLogger::Log(MessageType::cError, "Ran out of space for uniform data!\n"); return; }
		m_uniformData[m_numUniformDatas++] = dataToAdd;
	}

	void GraphicalObject::SetScaleMat(Mat4 scale)
	{
		m_scaleMatrix = scale;
	}

	void GraphicalObject::SetTransMat(Mat4 trans)
	{
		m_translationMatrix = trans;
		Engine::Vec3 p = GetPos();
		m_pos = Engine::Vec4(p.GetX(), p.GetY(), p.GetZ(), m_translationMatrix.GetAddress()[15]);
	}

	void GraphicalObject::SetRotMat(Mat4 rot)
	{
		m_rotationMatrix = rot;
	}

	void GraphicalObject::SetRotationAxis(Vec3 axis)
	{
		m_rotationAxis = axis;
	}

	void GraphicalObject::SetVelocity(Vec3 vel)
	{
		m_velocity = vel;
	}

	void GraphicalObject::SetRotation(GLfloat rot)
	{
		m_rotation = rot;
	}

	void GraphicalObject::SetRotationRate(GLfloat rate)
	{
		m_rotationRate = rate;
	}

	void GraphicalObject::SetMeshPointer(Mesh * pMesh)
	{
		if (!pMesh) { GameLogger::Log(MessageType::cWarning, "Invalid mesh pointer passed in for graphical object, ignoring!\n"); return; }
		m_pMesh = pMesh;
	}

	void GraphicalObject::PointToCollidingNode(GraphicalObject * nextNode)
	{
		m_pNextCollidingNode = nextNode;
	}

	void GraphicalObject::SetW(float w)
	{
		m_pos = Engine::Vec4(m_pos.GetX(), m_pos.GetY(), m_pos.GetZ(), w);
	}

	void GraphicalObject::AddPhongUniforms(int mtwLoc, int wtvLoc, void *wtvPtr, int projLoc, void *projPtr, int colorLoc,
										   int diffuseReflecLoc, int ambientReflectLoc, int specularReflectLoc, int specularPowerLoc,
										   int diffuseIntensityLoc, int ambientIntensityLoc, int specularIntensityLoc, void *lightIntensityPtr, 
										   int camPosLoc, void *camPosPtr, int lightPosLoc, void *lightPosPtr)
	{
		this->AddUniformData(UniformData(GL_FLOAT_MAT4, &m_fullTransform, mtwLoc));
		this->AddUniformData(UniformData(GL_FLOAT_MAT4, wtvPtr, wtvLoc));
		this->AddUniformData(UniformData(GL_FLOAT_MAT4, projPtr, projLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, &m_material.m_materialColor, colorLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, &m_material.m_diffuseReflectivity, diffuseReflecLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, &m_material.m_ambientReflectivity, ambientReflectLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, &m_material.m_specularReflectivity, specularReflectLoc));
		this->AddUniformData(UniformData(GL_FLOAT, &m_material.m_specularIntensity, specularPowerLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, lightIntensityPtr, diffuseIntensityLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, lightIntensityPtr, ambientIntensityLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, lightIntensityPtr, specularIntensityLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, camPosPtr, camPosLoc));
		this->AddUniformData(UniformData(GL_FLOAT_VEC3, lightPosPtr, lightPosLoc));

	}

	void GraphicalObject::PointToDrawingNode(GraphicalObject * nextNode)
	{
		m_pNextDrawingNode = nextNode;
	}

	void GraphicalObject::SetEnabled(bool visible)
	{
		m_enabled = visible;
	}
	
	UniformData GraphicalObject::GetUniformData(int index)
	{
		return m_uniformData[index];
	}

	void GraphicalObject::SetMaterial(Material mat)
	{
		m_material = mat;
	}

	void GraphicalObject::CalcFullTransform()
	{
		m_fullTransform = m_translationMatrix * (m_scaleMatrix * m_rotationMatrix);
	}

	Mat4 * GraphicalObject::GetFullTransformPtr()
	{
		return &m_fullTransform;
	}
}

