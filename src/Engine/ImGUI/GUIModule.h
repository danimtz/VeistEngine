#pragma once


#include "Engine/Renderer/RenderModule.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

class GUIModule
{
public:

	static void init(RenderBackend* render_backend);
	static void shutdown();

	static void beginFrame();
	static void endFrame();

private:
	static void setDarkThemeColors();

	

};

