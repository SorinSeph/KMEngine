#pragma once
#include <vector>
#include <any>

class CBaseModule;

class CMediator
{
public:
	template <typename TObject, typename... Args>
	void Notify(TObject&& Ptr, Args&&... args) const
	{
		Ptr(args...);
	}

	std::vector<CBaseModule*> m_Modules;

	std::vector<std::any> m_ModuleVector;
};

class CBaseModule
{
public:
	void SetMediator(CMediator& Mediator)
	{
		m_Mediator = &Mediator;
	}

	template <typename... Args>
	void Notify(auto&& Ptr, Args&&... args)
	{
		//std::cout << "Component 2 does Variadic C.\n";
		m_Mediator->Notify(Ptr, args...);
	}

	CMediator* m_Mediator;
};