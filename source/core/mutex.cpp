#include "mutex.hpp"
#include "thread.hpp"

#include <iostream>
using namespace std;

Mutex::Mutex(void) : m_bCreated(true)
{
#ifdef CAESARIA_PLATFORM_WIN
   m_mutex = CreateMutex(NULL,FALSE,NULL);
   if( !m_mutex ) m_bCreated = FALSE;
#else
   pthread_mutexattr_t mattr;

   pthread_mutexattr_init( &mattr );
   pthread_mutex_init(&m_mutex,&mattr);

#endif
   memset(&m_owner,0,sizeof(ThreadID));

}

Mutex::~Mutex(void)
{
#ifdef CAESARIA_PLATFORM_WIN
	WaitForSingleObject(m_mutex,INFINITE);
	CloseHandle(m_mutex);
#else
	pthread_mutex_lock(&m_mutex);
	pthread_mutex_unlock(&m_mutex); 
	pthread_mutex_destroy(&m_mutex);
#endif
}

/**
 *
 * Lock
 * the same thread can not lock the same mutex
 * more than once
 *
 **/
void
Mutex::lock()
{
	ThreadID id = Thread::getID();
	try
	{
		if(Thread::ThreadIdsEqual(&m_owner,&id) )
		    throw "\n\tthe same thread can not acquire a mutex twice!\n"; // the mutex is already locked by this thread
#ifdef WINDOWS
		WaitForSingleObject(m_mutex,INFINITE);
#else
		pthread_mutex_lock(&m_mutex);
#endif
		m_owner = Thread::getID();
	}
	catch( char *psz )
	{
#ifdef WINDOWS
		MessageBoxA(NULL,&psz[2],"Fatal exception CMutexClass::Lock",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CMutexClass::Lock : " << psz;
#endif


	}

}

/**
 *
 * Unlock
 * releases a mutex.  only the thread that acquires
 * the mutex can release it.
 *
 **/
void Mutex::unlock()
{
	ThreadID id = Thread::getID();
	try 
	{
		if( ! Thread::ThreadIdsEqual(&id,&m_owner) )
		throw "\n\tonly the thread that acquires a mutex can release it!"; 

		 memset(&m_owner,0,sizeof(ThreadID));
#ifdef CAESARIA_PLATFORM_WIN
	   ReleaseMutex(m_mutex);
#else
	   pthread_mutex_unlock(&m_mutex);
#endif
	}
	catch ( char *psz)
	{
#ifdef CAESARIA_PLATFORM_WIN
		MessageBoxA(NULL,&psz[2],"Fatal exception CMutexClass::Unlock",MB_ICONHAND);
		exit(-1);
#else
		cerr << "Fatal exception CMutexClass::Unlock : " << psz;
#endif

	}
}

