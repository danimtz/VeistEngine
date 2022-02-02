#include <pch.h>
#include <Veist.h>
#include <Veist/Core/EntryPoint.h> //TODO move engine into Veist folder


namespace Veist
{
	class EditorApp : public Application
	{
	public:
		
		EditorApp() : Application("Editor") {}
	
		~EditorApp() {}
	
	
	};

	Application* CreateApplication()
	{
		return new EditorApp();
	}

	
}