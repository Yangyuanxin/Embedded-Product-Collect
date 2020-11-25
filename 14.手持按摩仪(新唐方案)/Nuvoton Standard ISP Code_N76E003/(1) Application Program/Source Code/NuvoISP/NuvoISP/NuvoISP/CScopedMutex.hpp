#if !defined(_CMUTEX2_H_)
#define _CMUTEX2_H_

class CMutex2
{
public:
	CMutex2()
		: m_bLocked(FALSE)
		, m_uLockCount(0)
		, m_dwThread(-1)
	{
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	}

	virtual ~CMutex2()
	{
		::ReleaseMutex(m_hMutex);
	}

	void Lock()
	{
		if (!m_bLocked || m_dwThread != ::GetCurrentThreadId())
		//if (m_dwThread != ::GetCurrentThreadId())
		{
			::WaitForSingleObject(m_hMutex, INFINITE);
			m_dwThread	= ::GetCurrentThreadId();
			m_bLocked	= TRUE;
		}

		++m_uLockCount;
	}

	void Unlock()
	{
		if (--m_uLockCount == 0)
		{
			m_bLocked = FALSE;
		//	m_dwThread = -1;
			::ReleaseMutex(m_hMutex);
		}
	}

	void Recursive_Lock(unsigned int uLockCount)
	{
		while(uLockCount-- != 0)
			Lock();
		Unlock();
	}

	unsigned int Recursive_Unlock()
	{
		Lock();
		unsigned int uRet		= m_uLockCount;
		unsigned int uLockCount = m_uLockCount;
		while(uLockCount-- != 0)
			Unlock();
		return uRet;
	}

	unsigned int GetLockCount() const
	{
		return m_uLockCount;
	}
protected:
	BOOL			m_bLocked;
	DWORD			m_dwThread;
	HANDLE			m_hMutex;
	unsigned int	m_uLockCount;
};


class ScopedMutex
{
public:
	explicit ScopedMutex(CMutex2 &mutex, BOOL bUnlock = FALSE)
		: m_bUnlock(bUnlock)
		, m_pMutex(&mutex)
	{
		if (m_bUnlock)
		{
			m_uLockCount = m_pMutex->Recursive_Unlock();
		}
		else
		{
			m_pMutex->Lock();
			//DGB_PRINTF("In lock id = %d, count = %d\n", (int)GetCurrentThreadId(), m_pMutex->GetLockCount());
			//DGB_PRINTF("In lock %d, id = %d\n", line, (int)GetCurrentThreadId());
			//DGB_PRINTF("In lock, %x, %d\n", &hMutex, GetTickCount());
		}
	}

	virtual ~ScopedMutex( )
	{
		//DGB_PRINTF("In unlock, id = %d, count = %d\n", (int)GetCurrentThreadId(), m_pMutex->GetLockCount());
		//DGB_PRINTF("In unlock, %d\n", GetTickCount());
		//::ReleaseMutex(m_hMutex);	//Unlock

		if (m_bUnlock)
		{
			m_pMutex->Recursive_Lock(m_uLockCount);
		}
		else
			m_pMutex->Unlock();
	}

protected:
	CMutex2			*m_pMutex;
	BOOL			m_bUnlock;
	unsigned int	m_uLockCount;
};

#endif // #if !defined(_CMUTEX2_H_)