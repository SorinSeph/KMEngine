#pragma once
#include <vector>
#include <any>

#define CORE_MODULE_COUNT 5

class CBaseModule;

class CMediator
{
public:
	template <typename TObject, typename... Args>
	void Notify(TObject&& Ptr, Args&&... args) const
	{
		Ptr(args...);
	}

	CBaseModule *m_ModuleArray[CORE_MODULE_COUNT];
};

class CBaseModule
{
public:
	void SetMediator(CMediator& Mediator)
	{
		m_pMediator = &Mediator;
	}

	template <typename... Args>
	void Notify(auto&& Ptr, Args&&... args)
	{
		m_pMediator->Notify(Ptr, args...);
	}

	CMediator* m_pMediator;
};