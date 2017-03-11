#include "EngineDemo.h"
#include "MousePicker.h"
#include "MyWindow.h"
#include <iostream>

#include "InstanceBuffer.h"
#include "MouseManager.h"
#include "GameLogger.h"
#include "MyGL.h"
#include "ChaseCameraComponent.h"
#include "CollisionTester.h"
#include "MyFiles.h"
#include "ShaderProgram.h"
#include "BitmapLoader.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#pragma warning(disable : 4127)
#include <QtGui\qmouseevent>
#pragma warning(pop)

#include "ShapeGenerator.h"

#include "RenderEngine.h"
#include "ConfigReader.h"
#include "MathUtility.h"
#include "GraphicalObjectComponent.h"
#include "SpatialComponent.h"
#include "KeyboardComponent.h"
#include "MouseComponent.h"
#include "UniformData.h"
#include "FrameBuffer.h"

// Justin Furtado
// 6/21/2016
// EngineDemo.cpp
// The game

const float dargonSpacing = 115.0f;
const int DARGONS_PER_ROW = 4;
const int NUM_DARGONS_DEMO1 = DARGONS_PER_ROW * DARGONS_PER_ROW;
const int NUM_DRAGONS_DEMO2 = DARGONS_PER_ROW * DARGONS_PER_ROW;
const float DARGON_PLANE_OFFSET = 1000.0f;
const float LIGHT_HEIGHT = 15.0f;
const float degreesPerSecond = 10.0f;
const int NUM_SCENES = 6;
const float SCENE_PLANE_SCALE = 10.0f;
const int NUM_USED_SHADERS = 10;
const int NUM_NON_TEAPOT_SHADERS = 4;
const int TEAPOTS_PER_SHADER = 3;
const int NUM_DARGONS_TOTAL = NUM_DARGONS_DEMO1 + NUM_DRAGONS_DEMO2;
const int NUM_DEMO_OBJECTS = NUM_DARGONS_TOTAL + 2;
const float TEAPOT_DISTANCE = 35.0f;
const float ROTATE_DISTANCE = TEAPOT_DISTANCE / 2.5f;
Engine::GraphicalObject m_grid;
Engine::GraphicalObject m_demoObjects[NUM_DEMO_OBJECTS];
Engine::GraphicalObject m_lights[NUM_DEMO_OBJECTS];
Engine::GraphicalObject m_scenePlanes[NUM_SCENES];
Engine::GraphicalObject playerGraphicalObject;
//Engine::GraphicalObject cube;
const float EngineDemo::RENDER_DISTANCE = 2500.0f;
Engine::Entity player;
Engine::ChaseCameraComponent playerCamera(Engine::Vec3(0, 30, 50), Engine::Vec3(0, 5, 0), Engine::Vec3(0));
Engine::GraphicalObjectComponent playerGob;
Engine::SpatialComponent playerSpatial;
MouseComponent mouseComponent;
KeyboardComponent playerInput;
Engine::Vec3 spotlightDir(0.0f, -1.0f, 0.0f);
float spotlightAttenuation = 1.0f, spotlightCutoff = Engine::MathUtility::ToRadians(45.0f);
int numCelLevels = 4;
Engine::Vec3 backgroundColor(0.0f);
Engine::Vec3 planeColor(1.0f);
float fogMinDist = 70.0f, fogMaxDist = 250.0f;
//Engine::GraphicalObject m_crate;
int mossTexID;
int alphaTexID;

const int framebufferWidth = 2048;
const int framebufferHeight = 2048;
const float framebufferAspect = framebufferWidth * 1.0f / framebufferHeight;
Engine::Mat4 framebufferLookAt;
Engine::Mat4 framebufferPerspective;
Engine::FrameBuffer nearestBuffer;
Engine::FrameBuffer linearBuffer;
Engine::FrameBuffer *pCurrentBuffer;
int whiteSquareId = 0;
Engine::GraphicalObject framebufferCamera;
Engine::Camera fbCam;
float halfWidth = 2.0f;
int fractalGradientTextureID{ 0 };
int fractalGradientAlternateTextureID{ 0 };

const float cubeSize = 1000.0f;
const int numParticles = 100000;

Engine::Mat4 vpm;
int vpmLoc;
Engine::Vec3 dontUseMe(0.0f);

float lineWidth = 0.1f;
Engine::Vec4 lineColor(0.0f);
int lineWidthLoc;
int lineColorLoc;

float hairLength = 3.0f;
int hairLengthLoc;

Engine::Vec3 triangleColor(1.0f, 0.0f, 0.0f);
Engine::Vec3 faceNormalColor(0.0f, 1.0f, 0.0f);
Engine::Vec3 vertexNormalColor(0.0f, 0.0f, 1.0f);
int vColorLoc, fColorLoc;

int cycleHairMode = 0;
int cycleHairModeLoc;

float explodeDist = 0.0f;
int explodeDistLoc;

Engine::Mat4 shadowMatrix;
int shadowMatrixLoc;

int bufferTexID;
Engine::FrameBuffer volumetricBuffer;

int geomSubIndex = 0;
int fragSubIndex = 0;
Engine::Mat4 identity;

bool plane = false;
Engine::Vec3 eyeLightVal;
int eyeLightPosLoc;
Engine::Vec3 zeroVec(0.0f);

float screenToTexWidth;
float screenToTexHeight;

Engine::Mat4 persp;

float amplitude = 0.0f;
float wavenumber = 0.0f;
float velocity = 0.0f;
float time = 0.0f;
int ampLoc;
int wavLoc;
int velLoc;
int timeLoc;
int texID;
int lifeTimeLoc;
float lifeTime;
Engine::Vec3 gravity;
int gravityLoc;
Engine::Vec3 darginVelocity;
int darginVelLoc;
bool drawDemoQuads = false;
int instanceCycleIndex = 0;
Engine::InstanceBuffer instanceBuffer;

int numRows = 10;
int numColumns = 10;
int rowsLoc;
int columnsLoc;
float width = 0.5f;
float height = 0.5f;
int widthLoc;
int heightLoc;

Engine::Vec3 rowColor(1.0f);
Engine::Vec3 columnColor(0.0f);
int rowColLoc;
int colColLoc;

const int instanceCycles = 3;
Engine::InstanceBuffer asteroids;

bool EngineDemo::Initialize(Engine::MyWindow *window)
{
	m_pWindow = window;

	if (!InitializeGL())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize game! Failed to InitializeGL()!\n");
		return false;
	}

	if (!Engine::RenderEngine::Initialize(&m_shaderPrograms[0], NUM_SHADER_PROGRAMS))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Could not do anything because RenderEngine failed...\n");
		return false;
	}

	if (!Engine::ShapeGenerator::Initialize(m_shaderPrograms[2].GetProgramId(), m_shaderPrograms[0].GetProgramId(), m_shaderPrograms[5].GetProgramId()))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to initialize game because shape generator failed to initialize!\n");
		return false;
	}

	if (!UglyDemoCode()) { return false; }

	if (!ReadConfigValues())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize EngineDemo, failed to read config values!\n");
		return false;
	}

	if (!Engine::ConfigReader::pReader->RegisterCallbackForConfigChanges(EngineDemo::OnConfigReload, this))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to register callback for EngineDemo!\n");
		return false;
	}

	// ` for numpad 0
	if (!keyboardManager.AddKeys("XTWASDRFLGCM QE012345678`9iKNJB")
		|| !keyboardManager.AddKey(VK_OEM_4) || !keyboardManager.AddKey(VK_OEM_6) || !keyboardManager.AddKey(VK_OEM_5)
		|| !keyboardManager.AddKey(VK_PRIOR) || !keyboardManager.AddKey(VK_NEXT)
		|| !keyboardManager.AddKey(VK_OEM_PERIOD) || !keyboardManager.AddKey(VK_SHIFT))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to add keys!\n");
		return false;
	}

	if (!keyboardManager.AddToggle('P', &paused, true))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to add key toggle!\n");
		return false;
	}

	Engine::CollisionTester::CalculateGrid();
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "Game Initialized Successfully!!!\n");
	return true;
}

bool EngineDemo::Shutdown()
{
	// Display some info on shutdown
	Engine::RenderEngine::LogStats();

	if (!m_pWindow->Shutdown()) { return false; }

	for (int i = 0; i < NUM_SHADER_PROGRAMS; ++i)
	{
		if (!m_shaderPrograms[i].Shutdown()) { return false; }
	}

	if (!Engine::TextObject::Shutdown()) { return false; }
	if (!Engine::RenderEngine::Shutdown()) { return false; }
	if (!Engine::ShapeGenerator::Shutdown()) { return false; }
	
	player.Shutdown();

	Engine::GameLogger::Log(Engine::MessageType::cProcess, "Game Shutdown Successfully!!!\n");
	return true;
}

bool EngineDemo::InitializeCallback(void * game, Engine::MyWindow * window)
{
	if (!game) { return false; }
	return reinterpret_cast<EngineDemo *>(game)->Initialize(window);
}

void EngineDemo::UpdateCallback(void * game, float dt)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->Update(dt);
}

void EngineDemo::ResizeCallback(void * game)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnResizeWindow();
}

void EngineDemo::DrawCallback(void * game)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->Draw();
}

void EngineDemo::MouseScrollCallback(void * game, int degrees)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnMouseScroll(degrees);
}

void EngineDemo::MouseMoveCallback(void * game, int dx, int dy)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnMouseMove(dx, dy);
}

void EngineDemo::Update(float dt)
{
	const float maxBorder = 0.1f;
	const float minBorder = -0.1f;

	keyboardManager.Update(dt); // needs to come before to un-pause

	if (!won) { ShowFrameRate(dt); }
	if (!ProcessInput(dt)) { return; }

	playerGraphicalObject.CalcFullTransform();
	player.Update(dt);

	if (paused) { return; }

	time += dt;

	for (int i = 0; i < NUM_DEMO_OBJECTS; ++i)
	{
		m_demoObjects[i].SetRotation(m_demoObjects[i].GetRotation() + dt * m_demoObjects[i].GetRotationRate());
		m_demoObjects[i].SetRotMat(Engine::Mat4::RotationAroundAxis(m_demoObjects[i].GetRotationAxis(), m_demoObjects[i].GetRotation()));

		/*if (i >= 4 && i <= 6)
		{
			Engine::Vec3 xyz = Engine::Vec3(i % 3 == 2 ? 1.0f : 0.0f,
				i % 3 == 0 ? 1.0f : 0.0f,
				i % 3 == 1 ? 1.0f : 0.0f);
			Engine::Vec3 teapotPos = m_demoObjects[5].GetPos();
				m_lights[i].SetRotation(m_lights[i].GetRotation() + m_lights[i].GetRotationRate() * dt);
			Engine::Mat4 rotationAroundAxis = Engine::Mat4::RotationAroundAxis(m_lights[i].GetRotationAxis(), m_lights[i].GetRotation());
			m_lights[i].SetTransMat(Engine::Mat4::Translation(teapotPos + (ROTATE_DISTANCE * (rotationAroundAxis * xyz))));
		}*/

		m_lights[i].CalcFullTransform();
		m_demoObjects[i].CalcFullTransform();
	}

	m_grid.CalcFullTransform();

	static Engine::GraphicalObject *s_pSelected = nullptr;
	bool thisFrame = false;
	Engine::RayCastingOutput rco = Engine::CollisionTester::FindFromMousePos(Engine::MouseManager::GetMouseX(), Engine::MouseManager::GetMouseY(), 1000.0f);
	for (int i = 0; i < NUM_DEMO_OBJECTS; ++i)
	{
		if (rco.m_belongsTo == &m_lights[i] && Engine::MouseManager::IsLeftMouseClicked())
		{
			s_pSelected = &m_lights[i];
			thisFrame = true;
		}
	}

	if (s_pSelected && !thisFrame && Engine::MouseManager::IsLeftMouseClicked())
	{
		s_pSelected = nullptr;
		Engine::CollisionTester::CalculateGrid(); // handle de-selection of light
	}

	bool isPlane = false;
	for (int i = 0; i < NUM_SCENES; ++i) { if (rco.m_belongsTo == &m_scenePlanes[i]) { isPlane = true; } }
	if (rco.m_didIntersect && (isPlane) && s_pSelected)
	{ 
		s_pSelected->SetTransMat(Engine::Mat4::Translation(rco.m_intersectionPoint + Engine::Vec3(0.0f, LIGHT_HEIGHT, 0.0f))); 
	}

	for (int i = 0; i < NUM_SCENES; ++i){ m_scenePlanes[i].CalcFullTransform(); }

	static float error = 0.0f;
	error += 25.0f * dt;
	if (error > 1.0f) { error -= 1.0f; 	fbCam.MouseRotate(1, 0);}

	fbCam.SetPosition(m_lights[0].GetPos());
	m_demoObjects[NUM_DARGONS_TOTAL].SetTransMat(Engine::Mat4::Translation(m_lights[0].GetPos()));
	m_demoObjects[NUM_DARGONS_TOTAL].SetRotMat(fbCam.GetRotMat());
	m_demoObjects[NUM_DARGONS_TOTAL].CalcFullTransform();

	framebufferLookAt = fbCam.GetWorldToViewMatrix();

	shadowMatrix = Engine::Mat4::Bias() * (framebufferPerspective * framebufferLookAt);

	//Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "fractalSeed : (%.3f, %.3f)\n", fractalSeed.GetX(), fractalSeed.GetY()); // Amazingly useful for debugging fractal shader
	shaderOffset += step * dt; if (shaderOffset > maxBorder) { shaderOffset = maxBorder; step *= -1.0f; } if (shaderOffset < minBorder) { shaderOffset = minBorder; step *= -1.0f; }
	fractalSeed.GetAddress()[0] += fsx.GetX() * dt; if (fractalSeed.GetX() < fsx.GetY()) { fractalSeed.GetAddress()[0] = fsx.GetY(); fsx.GetAddress()[0] *= -1.0f; } if (fractalSeed.GetX() > fsx.GetZ()) { fractalSeed.GetAddress()[0] = fsx.GetZ(); fsx.GetAddress()[0] *= -1.0f; }
	fractalSeed.GetAddress()[1] += fsy.GetX() * dt; if (fractalSeed.GetY() < fsy.GetY()) { fractalSeed.GetAddress()[1] = fsy.GetY(); fsy.GetAddress()[0] *= -1.0f; } if (fractalSeed.GetY() > fsy.GetZ()) { fractalSeed.GetAddress()[1] = fsy.GetZ(); fsy.GetAddress()[0] *= -1.0f; }

	vpm = Engine::Mat4::ViewPort((float)m_pWindow->width(), 0.0f, (float)m_pWindow->height(), 0.0f, RENDER_DISTANCE, m_perspective.GetNearDist());
	Engine::Vec3 darginForward = playerGraphicalObject.GetRotMat() * Engine::Vec3(0.0f, 0.0f, -1.0f);
	Engine::Vec3 darginRight = playerGraphicalObject.GetRotMat() * Engine::Vec3(1.0f, 0.0f, 0.0f);
	Engine::Vec3 darginUp = playerGraphicalObject.GetRotMat() * Engine::Vec3(0.0f, 1.0f, 0.0f);

	m_demoObjects[1].SetTransMat(Engine::Mat4::Translation(playerGraphicalObject.GetPos() + 5.0f * darginForward + 7.0f * darginUp));
	Engine::Vec3 up = Engine::Vec3(0.0f, 1.0f, 0.0f);
	Engine::Mat4 rotationMatrix = Engine::Mat4::RotationAroundAxis(up.Cross(darginForward), Engine::MathUtility::GetVectorAngleRadians(darginForward, up));
	m_demoObjects[1].SetRotMat(rotationMatrix);
	m_demoObjects[1].CalcFullTransform();


	static Engine::Vec3 lastPos;
	darginVelocity = (playerGraphicalObject.GetPos() - lastPos) / dt;
	lastPos = playerGraphicalObject.GetPos();

}

void EngineDemo::Draw()
{
	// Clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_demoObjects[3].SetEnabled(false);
	m_demoObjects[2].SetEnabled(false);
	m_demoObjects[1].SetEnabled(false);
	Engine::RenderEngine::Draw();
	m_demoObjects[1].SetEnabled(true);
	m_demoObjects[2].SetEnabled(true);
	m_demoObjects[3].SetEnabled(true);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	Engine::RenderEngine::DrawSingleObjectRegularly(&m_demoObjects[1]);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if (drawDemoQuads)
	{
		if (instanceCycleIndex == 0)
		{
			Engine::RenderEngine::DrawInstanced(&m_demoObjects[2], &instanceBuffer);
		}
		else if (instanceCycleIndex == 1)
		{
			Engine::RenderEngine::DrawInstanced(&m_demoObjects[3], numRows * numColumns);
		}
		else if (instanceCycleIndex == 2)
		{
			Engine::RenderEngine::DrawInstanced(&m_demoObjects[4], numRows * numColumns);
		}
	}

	Engine::RenderEngine::DrawInstanced(&m_demoObjects[5], &asteroids);



	/*if (plane)
	{
		eyeLightVal = playerCamera.GetWorldToViewMatrix() * m_lights[1].GetPos();
		screenToTexWidth = (float)framebufferWidth / m_pWindow->width();
		screenToTexHeight = (float)framebufferHeight / m_pWindow->height();
		persp = Engine::Mat4::InfinitePerspective(m_perspective.GetFOVY(), m_perspective.GetAspectRatio(), m_perspective.GetNearDist());

		PassOneVolumetric();
		PassTwoVolumetric();
		PassThreeVolumetric();

		Engine::RenderEngine::DrawSingleObjectRegularly(&m_lights[1]);
	}
	else
	{
		PassOneRegular();
		PassTwoRegular();
		glClearStencil(0);

		Engine::RenderEngine::DrawSingleObjectRegularly(&m_lights[0]);
		Engine::RenderEngine::DrawSingleObjectRegularly(&m_demoObjects[NUM_DARGONS_TOTAL]);
	}*/


	// draw fps text
	m_fpsTextObject.RenderText(&m_shaderPrograms[0], debugColorLoc);
	m_EngineDemoInfoObject.RenderText(&m_shaderPrograms[0], debugColorLoc);
}

void EngineDemo::OnResizeWindow()
{
	if (m_pWindow == nullptr) { return; }
	float aspect = static_cast<float>(m_pWindow->width()) / m_pWindow->height();
	m_perspective.SetAspectRatio(aspect);
	m_perspective.SetScreenDimmensions(static_cast<float>(m_pWindow->width()), static_cast<float>(m_pWindow->height()));
	Engine::MousePicker::SetPerspectiveInfo(m_perspective.GetFOVY(), m_perspective.GetNearDist(), m_perspective.GetWidth(), m_perspective.GetHeight());
	glViewport(0, 0, m_pWindow->width(), m_pWindow->height());

}

void EngineDemo::OnMouseScroll(int degrees)
{
	mouseComponent.MouseScroll(degrees);
}

void EngineDemo::OnMouseMove(int deltaX, int deltaY)
{
	if (Engine::MouseManager::IsRightMouseDown()) { mouseComponent.MouseMove(deltaX, deltaY); }
	mouseComponent.SetMousePos(Engine::MouseManager::GetMouseX(), Engine::MouseManager::GetMouseY());
}

void EngineDemo::OnConfigReload(void * classInstance)
{
	// error checking
	if (!classInstance) { return; }

	// get pointer to instance
	EngineDemo *pGame = reinterpret_cast<EngineDemo*>(classInstance);

	if (Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.FpsInterval", pGame->m_fpsInterval))
	{
		if (pGame->m_fpsInterval < 0.5f) { pGame->m_fpsInterval = 0.5f; }
	}

	float value;
	Engine::Vec2 inV2;
	int inInt;
	Engine::Vec3 color;
	Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.Spotlight.AttenuationExponent", 4, pGame->spotlightAttenuations.GetAddress());
	if (Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FractalSeed", 2, inV2.GetAddress())) { pGame->fractalSeed = inV2; }
	if (Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FSX", 3, color.GetAddress())) { pGame->fsx = color; }
	if (Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FSY", 3, color.GetAddress())) { pGame->fsy = color; }
	if (Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.ShaderTest.RepeatScale", value)) { pGame->repeatScale = value; }
	if (Engine::ConfigReader::pReader->GetIntForKey("EngineDemo.ShaderTest.NumIterations", inInt)) { pGame->numIterations = inInt; }
}

//void EngineDemo::PTCallback(Engine::GraphicalObject * instance, void * classInstance)
//{
//	EngineDemo *pGame = reinterpret_cast<EngineDemo*>(classInstance);
//
//	PassMatrix(instance, classInstance);
//	PassTint(instance, classInstance);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, instance->GetMeshPointer()->GetTextureID());
//	glUniform1i(pGame->texLoc, 0);
//}

bool EngineDemo::InitializeGL()
{
	glViewport(0, 0, m_pWindow->width(), m_pWindow->height());

	glClearColor(backgroundColor.GetX(), backgroundColor.GetY(), backgroundColor.GetZ(), 1.0f);

	glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// TODO: enable and disable!?!?!
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClearStencil(0);

	if (m_shaderPrograms[0].Initialize())
	{
		m_shaderPrograms[0].AddVertexShader("..\\Data\\Shaders\\Debug.Vert.shader");
		m_shaderPrograms[0].AddFragmentShader("..\\Data\\Shaders\\Debug.Frag.shader");
		m_shaderPrograms[0].LinkProgram();
		m_shaderPrograms[0].UseProgram();
	}

	if (m_shaderPrograms[1].Initialize())
	{
		m_shaderPrograms[1].AddVertexShader("..\\Data\\Shaders\\SimpleTexture.vert.shader");
		m_shaderPrograms[1].AddFragmentShader("..\\Data\\Shaders\\SimpleTexture.frag.shader");
		m_shaderPrograms[1].LinkProgram();
		m_shaderPrograms[1].UseProgram();
	}

	if (m_shaderPrograms[2].Initialize())
	{
		m_shaderPrograms[2].AddVertexShader("..\\Data\\Shaders\\PC.vert.shader");
		m_shaderPrograms[2].AddFragmentShader("..\\Data\\Shaders\\PC.frag.shader");
		m_shaderPrograms[2].LinkProgram();
		m_shaderPrograms[2].UseProgram();
	}

	if (m_shaderPrograms[3].Initialize())
	{					 
		m_shaderPrograms[3].AddVertexShader("..\\Data\\Shaders\\Asteroid.vert.shader");
		m_shaderPrograms[3].AddFragmentShader("..\\Data\\Shaders\\Asteroid.frag.shader");
		m_shaderPrograms[3].LinkProgram();
		m_shaderPrograms[3].UseProgram();
	}

	if (m_shaderPrograms[4].Initialize())
	{
		m_shaderPrograms[4].AddVertexShader("..\\Data\\Shaders\\WavyPlane.vert.shader");
		m_shaderPrograms[4].AddGeometryShader("..\\Data\\Shaders\\WavyPlane.geom.shader");
		m_shaderPrograms[4].AddFragmentShader("..\\Data\\Shaders\\WavyPlane.frag.shader");
		m_shaderPrograms[4].LinkProgram();
		m_shaderPrograms[4].UseProgram();
	}


	if (m_shaderPrograms[5].Initialize())
	{					 
		m_shaderPrograms[5].AddVertexShader("..\\Data\\Shaders\\ParticleFountain.vert.shader");
		m_shaderPrograms[5].AddGeometryShader("..\\Data\\Shaders\\ParticleFountain.geom.shader");
		m_shaderPrograms[5].AddFragmentShader("..\\Data\\Shaders\\ParticleFountain.frag.shader");
		m_shaderPrograms[5].LinkProgram();
		m_shaderPrograms[5].UseProgram();
	}


	if (m_shaderPrograms[6].Initialize())
	{					 
		m_shaderPrograms[6].AddVertexShader("..\\Data\\Shaders\\DemoQuad.vert.shader");
		m_shaderPrograms[6].AddFragmentShader("..\\Data\\Shaders\\DemoQuad.frag.shader");
		m_shaderPrograms[6].LinkProgram();
		m_shaderPrograms[6].UseProgram();
	}

	if (m_shaderPrograms[7].Initialize())
	{					 
		m_shaderPrograms[7].AddVertexShader("..\\Data\\Shaders\\DemoQuad2.vert.shader");
		m_shaderPrograms[7].AddFragmentShader("..\\Data\\Shaders\\DemoQuad2.frag.shader");
		m_shaderPrograms[7].LinkProgram();
		m_shaderPrograms[7].UseProgram();
	}

	if (m_shaderPrograms[8].Initialize())
	{					 
		m_shaderPrograms[8].AddVertexShader("..\\Data\\Shaders\\DemoQuad3.vert.shader");
		m_shaderPrograms[8].AddFragmentShader("..\\Data\\Shaders\\DemoQuad3.frag.shader");
		m_shaderPrograms[8].LinkProgram();
		m_shaderPrograms[8].UseProgram();
	}

	if (m_shaderPrograms[9].Initialize())
	{					 
		m_shaderPrograms[9].AddVertexShader("..\\Data\\Shaders\\PhongPhong.vert.shader");
		m_shaderPrograms[9].AddFragmentShader("..\\Data\\Shaders\\PhongPhong.frag.shader");
		m_shaderPrograms[9].LinkProgram();
		m_shaderPrograms[9].UseProgram();
	}

	debugColorLoc = m_shaderPrograms[0].GetUniformLocation("tint");
	tintColorLoc = m_shaderPrograms[1].GetUniformLocation("tint");
	tintIntensityLoc = m_shaderPrograms[0].GetUniformLocation("tintIntensity");
	texLoc = m_shaderPrograms[1].GetUniformLocation("textureSampler");
	ambientColorLoc = m_shaderPrograms[4].GetUniformLocation("ambientLightColor");
	ambientIntensityLoc = m_shaderPrograms[4].GetUniformLocation("ambientLightIntensity");
	diffuseColorLoc = m_shaderPrograms[4].GetUniformLocation("diffuseLightColor");
	diffuseIntensityLoc = m_shaderPrograms[4].GetUniformLocation("diffuseLightIntensity");
	specularColorLoc = m_shaderPrograms[4].GetUniformLocation("specularLightColor");
	specularIntensityLoc = m_shaderPrograms[4].GetUniformLocation("specularLightIntensity");
	specularPowerLoc = m_shaderPrograms[4].GetUniformLocation("specularPower");
	modelToWorldMatLoc = m_shaderPrograms[4].GetUniformLocation("modelToWorld");
	worldToViewMatLoc = m_shaderPrograms[4].GetUniformLocation("worldToView");
	perspectiveMatLoc = m_shaderPrograms[4].GetUniformLocation("projection");
	lightLoc = m_shaderPrograms[4].GetUniformLocation("lightPos_WorldSpace");
	cameraPosLoc = m_shaderPrograms[4].GetUniformLocation("cameraPosition_WorldSpace");
	//subOneIndex = m_shaderPrograms[3].GetSubroutineIndex(GL_FRAGMENT_SHADER, "RecordDepth");
	//subTwoIndex = m_shaderPrograms[3].GetSubroutineIndex(GL_FRAGMENT_SHADER, "PhongWithShadow");
	//subThreeIndex = m_shaderPrograms[3].GetSubroutineIndex(GL_FRAGMENT_SHADER, "PhongWithShadowAverage");
	//edgeSubIndex = m_shaderPrograms[4].GetSubroutineIndex(GL_GEOMETRY_SHADER, "DoEdges");
	//passSubIndex = m_shaderPrograms[4].GetSubroutineIndex(GL_GEOMETRY_SHADER, "PassThrough");
	//finalSubIndex = m_shaderPrograms[4].GetSubroutineIndex(GL_FRAGMENT_SHADER, "FinalPass");
	//shadeSubIndex = m_shaderPrograms[4].GetSubroutineIndex(GL_FRAGMENT_SHADER, "Shade");
	//doNothingFragSubIndex = m_shaderPrograms[4].GetSubroutineIndex(GL_FRAGMENT_SHADER, "DoNothing");
	//eyeLightPosLoc = m_shaderPrograms[4].GetUniformLocation("eyeLightPos");


	//directionalPositionLoc = m_shaderPrograms[3].GetUniformLocation("objectCenterPosition_WorldSpace");
	//lightsMin = m_shaderPrograms[6].GetUniformLocation("lights[0].positionEye");
	//spotMin = m_shaderPrograms[7].GetUniformLocation("spotLight.positionEye");
	//levelsLoc = m_shaderPrograms[8].GetUniformLocation("numLevels");
	//fogMinLoc = m_shaderPrograms[9].GetUniformLocation("fog.minDist");
	//fogMaxLoc = m_shaderPrograms[9].GetUniformLocation("fog.maxDist");
	//fogColorLoc = m_shaderPrograms[9].GetUniformLocation("fog.color");
	//mossLoc = m_shaderPrograms[3].GetUniformLocation("mossSampler");
	//brickLoc = m_shaderPrograms[3].GetUniformLocation("shadowMap");
	halfWidthLoc = m_shaderPrograms[5].GetUniformLocation("halfWidth");
	//repeatScaleLoc = m_shaderPrograms[3].GetUniformLocation("repeatScale");
	//numIterationsLoc = m_shaderPrograms[3].GetUniformLocation("numIterations");
	//shaderOffsetLoc = m_shaderPrograms[3].GetUniformLocation("randomValue");
	vpmLoc = m_shaderPrograms[4].GetUniformLocation("viewportMatrix");
	lineWidthLoc = m_shaderPrograms[4].GetUniformLocation("lineInfo.width");
	lineColorLoc = m_shaderPrograms[4].GetUniformLocation("lineInfo.color");
	//fColorLoc = m_shaderPrograms[5].GetUniformLocation("vertexNormalColor");
	//vColorLoc = m_shaderPrograms[5].GetUniformLocation("faceNormalColor");
	//hairLengthLoc = m_shaderPrograms[5].GetUniformLocation("hairLength");
	//cycleHairModeLoc = m_shaderPrograms[5].GetUniformLocation("hairState");
	//explodeDistLoc = m_shaderPrograms[6].GetUniformLocation("explodeDist");
	//shadowMatrixLoc = m_shaderPrograms[3].GetUniformLocation("shadowMatrix");
	velLoc = m_shaderPrograms[4].GetUniformLocation("velocity");
	timeLoc = m_shaderPrograms[4].GetUniformLocation("time");
	wavLoc = m_shaderPrograms[4].GetUniformLocation("wavenumber");
	ampLoc = m_shaderPrograms[4].GetUniformLocation("amplitude");
	lifeTimeLoc = m_shaderPrograms[5].GetUniformLocation("uParticleLifetime");
	gravityLoc = m_shaderPrograms[5].GetUniformLocation("uGravity");
	//darginVelLoc = m_shaderPrograms[5].GetUniformLocation("velocityOffset");

	widthLoc = m_shaderPrograms[7].GetUniformLocation("width");
	heightLoc = m_shaderPrograms[7].GetUniformLocation("height");
	rowsLoc = m_shaderPrograms[7].GetUniformLocation("numRows");
	columnsLoc = m_shaderPrograms[7].GetUniformLocation("numColumns");
	rowColLoc = m_shaderPrograms[8].GetUniformLocation("rowColor");
	colColLoc = m_shaderPrograms[8].GetUniformLocation("columnColor");

	if (Engine::MyGL::TestForError(Engine::MessageType::cFatal_Error, "InitializeGL errors!"))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to InitializeGL()! TestForErrors found gl errors!\n");
		return false;
	}
	
	Engine::GameLogger::Log(Engine::MessageType::Process, "EngineDemo::InitializeGL() succeeded!\n");
	return true;
}

bool EngineDemo::ReadConfigValues()
{
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.FpsInterval", m_fpsInterval)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FpsInterval!\n"); return false; }
	if (m_fpsInterval < 0.5f) { m_fpsInterval = 0.5f; }

	float cameraSpeed = 1.0f;
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.CameraSpeed", cameraSpeed)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key CameraSpeed!\n"); return false; }
	if (cameraSpeed < 0.0f) { cameraSpeed = 1.0f; }
	player.GetComponentByType<Engine::ChaseCameraComponent>()->SetSpeed(cameraSpeed);

	float cameraRotationSpeed = 1.0f;
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.CameraRotationSpeed", cameraRotationSpeed)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key CameraRotationSpeed!\n"); return false; }
	if (cameraRotationSpeed < 0.0f) { cameraRotationSpeed = 1.0f; }
	player.GetComponentByType<Engine::ChaseCameraComponent>()->SetRotateSpeed(cameraRotationSpeed);

	if (!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.Spotlight.AttenuationExponent", 4, spotlightAttenuations.GetAddress()))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get floats for key AttenuationExponent!\n"); return false;
	}
	
	if(!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FractalSeed", 2, fractalSeed.GetAddress())) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FractalSeed!\n"); return false; }
	if (!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FSX", 3, fsx.GetAddress())) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FSX!\n"); return false; }
	if (!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.ShaderTest.FSY", 3, fsy.GetAddress())) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FSY!\n"); return false; }
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.ShaderTest.RepeatScale", repeatScale)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key RepeatScale!\n"); return false; }
	if (!Engine::ConfigReader::pReader->GetIntForKey("EngineDemo.ShaderTest.NumIterations", numIterations)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get int for key NumIterations!\n"); return false; }

	Engine::GameLogger::Log(Engine::MessageType::Process, "Successfully read in config values!\n");
	return true;
}

bool EngineDemo::ProcessInput(float dt)
{
	static int spotLightIndex = 0;
	static bool specToggle = false;
	
	float lineDelta = 0.1f;
	static void *pSubIndex = &subTwoIndex;

	//int multiKeyTest[]{ 'J', 'K', VK_OEM_PERIOD };
	//if (keyboardManager.KeysArePressed(&multiKeyTest[0], 3)) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "3 keys pressed!\n"); }
	if (keyboardManager.KeyWasPressed('T')) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "%f\n", dt); }
	if (keyboardManager.KeyWasReleased('C')) { Engine::CollisionTester::ConsoleLogOutput(); }
	if (keyboardManager.KeyWasPressed('`')) { Engine::ConfigReader::pReader->ProcessConfigFile(); }
	if (keyboardManager.KeyWasPressed('M')) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "(%.3f, %.3f, %.3f)\n", playerGraphicalObject.GetPos().GetX(), playerGraphicalObject.GetPos().GetY(), playerGraphicalObject.GetPos().GetZ()); }
	if (keyboardManager.KeyWasPressed('L')) { Engine::RenderEngine::LogStats(); }
	if (keyboardManager.KeyWasPressed('N')) { cycleHairMode++; cycleHairMode %= 4; }
	if (keyboardManager.KeyIsDown(VK_SHIFT)) { lineDelta *= -1.0f; }
	if (keyboardManager.KeyIsDown('K')) { lineDelta *= 0.1f; }
	if (keyboardManager.KeyWasPressed('J')) { lineWidth = Engine::MathUtility::Clamp(lineWidth + lineDelta, 0.0f, 6.0f); }


	if (keyboardManager.KeyWasPressed('6')) { time = 0.0f; }
	if (keyboardManager.KeyWasPressed('7')) { drawDemoQuads = !drawDemoQuads; }
	if (keyboardManager.KeyWasPressed('8')) { instanceCycleIndex = (instanceCycleIndex + 1) % instanceCycles; }

	//if (keyboardManager.KeyWasPressed('0')) { HandleBitKeys(0); }
	//if (keyboardManager.KeyWasPressed('2')) { HandleBitKeys(2); }
	//if (keyboardManager.KeyWasPressed('3')) { HandleBitKeys(3); }
	//if (keyboardManager.KeyWasPressed('4')) { HandleBitKeys(4); }
	//if (keyboardManager.KeyWasPressed('5')) { HandleBitKeys(5); }
	//if (keyboardManager.KeyWasPressed('6')) { HandleBitKeys(6); }
	//if (keyboardManager.KeyWasPressed('7')) { HandleBitKeys(7); }

	/*if (keyboardManager.KeyWasPressed('B'))
	{
		specToggle = !specToggle;
		Engine::Vec3 spec(specToggle ? 1.0f : 0.0f);
		m_scenePlanes[5].GetMatPtr()->m_specularReflectivity = spec;
		m_demoObjects[8].GetMatPtr()->m_specularReflectivity = spec;
		m_demoObjects[9].GetMatPtr()->m_specularReflectivity = spec;
	}

	int levelChange = 1;
	float amount = 1.0f;
	if (keyboardManager.KeyIsDown(VK_SHIFT)) { amount *= 10.0f; levelChange *= 10; }
	if (keyboardManager.KeyWasPressed(VK_PRIOR)) { numCelLevels = Engine::MathUtility::Clamp(numCelLevels + levelChange, 1, 100); }
	if (keyboardManager.KeyWasPressed(VK_NEXT)) { numCelLevels = Engine::MathUtility::Clamp(numCelLevels - levelChange, 1, 100); }
	if (keyboardManager.KeyWasPressed(VK_OEM_4)) { spotlightCutoff -= Engine::MathUtility::ToRadians(amount); }
	if (keyboardManager.KeyWasPressed(VK_OEM_6)) { spotlightCutoff += Engine::MathUtility::ToRadians(amount); }
	if (keyboardManager.KeyWasPressed(VK_OEM_5)) { spotLightIndex++; spotLightIndex %= 4; spotlightAttenuation = spotlightAttenuations.GetAddress()[spotLightIndex]; }
*/
	if (keyboardManager.KeyWasPressed('X')) { Shutdown(); return false; }

	return true;
}

void EngineDemo::ShowFrameRate(float dt)
{
	static unsigned int numFrames = 0;
	static float    timeAccumulator = 0.0f;

	++numFrames;
	timeAccumulator += dt;	

	if (timeAccumulator >= m_fpsInterval)
	{
		//Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, m_fpsInterval);
		char fpsText[50];
		sprintf_s(fpsText, 50, "FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, m_fpsInterval);
		m_fpsTextObject.SetupText(-0.9f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, fpsText);
		timeAccumulator = 0;
		numFrames = 0;
	}
}

void EngineDemo::DoFramebufferThing(int w1, int h1, int w2, int h2, int tId)
{
	const GLuint     unbindFbo = 0; // 0 unbinds fbo
	GLint      srcX0 = 0;
	GLint      srcY0 = 0;
	GLint      srcX1 = w1 - 1;
	GLint      srcY1 = h1 - 1;
	GLint      destX0 = 0;
	GLint      destY0 = 0;
	GLint      destX1 = w2 - 1;
	GLint      destY1 = h2 - 1;
	const GLbitfield mask = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
	const GLenum     filter = GL_NEAREST;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, tId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, unbindFbo);
	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, destX0, destY0, destX1, destY1, mask, filter);
}

void EngineDemo::DontDepthThisPass()
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
}

void EngineDemo::DoColorThisPass()
{
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

}

void EngineDemo::DoStencilThing()
{
	// TODO: MOVE TO INITIALIZE, ONLY DO WHAT IS NEEDED EVERY FRAME (ENABLE/DISABLE?)
	const GLenum stencilFunc = GL_ALWAYS;
	const GLint  stencilRefValue = 0;
	const GLuint stencilMask = 0xFFFF;
	const GLenum stencilFailAction = GL_KEEP;
	const GLenum depthFailAction = GL_KEEP;
	const GLenum frontPassAction = GL_INCR_WRAP;
	const GLenum backPassAction = GL_DECR_WRAP;

	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(stencilFunc, stencilRefValue, stencilMask);
	glStencilOpSeparate(GL_FRONT, stencilFailAction, depthFailAction, frontPassAction);
	glStencilOpSeparate(GL_BACK, stencilFailAction, depthFailAction, backPassAction);
}

void EngineDemo::DoStencilThingTwo()
{
	const GLenum stencilFunc = GL_EQUAL;
	const GLint  stencilRefValue = 0;
	const GLuint stencilMask = 0xFFFF;
	const GLenum failAction = GL_KEEP;
	const GLenum zFailAction = GL_KEEP;
	const GLenum zPassAction = GL_KEEP;

	glStencilFunc(stencilFunc, stencilRefValue, stencilMask);
	glStencilOp(failAction, zFailAction, zPassAction);

}

void EngineDemo::PassOneRegular()
{
	// bind the framebuffer
	pCurrentBuffer->Bind();

	// record depth only for these draw calls
	int prev = subTwoIndex;
	subTwoIndex = subOneIndex;

	glCullFace(GL_FRONT);

	Engine::RenderEngine::DrawSingleObjectDifferently(&m_scenePlanes[0], framebufferPerspective.GetAddress(), framebufferLookAt.GetAddress(), nullptr, worldToViewMatLoc, perspectiveMatLoc, texLoc);
	Engine::RenderEngine::DrawSingleObjectDifferently(&playerGraphicalObject, framebufferPerspective.GetAddress(), framebufferLookAt.GetAddress(), nullptr, worldToViewMatLoc, perspectiveMatLoc, texLoc);

	for (int i = 0; i < NUM_DARGONS_DEMO1; ++i)
	{
		Engine::RenderEngine::DrawSingleObjectDifferently(&m_demoObjects[i], framebufferPerspective.GetAddress(), framebufferLookAt.GetAddress(), nullptr, worldToViewMatLoc, perspectiveMatLoc, texLoc);
	}

	glCullFace(GL_BACK);

	// go back to normal subroutine
	subTwoIndex = prev;

	// restore stuff
	pCurrentBuffer->UnBind(0, 0, m_pWindow->width(), m_pWindow->height());

}

void EngineDemo::PassTwoRegular()
{
	// draw regularish
	Engine::RenderEngine::DrawSingleObjectRegularly(&m_scenePlanes[0]);
	Engine::RenderEngine::DrawSingleObjectRegularly(&playerGraphicalObject);

	for (int i = 0; i < NUM_DARGONS_DEMO1; ++i)
	{
		Engine::RenderEngine::DrawSingleObjectRegularly(&m_demoObjects[i]);
	}
}

void EngineDemo::PassOneVolumetric()
{	
	// pass one for volumetric
	volumetricBuffer.Bind();

	// use shade and passthrough
	geomSubIndex = passSubIndex;
	fragSubIndex = shadeSubIndex;
	
	// TODO INF PERSP!?!?

	Engine::RenderEngine::DrawSingleObjectRegularly(&m_scenePlanes[1]);

	for (int i = NUM_DARGONS_DEMO1; i < NUM_DARGONS_TOTAL; ++i)
	{
		Engine::RenderEngine::DrawSingleObjectRegularly(&m_demoObjects[i]);
	}

}

void EngineDemo::PassTwoVolumetric()
{
	// pass two for volumetric

	DoFramebufferThing(volumetricBuffer.GetWidth(), volumetricBuffer.GetHeight(), m_pWindow->width(), m_pWindow->height(), *volumetricBuffer.GetTexIdPtr());
	DontDepthThisPass();
	volumetricBuffer.UnBind(0, 0, m_pWindow->width(), m_pWindow->height());
	
	glEnable(GL_STENCIL_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);
	glClearStencil(0);

	DoStencilThing();
	
	geomSubIndex = edgeSubIndex;
	fragSubIndex = doNothingFragSubIndex;
	
	//Engine::RenderEngine::DrawSingleObjectRegularly(&m_scenePlanes[1]);
	
	for (int i = NUM_DARGONS_DEMO1; i < NUM_DARGONS_TOTAL; ++i)
	{
		Engine::RenderEngine::DrawSingleObjectRegularly(&m_demoObjects[i]);
	}
	
	DoColorThisPass();
	
}

void EngineDemo::PassThreeVolumetric()
{
	glDisable(GL_DEPTH_TEST);
	DoBlend();

	// Pass three
	DoStencilThingTwo();

	// pass throgh the geometry, and draw to a plane
	geomSubIndex = passSubIndex;
	fragSubIndex = finalSubIndex;

	// draw the near plane plane
	Engine::RenderEngine::DrawSingleObjectRegularly(&m_demoObjects[NUM_DARGONS_TOTAL + 1]);

	DontBlend();
	glEnable(GL_DEPTH_TEST);

	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
}

void EngineDemo::DoBlend()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
}

void EngineDemo::DontBlend()
{
	glDisable(GL_BLEND);
}

bool EngineDemo::UglyDemoCode()
{
	player.SetName("Player");
	player.AddComponent(&playerSpatial, "PlayerSpatial");
	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\BetterDargon.PN.scene", &playerGraphicalObject, m_shaderPrograms[9].GetProgramId());

	playerGraphicalObject.AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&playerGraphicalObject.GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[0].GetLocPtr());
	//playerGraphicalObject.AddUniformData(Engine::UniformData(GL_FRAGMENT_SHADER, &subTwoIndex, 1));
	//playerGraphicalObject.AddUniformData(Engine::UniformData(GL_TEXTURE0, &bufferTexID, brickLoc));
	//playerGraphicalObject.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, &shadowMatrix, shadowMatrixLoc, true));

	playerGraphicalObject.GetMatPtr()->m_specularIntensity = 32.0f;
	playerGraphicalObject.GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f);
	playerGraphicalObject.GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.7f);
	playerGraphicalObject.GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
	playerGraphicalObject.SetScaleMat(Engine::Mat4::Scale(1.0f));
	playerGraphicalObject.SetTransMat(Engine::Mat4::Translation(Engine::Vec3(375.0f, 5.0f, 5.0f)));
	Engine::RenderEngine::AddGraphicalObject(&playerGraphicalObject);
	playerGob.SetGraphicalObject(&playerGraphicalObject);
	player.AddComponent(&playerGob, "PlayerGob");
	player.AddComponent(&playerCamera, "PlayerCamera");
	player.AddComponent(&playerInput, "PlayerInput");
	player.AddComponent(&mouseComponent, "MouseComponent");
	player.Initialize();

	player.GetComponentByType<Engine::SpatialComponent>()->SetPosition(Engine::Vec3(375.0f, 5.0f, 5.0f));
	if (!Engine::TextObject::Initialize(matLoc, tintColorLoc))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize EngineDemo, failed to initialize text renderer!\n");
		return false;
	}

	m_fpsTextObject.SetupText(-0.9f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "FPS not calculated yet!\n");
	m_EngineDemoInfoObject.SetupText(0.3f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "New Game Started!\n");
	m_perspective.SetPerspective(m_pWindow->width() / static_cast<float>(m_pWindow->height()), Engine::MathUtility::ToRadians(60.0f), 1.0f, RENDER_DISTANCE);
	m_perspective.SetScreenDimmensions(static_cast<float>(m_pWindow->width()), static_cast<float>(m_pWindow->height()));
	Engine::MousePicker::SetPerspectiveInfo(m_perspective.GetFOVY(), m_perspective.GetNearDist(), m_perspective.GetWidth(), m_perspective.GetHeight());

	if (!nearestBuffer.InitializeForDepth(framebufferWidth, framebufferHeight, true)) { return false; }
	if (!linearBuffer.InitializeForDepth(framebufferWidth, framebufferHeight, false)) { return false; }
	Engine::Perspective p;
	p.SetPerspective(framebufferAspect, Engine::MathUtility::ToRadians(60.0f), 1.0f, RENDER_DISTANCE);
	framebufferPerspective = p.GetPerspective();
	fbCam.SetPosition(Engine::Vec3(0.0f, LIGHT_HEIGHT, 0.0f));
	fbCam.MouseRotate(0, 0);

	if (!volumetricBuffer.InitializeForShadows(framebufferWidth, framebufferHeight)) { return false; }

	pCurrentBuffer = &nearestBuffer;
	bufferTexID = *pCurrentBuffer->GetTexIdPtr();

	/*Engine::ShapeGenerator::MakeSphere(&framebufferCamera, Engine::Vec3(0.0f, 1.0f, 0.0f));
	framebufferCamera.SetTransMat(Engine::Mat4::Translation(fbCam.GetPosition()));
	framebufferCamera.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, framebufferCamera.GetFullTransformPtr(), modelToWorldMatLoc));
	framebufferCamera.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	framebufferCamera.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	Engine::RenderEngine::AddGraphicalObject(&framebufferCamera);*/

	framebufferLookAt = fbCam.GetWorldToViewMatrix();
	whiteSquareId = Engine::BitmapLoader::SetupWhitePixel();

	Engine::ShapeGenerator::MakeGrid(&m_grid, Engine::CollisionTester::GetGridWidthSections(), Engine::CollisionTester::GetGridHeightSections(), Engine::Vec3(0.5f));
	Engine::RenderEngine::AddGraphicalObject(&m_grid);
	m_grid.SetScaleMat(Engine::Mat4::Scale(Engine::CollisionTester::GetGridScale()));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_grid.GetFullTransformPtr(), modelToWorldMatLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_grid.GetMatPtr()->m_materialColor, tintColorLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT, &m_grid.GetMatPtr()->m_specularIntensity, tintIntensityLoc));
	
	Engine::ShapeGenerator::MakeTessalatedPlane(&m_demoObjects[0], Engine::Vec3(-50.0f, 0.0f, -50.0f), Engine::Vec3(50.0f, 0.0f, 50.0f), Engine::Vec3(250, 0, 250), m_shaderPrograms[4].GetProgramId());
	Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[0]);
	m_demoObjects[0].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_demoObjects[0].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[0].GetLocPtr());
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_demoObjects[0].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_demoObjects[0].GetMatPtr()->m_materialColor, tintColorLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, &vpm, vpmLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT, &amplitude, ampLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT, &velocity, velLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT, &wavenumber, wavLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT, &time, timeLoc));
	m_demoObjects[0].GetMatPtr()->m_specularIntensity = 8.0f;
	m_demoObjects[0].GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f);
	m_demoObjects[0].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.6f);
	m_demoObjects[0].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.3f);
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT, &lineWidth, lineWidthLoc));
	m_demoObjects[0].AddUniformData(Engine::UniformData(GL_FLOAT_VEC4, &lineColor, lineColorLoc));
	wavenumber = 2 * Engine::MathUtility::PI / 10.0f;
	velocity = 2.0f;
	amplitude = 2.0f;

	Engine::ShapeGenerator::CreatePoints(&m_demoObjects[1], 25000, 0.0f, Engine::MathUtility::ToRadians(30.0f), 0.0f, Engine::MathUtility::ToRadians(360.0f), 12.0f, 15.0f, 0.00035f, m_shaderPrograms[5].GetProgramId());
	Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[1]);
	m_demoObjects[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_demoObjects[1].GetFullTransformPtr(), modelToWorldMatLoc));
	m_demoObjects[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_demoObjects[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_demoObjects[1].AddUniformData(Engine::UniformData(GL_FLOAT, &halfWidth, halfWidthLoc));
	m_demoObjects[1].AddUniformData(Engine::UniformData(GL_TEXTURE0, &texID, texLoc));
	m_demoObjects[1].AddUniformData(Engine::UniformData(GL_FLOAT, &lifeTime, lifeTimeLoc));
	m_demoObjects[1].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &gravity, gravityLoc));
	//m_demoObjects[1].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &darginVelocity, darginVelLoc));
	m_demoObjects[1].AddUniformData(Engine::UniformData(GL_FLOAT, &time, timeLoc));
	gravity = Engine::Vec3(0.0f, -1.5f, 0.0f);
	halfWidth = 1.0f;
	lifeTime = 5.0f;
	time = 0.0f;
	texID = Engine::BitmapLoader::LoadTexture("..\\Data\\Textures\\fire.bmp");

	Engine::ShapeGenerator::MakeDemoQuad(&m_demoObjects[2], m_shaderPrograms[6].GetProgramId());
	Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[2]);

	Engine::ShapeGenerator::MakeDemoQuad(&m_demoObjects[3], m_shaderPrograms[7].GetProgramId());
	Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[3]);
	m_demoObjects[3].AddUniformData(Engine::UniformData(GL_FLOAT, &width, widthLoc));
	m_demoObjects[3].AddUniformData(Engine::UniformData(GL_FLOAT, &height, heightLoc));
	m_demoObjects[3].AddUniformData(Engine::UniformData(GL_INT, &numRows, rowsLoc));
	m_demoObjects[3].AddUniformData(Engine::UniformData(GL_INT, &numColumns, columnsLoc));

	Engine::ShapeGenerator::MakeDemoQuad(&m_demoObjects[4], m_shaderPrograms[8].GetProgramId());
	Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[4]);
	m_demoObjects[4].AddUniformData(Engine::UniformData(GL_FLOAT, &width, widthLoc));
	m_demoObjects[4].AddUniformData(Engine::UniformData(GL_FLOAT, &height, heightLoc));
	m_demoObjects[4].AddUniformData(Engine::UniformData(GL_INT, &numRows, rowsLoc));
	m_demoObjects[4].AddUniformData(Engine::UniformData(GL_INT, &numColumns, columnsLoc));
	m_demoObjects[4].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &rowColor, rowColLoc));
	m_demoObjects[4].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &columnColor, colColLoc));

	int objectsX = 10;
	int objectsY = 10;
	int howManyObjects = objectsX * objectsY;
	float *pD = new float[howManyObjects * 3];
	
	for (int i = 0; i < howManyObjects * 3; i += 3)
	{
		float x = (float)((i/6) % objectsX);
		float y = (float)((i/6) / objectsY);
		pD[i + 0] = 1.0f - (2.0f * x / objectsX);
		pD[i + 1] = 1.0f - (4.0f * y / objectsY); 
		pD[i + 2] = 0.1f;
	}

	//for (int i = 0; i < howManyObjects * 3; i += 3)
	//{
	//	Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "%.3f, %.3f, %.3f\n", pD[i], pD[i+1], pD[i+2]);
	//}

	instanceBuffer.Initialize(pD, 3*sizeof(float), howManyObjects, 3);

	delete[] pD;

	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\Dargon.PT.scene", &m_demoObjects[5], m_shaderPrograms[3].GetProgramId(), "..\\Data\\Textures\\DargonSkin.bmp");
	Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[5]);
	m_demoObjects[5].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_demoObjects[5].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	m_demoObjects[5].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_demoObjects[5].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_demoObjects[5].AddUniformData(Engine::UniformData(GL_TEXTURE0, m_demoObjects[5].GetMeshPointer()->GetTextureIDPtr(), texLoc));
	m_demoObjects[5].SetRotationAxis(Engine::Vec3(0.4f, 0.6f, 0.8f));
	m_demoObjects[5].SetRotationRate(Engine::MathUtility::ToRadians(18.0f));

	GLuint amount = 100;
	Engine::Mat4* modelMatrices;
	modelMatrices = new Engine::Mat4[amount]{ Engine::Mat4() };
	GLfloat radius = 500.0f;
	GLfloat offset = 20.5f;
	for (GLuint i = 0; i < amount; i++)
	{
		Engine::Mat4 model;

		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat y = displacement * 0.4f; // y value has smaller displacement
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = model * Engine::Mat4::Translation(Engine::Vec3(x, y, z));

		GLfloat scale = (rand() % 20) / 100.0f + 0.05f;
		model = model * Engine::Mat4::Scale(scale * 50.0f);

		GLfloat rotAngle = (float)(rand() % 360);
		model = model * Engine::Mat4::RotationAroundAxis(Engine::Vec3(0.4f, 0.6f, 0.8f), rotAngle);
		
		modelMatrices[i] = model;
	}

	asteroids.Initialize(modelMatrices, 16*sizeof(float), amount, 16 * amount);

	delete[] modelMatrices;

	int i = 0;
	Engine::ShapeGenerator::MakeLightingCube(&m_lights[i]);
	m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_lights[i].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[i].GetMatPtr()->m_materialColor, tintColorLoc));
	m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[i].GetMatPtr()->m_materialColor, debugColorLoc));
	m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT, &m_lights[i].GetMatPtr()->m_specularIntensity, tintIntensityLoc));
	m_lights[i].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(0.0f, LIGHT_HEIGHT, 0.0f)));
	m_lights[i].GetMatPtr()->m_materialColor = Engine::Vec3(1.0f, 1.0f, 1.0f);
	m_lights[i].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(1.0f, 1.0f, 1.0f);
	Engine::RenderEngine::AddGraphicalObject(&m_lights[i]);
	//Engine::CollisionTester::AddGraphicalObject(&m_lights[i]);


	return true;
}

void EngineDemo::SwapFrameBuffers()
{
	pCurrentBuffer = ((pCurrentBuffer == &nearestBuffer) ? &linearBuffer : &nearestBuffer);
	bufferTexID = *pCurrentBuffer->GetTexIdPtr();
}

void EngineDemo::SwapSubroutineIndex(void **pIndexPtr, int start, int end)
{
	if (*pIndexPtr == &subTwoIndex) { *pIndexPtr = &subThreeIndex; }
	else { *pIndexPtr = &subTwoIndex; }

	for (int i = start; i < end; ++i)
	{
		*m_demoObjects[i].GetUniformDataPtrPtrByLoc(1) = *pIndexPtr;
	}

	if (start == 0) { *m_scenePlanes[0].GetUniformDataPtrPtrByLoc(1) = *pIndexPtr; }
}

