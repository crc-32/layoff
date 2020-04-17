#pragma once
#include <switch.h>

namespace layoff::IPC {

	struct ScopeLock
	{
		ScopeLock(Mutex& m) : mutex(m)
		{
			mutexLock(&mutex);
		}

		virtual ~ScopeLock()
		{
			mutexUnlock(&mutex);
		}

		ScopeLock(ScopeLock& other) = delete;
		ScopeLock& operator=(ScopeLock other) = delete;
	private:
		Mutex& mutex;
	};

	template<typename T>
	struct ObjLock : private ScopeLock
	{
		ObjLock(T& o, Mutex& m) : ScopeLock(m), obj(o)
		{

		}

		~ObjLock()
		{

		}
		
		T& obj;

		ObjLock(ObjLock& other) = delete;
		ObjLock& operator=(ObjLock other) = delete;
	};

}