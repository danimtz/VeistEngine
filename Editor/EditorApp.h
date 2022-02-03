#pragma once
#include <Veist.h>


namespace Veist
{

	class EditorApp : public Application
	{
	public:

		EditorApp() : Application("Editor") {}

		~EditorApp() {}

		void initClient() override;
		void runClient() override;
		void shutdownClient() override;





	private:

		Scene* scene;//TEMPORARY replace with actual layer that runs the scene i.e editor or runtime

	};


}