#pragma once

class CMediator
{
public:
	template <typename TObject>
	void Notify(TObject* TComponent, void(*Ptr)(TObject*)) const
	{
		Ptr(TComponent);
	}
};

class CBaseModule
{
public:
	void SetMediator(CMediator& Mediator)
	{
		m_Mediator = Mediator;
	}

private:
	CMediator m_Mediator;
};