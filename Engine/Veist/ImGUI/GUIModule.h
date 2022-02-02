#pragma once


#include "Veist/Renderer/RenderModule.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"


namespace Veist
{

class GUIModule
{
public:

	static void init(RenderBackend* render_backend);
	static void shutdown();

	static void onUpdate();//rework

	static void beginFrame();
	static void endFrame();

private:
	static void setDarkThemeColors();

	

};

}