#pragma once

#include <Cool/App/IApp.h>
#include <Cool/App/OpenGLWindow.h>
#include <Cool/Renderer_Fullscreen/Renderer_Fullscreen.h>
#include <Cool/Exporter.h>
#include <Cool/OpenGL/Shader.h>
#include <Cool/Camera/Camera.h>
#include <Cool/FileWatcher/FileWatcher.h>

using namespace Cool;

class App : public IApp {
public:
	App(OpenGLWindow& mainWindow);
	~App() = default;

	void update() override;
	void ImGuiWindows() override;
	void ImGuiMenus() override;
	void onKeyboardEvent(int key, int scancode, int action, int mods) override;
	void onMouseButtonEvent(int button, int action, int mods) override;
	void onScrollEvent(double xOffset, double yOffset) override;
	void onMouseMoveEvent(double xPos, double yPos) override;

private:
	void render();

private:
	OpenGLWindow& m_mainWindow;
	Shader m_shader;
	FileWatcher m_shaderWatcher;
	Camera m_camera;
	Renderer_Fullscreen m_renderer;
	Exporter m_exporter;
	glm::vec3 m_bgColor = glm::vec3(0.478f, 0.674f, 0.792f);
#ifndef NDEBUG
	bool m_bShow_Debug = true;
	bool m_bShow_ImGuiDemo = false;
#endif
};