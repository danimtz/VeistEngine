#pragma once


namespace Veist
{


	class ThreadPool
	{
	public:

		ThreadPool();
		~ThreadPool();

		//TODO completely rewrite this to be thread safe and actually work properly etc
		
		void spawnThread(std::function<void()> function) 
		{
			m_threads.push_back(std::thread(function));
		}

	private:

		std::vector<std::thread> m_threads;
	
	
	
	};



}


