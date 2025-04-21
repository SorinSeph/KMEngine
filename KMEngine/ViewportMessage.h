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

	bool m_bIsEntitySelected{ false };
	bool m_bIsDraggingGizmo{ false };

	float m_MouseX;
	float m_MouseY;

	float PreviousX{ 0.0f };
	float PreviousY{ 0.0f };

	int m_TestInt;
};

#endif