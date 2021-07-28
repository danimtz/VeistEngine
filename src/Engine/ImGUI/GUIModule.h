#pragma once


#include "Engine/Renderer/RenderModule.h"
#include "Engine/ImGUI/ImGUIContext.h"

class GUIModule
{
public:

	static void init();
	static void shutdown(){};

private:

	static std::unique_ptr<ImGUIContext> s_GUIcontext;

};

