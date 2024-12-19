#pragma once

class CMediator
{
public:
	template <typename TObject, typename... Args>
	void Notify(TObject&& Ptr, Args&&... args) const
	{
		Ptr(args...);
	}
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