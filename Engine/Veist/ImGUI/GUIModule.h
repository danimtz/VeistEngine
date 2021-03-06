#pragma once


#include "Veist/Graphics/RenderModule.h"

#include "imgui.h"
#include "imgui_internal.h""

//TODO: replace vulkan backend of imgui with my own custom one that uses my engines renderbackend. far future for now
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
	static void setImGuiStyle();

	

};

}