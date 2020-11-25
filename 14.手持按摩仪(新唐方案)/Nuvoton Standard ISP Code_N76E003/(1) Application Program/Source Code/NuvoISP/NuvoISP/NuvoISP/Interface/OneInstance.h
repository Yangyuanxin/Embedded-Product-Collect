#ifndef INC__ONE_INSTANCE_H__
#define INC__ONE_INSTANCE_H__
#include <windows.h>
#include <stdexcept>

class COneInstance
{
public:
	COneInstance(TCHAR *ptGlobalName)
	{
		/* Name of global mutex */
		_sntprintf_s(m_tMutexName, _countof(m_tMutexName), _TRUNCATE,
			_T("%s-5004780a-b256-43f3-88f5-1c1a798e40a2"), ptGlobalName);

		m_hMutexOneInstance = NULL;
		m_bSuccess = FALSE;
		m_bShared = FALSE;
	}

	virtual ~COneInstance()
	{
		Release();
	}

	virtual void Obtain()
	{
		if(m_hMutexOneInstance != NULL || m_bSuccess != FALSE)
			return;

		/* Create and obtain the global mutex,
		   which is not shared to other process */
		m_hMutexOneInstance = ::CreateMutex(NULL, FALSE, m_tMutexName);

		if(m_hMutexOneInstance == NULL)
		{
			m_bSuccess = FALSE;
			m_bShared = FALSE;
		}
		else
		{
			DWORD dwError = ::GetLastError();
			if(dwError == ERROR_ACCESS_DENIED)
			{
				m_bSuccess = FALSE;
				m_bShared = FALSE;
			}
			else if(dwError == ERROR_ALREADY_EXISTS)
			{
				m_bSuccess = FALSE;
				m_bShared = FALSE;
			}
			else
			{
				m_bSuccess = TRUE;
				m_bShared = FALSE;
			}
		}

		if(m_bSuccess == FALSE)
			Release();
	}

	virtual void SharedObtain()
	{
		if(m_hMutexOneInstance != NULL || m_bSuccess != FALSE)
			return;

		/* Create and obtain the global mutex,
		   which can be shared to other process.
		   Use IsSuccess() to determine if obtain is successful. */
		m_hMutexOneInstance = ::CreateMutex(NULL, FALSE, m_tMutexName);

		if(m_hMutexOneInstance == NULL)
		{
			m_bSuccess = FALSE;
			m_bShared = FALSE;
		}
		else
		{
			DWORD dwError = ::GetLastError();
			if(dwError == ERROR_ACCESS_DENIED)
			{
				m_bSuccess = FALSE;
				m_bShared = FALSE;
			}
			else if(dwError == ERROR_ALREADY_EXISTS)
			{
				m_bSuccess = TRUE;
				m_bShared = TRUE;
			}
			else
			{
				m_bSuccess = TRUE;
				m_bShared = FALSE;
			}
		}

		if(m_bSuccess == FALSE)
			Release();
	}

	virtual void Release()
	{
		m_bSuccess = FALSE;
		if(m_hMutexOneInstance != NULL)
		{
			::CloseHandle(m_hMutexOneInstance);
			m_hMutexOneInstance = NULL;
			m_bSuccess = FALSE;
			m_bShared = FALSE;
		}
	}

	virtual BOOL IsSuccess() const
	{
		return m_bSuccess;
	}

	virtual BOOL IsShared() const
	{
		return m_bShared;
	}

protected:
	HANDLE	m_hMutexOneInstance;
	BOOL	m_bSuccess;
	BOOL	m_bShared;
	TCHAR	m_tMutexName[128];
};


class COneInstance2
{
public:
	COneInstance2()
	{
		m_hMutexOneInstance = NULL;
		m_uLockCount = 0;
	}

	COneInstance2(TCHAR *ptGlobalName)
	{
		m_hMutexOneInstance = NULL;
		m_uLockCount = 0;
		this->SetName(ptGlobalName);
	}

	virtual ~COneInstance2()
	{
		this->ReleaseAndClose();
	}

	virtual void SetName(const TCHAR *ptGlobalName)
	{
		/* Set the name of global mutex */
		_sntprintf_s(m_tMutexName, _countof(m_tMutexName), _TRUNCATE,
			_T("%s-5004780a-b256-43f3-88f5-1c1a798e40b3"), ptGlobalName);
	}

	virtual void Open()
	{
		if(m_hMutexOneInstance != NULL || m_uLockCount != 0)
			return;
		/* Create and obtain the global mutex,
		   which is not shared to other process */
		m_hMutexOneInstance = ::CreateMutex(NULL, FALSE, m_tMutexName);
		if(m_hMutexOneInstance != NULL)
		{
			DWORD dwError = ::GetLastError();
			if(dwError == ERROR_ACCESS_DENIED)
				this->Close();/* Failed */
		}
	}

	virtual void Close()
	{
		if(m_hMutexOneInstance != NULL)
		{
			::CloseHandle(m_hMutexOneInstance);
			m_hMutexOneInstance = NULL;
		}
	}

	virtual void Obtain(DWORD dwMilliseconds = INFINITE)
	{
		if(m_hMutexOneInstance == NULL)
			m_uLockCount = 0;
		else
		{
			DWORD dwWait = ::WaitForSingleObject(m_hMutexOneInstance, dwMilliseconds);
			if(dwWait == WAIT_OBJECT_0)
				++m_uLockCount;
		}
	}

	virtual void Release()
	{
		if(m_uLockCount > 0)
		{
			--m_uLockCount;
			BOOL bRet = ::ReleaseMutex(m_hMutexOneInstance);
			if(!bRet)
				throw std::runtime_error("ReleaseMutex(m_hMutexOneInstance)");
		}
	}

	virtual void OpenAndObtain(DWORD dwMilliseconds = INFINITE)
	{
		this->Open();
		this->Obtain(dwMilliseconds);
		if(m_uLockCount == 0)
			this->Close();
	}

	virtual void ReleaseAndClose()
	{
		this->Release();
		this->Close();
	}

	virtual BOOL IsSuccess() const
	{
		return (m_uLockCount == 0 ? FALSE : TRUE);
	}

protected:
	HANDLE			m_hMutexOneInstance;
	unsigned int	m_uLockCount;
	TCHAR			m_tMutexName[128];
};

template<class T>
class ShareArea
{
public:
	ShareArea()
		: m_hMapFile(NULL)
		, m_pBaseMapFile(NULL)
		, m_Mutex()
	{
	}

	virtual ~ShareArea()
	{
		Close();
		m_Mutex.Close();
	}

	virtual void SetName(const TCHAR *ptGlobalName)
	{
		/* Set the name of shared data */
		_sntprintf_s(m_tShareName, _countof(m_tShareName), _TRUNCATE,
			_T("%s-12d3688b-44c0-412a-85dd-5ebcfea9f503"), ptGlobalName);
		m_Mutex.SetName(ptGlobalName);
	}

	virtual void Close()
	{
		m_Mutex.Obtain(INFINITE);

		if(m_pBaseMapFile != NULL)
		{
			::UnmapViewOfFile(m_pBaseMapFile);
			m_pBaseMapFile = NULL;
		}

		if(m_hMapFile != NULL)
		{
			::CloseHandle(m_hMapFile);
			m_hMapFile = NULL;
		}

		m_Mutex.Release();
		m_Mutex.Close();
	}

	virtual void Open()
	{
		m_Mutex.Open();
		m_Mutex.Obtain(INFINITE);

		bool bInitialize = false;

		/* Map the share area and returns the mapped address */
		if(m_hMapFile == NULL)
		{
			//创建一个有名的共享内存
			m_hMapFile = ::CreateFileMapping(
				(HANDLE)INVALID_HANDLE_VALUE,	//INVALID_HANDLE_VALUE表示创建一个进程间共享的对象
				NULL,
				PAGE_READWRITE,  //读写共享
				0,
				sizeof(T),	//共享区间大小
				m_tShareName);
			if(m_hMapFile != NULL
				&& ::GetLastError() != ERROR_ALREADY_EXISTS)
			{
				//Initialize the object with 0
				bInitialize = true;
			}
		}

		if(m_pBaseMapFile == NULL)
		{
			if(m_hMapFile != NULL)
				m_pBaseMapFile = ::MapViewOfFile(  //映射到本进程的地址空间
					m_hMapFile,
					FILE_MAP_READ|FILE_MAP_WRITE,
					0,
					0,
					0);

			if(m_pBaseMapFile != NULL && bInitialize)
				memset(m_pBaseMapFile, 0, sizeof(T));
		}

		m_Mutex.Release();
	}

	virtual T *GetData()
	{
		if(!m_Mutex.IsSuccess())
			throw std::runtime_error("share area not locked");
		return reinterpret_cast<T *>(m_pBaseMapFile);
	}

	virtual T *Obtain()
	{
		/* Obtain the mapped address */
		m_Mutex.Obtain(INFINITE);
		return reinterpret_cast<T *>(m_pBaseMapFile);
	}

	virtual void Release()
	{
		if(m_pBaseMapFile != NULL)
			::FlushViewOfFile(m_pBaseMapFile, sizeof(T));
		/* Release the mapped address */
		m_Mutex.Release();
	}

protected:
	HANDLE	m_hMapFile;
	void	*m_pBaseMapFile;
	TCHAR	m_tShareName[128];
	COneInstance2	m_Mutex;
};


#endif
