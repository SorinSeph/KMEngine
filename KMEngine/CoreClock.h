#include <Windows.h>
#include <vector>

class CBaseTimer;

class CCoreClock
{
public:
	CCoreClock()
		: m_TimersArray{}
		, m_SecondsPerCount{ 0.0 }
		, m_DeltaTime{ -1 }
		, m_BaseTime{ 0 }
		, m_PausedTime{ 0 }
		, m_StopTime{ 0 }
		, m_CurrTime{ 0 }
		, m_Stopped{ false }
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		m_SecondsPerCount = 1.0 / (double)countsPerSec;
	}

	void Start();

	void EngineTick();

	void Tick();

	void Reset();

	double GetDDeltaTime() const;

	float GetFDeltaTime() const;

	float GetFTotalTime() const;

	void AddTimer(CBaseTimer* InTimer);

private:
	std::vector<CBaseTimer*> m_TimersArray;
	double m_SecondsPerCount;
	double m_DeltaTime;
	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;
	bool m_Stopped;
};

class CBaseTimer
{
public:
	virtual void Execute() = 0;

	bool bRunning;
	double StartTime;
	double EndTime;
};

template <typename Class, typename ReturnType, typename ReturnType(Class::* FuncPtr)()>
class CTimer : public CBaseTimer
{
public:
	typedef ReturnType(Class::* MemFnPtr)();

	CTimer(Class InArg)
	{
		m_ClassType = InArg;
		m_FuncPtr = FuncPtr;
	}

	virtual void Execute() override
	{
		(m_ClassType.*m_FuncPtr)();
	}

	Class m_ClassType;
	MemFnPtr m_FuncPtr;
};

template <typename Class, typename ReturnType, typename ReturnType(Class::* FuncPtr)()>
class CTimer2 : public CBaseTimer
{
public:
	typedef ReturnType(Class::* MemFnPtr)();

	CTimer2(Class* InArg)
	{
		m_ClassType = InArg;
		m_FuncPtr = FuncPtr;
	}

	virtual void Execute() override
	{
		(m_ClassType->*m_FuncPtr)();
	}

	Class* m_ClassType;
	MemFnPtr m_FuncPtr;
};

template <typename TClass, typename TReturnType, typename TArg, typename TReturnType(TClass::* TPtr)(TArg)>
class CTimerParam : public CBaseTimer
{
public:
	typedef TReturnType(TClass::* FnPtr)(TArg);

	CTimerParam(TClass TObject, TArg Arg)
	{
		m_ClassType = TObject;
		m_FnPtr = TPtr;
		m_Arg = Arg;
	}

	void Execute()
	{
		(m_ClassType.*m_FnPtr)(m_Arg);
	}

	TClass m_ClassType;
	FnPtr m_FnPtr;
	TArg m_Arg;
};

class CTimerManager
{
private:
	CTimerManager() {};

public:
	static CTimerManager& GetTimerManager();

	template <typename TClass, typename TReturnType, typename TReturnType(TClass::* TPtr)()>
	void SetTimer(TClass TObject, float InEndTime)
	{
		if (m_pCoreClock)
		{
			CTimer<TClass, TReturnType, TPtr>* Timer{ new CTimer<TClass, TReturnType, TPtr>(TObject) };
			Timer->StartTime = m_pCoreClock->GetFTotalTime() + InEndTime;

			m_pCoreClock->AddTimer(Timer);
		}
	}

	template <typename TClass, typename TReturnType, typename TReturnType(TClass::* TPtr)()>
	void SetTimer2(TClass TObject, float InStartTime, float InEndTime)
	{
		if (m_pCoreClock)
		{
			CTimer<TClass, TReturnType, TPtr>* Timer{ new CTimer<TClass, TReturnType, TPtr>(TObject) };
			Timer->StartTime = InStartTime;
			Timer->EndTime = InEndTime;
			m_pCoreClock->AddTimer(Timer);
		}
	}

	template <typename TClass, typename TReturnType, typename TReturnType(TClass::* TPtr)()>
	void SetTimer3(TClass* TObject, float InStartTime, float InEndTime)
	{
		if (m_pCoreClock)
		{
			CTimer2<TClass, TReturnType, TPtr>* Timer{ new CTimer2<TClass, TReturnType, TPtr>(TObject) };
			Timer->StartTime = InStartTime;
			Timer->EndTime = InEndTime;
			m_pCoreClock->AddTimer(Timer);
		}
	}

	//template <typename TClass, typename TReturnType, typename TArg, typename TReturnType(TClass::* TPtr)(TArg)>
	//void SetTimer(TClass TObject, TReturnType RetType, TArg Arg, TReturnType(TClass::* InPtr)(TArg))
	//{
	//	BTimerParam<TObject, RetType, Arg, InPtr>* Timer{ new BTimerParam<TObject, RetType, Arg, InPtr>(TObject, Arg) };
	//	if (CoreTimerRef)
	//		CoreTimerRef->AddTimer(Timer);
	//}

	CCoreClock* m_pCoreClock{ nullptr };
};