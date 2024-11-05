#pragma once

class Mediator_
{

};

class Base_Module
{
public:
	Base_Module(Mediator_* Mediator = nullptr)
		: m_Mediator{ Mediator }
	{}

private:
	Mediator_* m_Mediator;
};