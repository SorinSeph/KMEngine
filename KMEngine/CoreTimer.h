#include <Windows.h>
#include <vector>

class BTimerCore;

class BCoreTimer
{
public:
	BCoreTimer()
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

	void AddTimer(BTimerCore* InTimer);

private:
	std::vector<BTimerCore*> m_TimersArray;
	double m_SecondsPerCount;
	double m_DeltaTime;
	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;
	bool m_Stopped;
};

class BTimerCore
{
public:
	virtual void Execute() = 0;

	bool bRunning;
	double StartTime;
	double EndTime;
};

template <typename Class, typename ReturnType, typename ReturnType(Class::* FuncPtr)()>
class BTimer : public BTimerCore
{
public:
	typedef ReturnType(Class::* MemFnPtr)();

	BTimer(Class InArg)
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

template <typename TClass, typename TReturnType, typename TArg, typename TReturnType(TClass::* TPtr)(TArg)>
class BTimerParam : public BTimerCore
{
public:
	typedef TReturnType(TClass::* FnPtr)(TArg);

	BTimerParam(TClass TObject, TArg Arg)
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

class FTimerManager
{
private:
	FTimerManager() {};

public:
	static FTimerManager& GetTimerManager();

	template <typename TClass, typename TReturnType, typename TReturnType(TClass::* TPtr)()>
	void SetTimer(TClass TObject, float InEndTime)
	{
		if (CoreTimerRef)
		{
			BTimer<TClass, TReturnType, TPtr>* Timer{ new BTimer<TClass, TReturnType, TPtr>(TObject) };
			Timer->StartTime = CoreTimerRef->GetFTotalTime() + InEndTime;

			CoreTimerRef->AddTimer(Timer);
		}
	}

	//template <typename TClass, typename TReturnType, typename TArg, typename TReturnType(TClass::* TPtr)(TArg)>
	//void SetTimer(TClass TObject, TReturnType RetType, TArg Arg, TReturnType(TClass::* InPtr)(TArg))
	//{
	//	BTimerParam<TObject, RetType, Arg, InPtr>* Timer{ new BTimerParam<TObject, RetType, Arg, InPtr>(TObject, Arg) };
	//	if (CoreTimerRef)
	//		CoreTimerRef->AddTimer(Timer);
	//}

	BCoreTimer* CoreTimerRef{ nullptr };
};