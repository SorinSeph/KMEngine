#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <tuple>

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

	std::vector<CBaseTimer*> m_TimersArray;
	
	void Start();

	void EngineTick();

	void Tick();

	void Reset();

	double GetDDeltaTime() const;

	float GetFDeltaTime() const;

	float GetFTotalTime() const;

	void AddTimer(CBaseTimer* InTimer);

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

	std::string m_TimerHandle;
	bool bRunning;
	bool bRunOnce;
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

template <typename TClass, typename TReturnType, typename... TArgs>
class CTimer3 : public CBaseTimer
{
public:
	using FnPtr = TReturnType(TClass::*)(TArgs...);

	CTimer3(TClass* Class, FnPtr Ptr, TArgs... args)
		: m_ClassType(Class), m_FnPtr(Ptr), m_Args(std::make_tuple(args...))
	{
	}

	void Execute()
	{
		std::apply([this](TArgs... unpackedArgs) {
			(m_ClassType->*m_FnPtr)(unpackedArgs...);
		}, m_Args);
	}

private:
	TClass* m_ClassType;
	FnPtr m_FnPtr;
	std::tuple<TArgs...> m_Args;
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

template <typename TObject, typename...TArgs>
class CTimerVariadicArgs : public CBaseTimer
{
public:
	//using FnPtr = void (TObject::*)(TArgs...);
	typedef void (TObject::* FnPtr)(TArgs...);

	CTimerVariadicArgs(TObject Obj, FnPtr Fn, TArgs... Args)
		: m_Object(Obj), m_FnPtr(Fn), m_Args(std::make_tuple(Args...)) {
	}

	void Execute() override
	{
		std::apply([this](auto&&... Args) {
			(m_Object.*m_FnPtr)(Args...);
		}, m_Args);
	}

	TObject m_Object;
	FnPtr m_FnPtr;
	std::tuple<TArgs...> m_Args;
};

template <typename TLambda, typename... TArgs>
class CTimerVariadicArgsLambda : public CBaseTimer
{
public:
	CTimerVariadicArgsLambda(TLambda Fn, TArgs... Args)
		: m_FnPtr(Fn), m_Args(std::make_tuple(Args...)) {
	}

	void Execute() override
	{
		std::apply(m_FnPtr, m_Args);
	}

	TLambda m_FnPtr;
	std::tuple<TArgs...> m_Args;
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

	template <typename TClass, typename TReturnType, typename TReturnType(TClass::* TPtr)()>
	void SetSingleTimer3(TClass* TObject, float InStartTime)
	{
		if (m_pCoreClock)
		{
			CTimer2<TClass, TReturnType, TPtr>* Timer{ new CTimer2<TClass, TReturnType, TPtr>(TObject) };
			Timer->StartTime = InStartTime;
			Timer->EndTime = 0;
			Timer->bRunOnce = true;
			m_pCoreClock->AddTimer(Timer);
		}
	}

	template<typename TObject, typename... TArgs>
	static void SetTimer4(float InStartTime, float InEndTime, TObject Obj, void (TObject::* Fn)(TArgs...), TArgs... Args)
	{
		auto* Timer = new CTimerVariadicArgs<TObject, TArgs...>(Obj, Fn, Args...);
		Timer->StartTime = InStartTime;
		Timer->EndTime = InEndTime;
		
		m_pCoreClock->AddTimer(Timer);
	}

	template<typename TLambda, typename... TArgs>
	void SetTimerVariadicArgsLambda(std::string TimerHandle, float InStartTime, float InEndTime, TLambda Fn, TArgs... Args)
	{
		CTimerVariadicArgsLambda<TLambda, TArgs...>* Timer = new CTimerVariadicArgsLambda<TLambda, TArgs...>(Fn, Args...);
		Timer->StartTime = InStartTime;
		Timer->EndTime = InEndTime;
		Timer->m_TimerHandle = TimerHandle;
		Timer->bRunOnce = false;

		m_pCoreClock->AddTimer(Timer);
	}

	template<typename TObject, typename... TArgs>
	static void SetSingleTimer4(float InStartTime, float InEndTime, TObject Obj, void (TObject::* Fn)(TArgs...), TArgs... Args)
	{
		auto* Timer = new CTimerVariadicArgs<TObject, TArgs...>(Obj, Fn, Args...);
		Timer->StartTime = InStartTime;
		Timer->EndTime = InEndTime;
		Timer->bRunOnce = true;

		m_pCoreClock->AddTimer(Timer);
	}

	template<typename TLambda, typename... TArgs>
	void SetSingleTimerVariadicArgsLambda(float InStartTime, float InEndTime, TLambda Fn, TArgs... Args)
	{
		auto* Timer = new CTimerVariadicArgsLambda<TLambda, TArgs...>(Fn, Args...);
		Timer->StartTime = InStartTime;
		Timer->EndTime = InEndTime;
		Timer->bRunOnce = true;

		m_pCoreClock->AddTimer(Timer);
	}


	//template <typename TClass, typename TReturnType, typename TArg, typename TReturnType(TClass::* TPtr)(TArg)>
	//void SetTimer(TClass TObject, TReturnType RetType, TArg Arg, TReturnType(TClass::* InPtr)(TArg))
	//{
	//	BTimerParam<TObject, RetType, Arg, InPtr>* Timer{ new BTimerParam<TObject, RetType, Arg, InPtr>(TObject, Arg) };
	//	if (CoreTimerRef)
	//		CoreTimerRef->AddTimer(Timer);
	//}

	void RemoveTimer(std::string TimerHandle)
	{
		for (int i = 0; i < m_pCoreClock->m_TimersArray.size(); i++)
		{
			if (m_pCoreClock->m_TimersArray[i]->m_TimerHandle == TimerHandle)
			{
				m_pCoreClock->m_TimersArray.erase(m_pCoreClock->m_TimersArray.begin() + i);
				break;
			}
		}
	}

	CCoreClock* m_pCoreClock{ nullptr };
};

