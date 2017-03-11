#ifndef EngineDemo_H
#define EngineDemo_H

// Justin Furtado
// 6/21/2016
// EngineDemo.h
// The game

#include "ShaderProgram.h"
#include "TextObject.h"
#include "Keyboard.h"
#include "Perspective.h"
#include "Camera.h"
#include "ChaseCamera.h"
#include "Vec2.h"
#include "Entity.h"
#include "MyWindow.h"
#include "GraphicalObject.h"
#include "FrameBuffer.h"

class QMouseEvent;

class EngineDemo
{
public:
	// methods
	bool Initialize(Engine::MyWindow *window);
	bool Shutdown();

	// callbacks
	static bool InitializeCallback(void *game, Engine::MyWindow *window);
	static void UpdateCallback(void *game, float dt);
	static void ResizeCallback(void *game);
	static void DrawCallback(void *game);
	static void MouseScrollCallback(void *game, int degrees);
	static void MouseMoveCallback(void *game, int dx, int dy);
	
	void Update(float dt);
	void Draw();
	
	void OnResizeWindow();
	void OnMouseScroll(int degrees);
	void OnMouseMove(int deltaX, int deltaY);

	bool GameIsPaused() const { return paused; }

	static void OnConfigReload(void *classInstance);
	static const float RENDER_DISTANCE;

private:
	// methods
	bool ReadConfigValues();
	bool InitializeGL();
	bool ProcessInput(float dt);
	void ShowFrameRate(float dt);
	bool UglyDemoCode();
	void SwapFrameBuffers();
	void SwapSubroutineIndex(void ** pIndexPtr, int start, int end);
	void DoBlend();
	void DontBlend();
	void DoFramebufferThing(int w1, int h1, int w2, int h2, int texID);
	void DontDepthThisPass();
	void DoColorThisPass();
	void DoStencilThing();
	void DoStencilThingTwo();
	void PassOneRegular();
	void PassTwoRegular();
	void PassOneVolumetric();
	void PassTwoVolumetric();
	void PassThreeVolumetric();

	//data
	static const int NUM_SHADER_PROGRAMS = 10;
	bool paused = false;
	Engine::Perspective m_perspective;
	Engine::TextObject m_fpsTextObject;
	Engine::TextObject m_EngineDemoInfoObject;
	Engine::MyWindow *m_pWindow{ nullptr };
	Engine::ShaderProgram m_shaderPrograms[NUM_SHADER_PROGRAMS];
	GLint matLoc;
	GLint debugColorLoc;
	GLint tintColorLoc;
	GLint tintIntensityLoc;
	GLint texLoc;
	GLint modelToWorldMatLoc;
	GLint worldToViewMatLoc;
	GLint perspectiveMatLoc;
	GLint lightLoc;
	GLint cameraPosLoc;
	GLint ambientColorLoc;
	GLint ambientIntensityLoc;
	GLint diffuseColorLoc;
	GLint diffuseIntensityLoc;
	GLint specularColorLoc;
	GLint specularIntensityLoc;
	GLint specularPowerLoc;
	GLint lightsMin;
	GLint spotMin;
	GLint phongShaderMethodIndex;
	GLint diffuseShaderMethodIndex;
	GLint directionalPositionLoc;
	GLint levelsLoc;
	GLint subOneIndex;
	GLint subTwoIndex;
	GLint subThreeIndex;
	GLint shadeSubIndex;
	GLint doNothingFragSubIndex;
	GLint passSubIndex;
	GLint edgeSubIndex;
	GLint finalSubIndex;
	GLint fogMinLoc;
	GLint fogMaxLoc;
	GLint fogColorLoc;
	GLint mossLoc;
	GLint brickLoc;
	GLint halfWidthLoc;
	GLint repeatScaleLoc;
	float repeatScale;
	GLint numIterationsLoc;
	int numIterations;
	GLint shaderOffsetLoc;
	float step = 0.1f;
	float shaderOffset = 0.0f;
	Engine::Vec2 fractalSeed;
	Engine::Vec3 fsx;
	Engine::Vec3 fsy;
	Engine::Vec4 spotlightAttenuations;
	float m_fpsInterval = 1.0f;
	bool won = false;
	float specularPower;
	Engine::Keyboard keyboardManager;
};

#endif // ifndef EngineDemo_H