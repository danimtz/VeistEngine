#include "pch.h"

#include "ThreadPool.h"


namespace Veist
{


	ThreadPool::ThreadPool()
	{

		//m_threads.push_back();


	}

	ThreadPool::~ThreadPool()
	{

		for (int i = 0; i < m_threads.size(); i++)
		{
			m_threads[i].join();
		}


	}

}
