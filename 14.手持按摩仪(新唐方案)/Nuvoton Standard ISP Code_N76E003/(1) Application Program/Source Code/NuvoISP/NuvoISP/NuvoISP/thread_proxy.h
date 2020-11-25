#ifndef __THREAD_PROXY_H__
#define __THREAD_PROXY_H__


#include <process.h>


template <class T>
class thread_proxy_imp
{
protected:
    typedef struct
    {
        DWORD (T::*lpStartAddress)(LPVOID);
        T *obj;
        LPVOID lpParameter;
		HANDLE sem;
    } thread_param;

    static unsigned int thread_proxy_entry (LPVOID lpParameter)
    {
        thread_param *param_ptr = static_cast<thread_param *> (lpParameter);
        thread_param param = *param_ptr;
        ReleaseSemaphore (param_ptr->sem, 1, NULL);

		srand((unsigned int)time(0));

        /* 注意 ->* 操作符 */
        return (param.obj->*param.lpStartAddress) (param.lpParameter);
    }

public:
    CWinThread *create_thread (
		T *obj,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,  // pointer to security attributes
		DWORD dwStackSize,                         // initial thread stack size
		DWORD (T::*lpStartAddress)(LPVOID), // pointer to thread function
		LPVOID lpParameter,                        // argument for new thread
		DWORD dwCreationFlags,                     // creation flags
		LPDWORD lpThreadId                         // pointer to receive thread ID
		)
    {
        CWinThread *rt;
        thread_param param = {lpStartAddress, obj, lpParameter};
        /* 为防止param在新线程使用它之前被析构，加上一个semaphore. */
		param.sem = ::CreateSemaphore (NULL, 0, 1, NULL);

		rt = ::AfxBeginThread(
			&thread_proxy_imp::thread_proxy_entry,
			static_cast<LPVOID> (&param),
			0,
			0,
			CREATE_SUSPENDED);
		rt->m_bAutoDelete = FALSE;
		rt->ResumeThread();

		//	lpThreadAttributes,
		//	dwStackSize,
		//	&thread_proxy_imp::thread_proxy_entry,
		//	static_cast<LPVOID> (&param),
		//	dwCreationFlags,
		//	&uThreadID );
		//	if( lpThreadId ) 
		//		*lpThreadId=uThreadID;
			/*
			{
				char szTmp[128];
				wsprintf( szTmp, ">>>>>>>>>>4<< Thread ID= %X\n", dwThreadID );
				OutputDebugString( szTmp );
			}
			*/
		::WaitForSingleObject (param.sem, INFINITE);
		::CloseHandle (param.sem);
        return rt;
    }

};


template <class T>
CWinThread *thread_proxy(
		T *obj,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,  // pointer to security attributes
		DWORD dwStackSize,                         // initial thread stack size
		DWORD (T::*lpStartAddress)(LPVOID),        // pointer to thread function
		LPVOID lpParameter,                        // argument for new thread
		DWORD dwCreationFlags,                     // creation flags
		LPDWORD lpThreadId                         // pointer to receive thread ID
		)
{
	return thread_proxy_imp<T> ().create_thread (obj,
		lpThreadAttributes,
		dwStackSize,
		lpStartAddress,
		lpParameter,
		dwCreationFlags,
		lpThreadId);
}


#endif
