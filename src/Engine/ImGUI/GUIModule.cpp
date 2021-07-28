#include "GUIModule.h"

std::unique_ptr<ImGUIContext> GUIModule::s_GUIcontext = nullptr;



void GUIModule::init() 
{
	s_GUIcontext = ImGUIContext::Create();

}