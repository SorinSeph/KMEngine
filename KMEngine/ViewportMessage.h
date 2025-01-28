#ifndef VIEWPORTMESSAGE_H
#define VIEWPORTMESSAGE_H

//#include "UIModule.h"

class CUIModule;

class CViewportMessage
{
public:
	static CViewportMessage& GetViewportMessage()
	{
		static CViewportMessage ViewportMessage;
		return ViewportMessage;
	}

	CUIModule* m_pUIModule{};
	////template <typename ...Args>
	////void Send(auto&& Ptr, Args&&... args);

	void SendToUIModule(int MouseX, int MouseY);

	float m_MouseX;
	float m_MouseY;

	int m_TestInt;
};

#endif