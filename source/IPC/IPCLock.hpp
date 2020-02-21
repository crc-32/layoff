#pragma once
#include <switch.h>

namespace layoff::IPC {

	template<typename T>
	struct ObjLock
	{
		ObjLock(T& o, Mutex& m) : obj(o), mutex(m)
		{
			mutexLock(&mutex);
		}

		~ObjLock()
		{
			mutexUnlock(&mutex);
		}
		
		T& obj;

		ObjLock(ObjLock& other) = delete;
		ObjLock& operator=(ObjLock other) = delete;
	private:
		Mutex& mutex;
	};

}