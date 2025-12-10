#ifndef UIMESSAGEQUEUE_H
#define UIMESSAGEQUEUE_H

#include <string>

class CUIModule;

class CUIMessageQueue
{
public:
	static CUIMessageQueue& GetUIMessageQueue()
	{
		static CUIMessageQueue UIMessageQueue;
		return UIMessageQueue;
	}

	CUIModule* m_pUIModule{};
	////template <typename ...Args>
	////void Send(auto&& Ptr, Args&&... args);

	void RayPicking(int MouseX, int MouseY);

	void RaycastOpenGL(int MouseX, int MouseY);

	void ImportGLTF(std::string& FilePath, const std::string& FileContent);

	bool m_bIsEntitySelected{ false };
	bool m_bIsDraggingGizmo{ false };
	bool m_bIsLeftMouseButtonDown{false};

	float m_MouseX;
	float m_MouseY;

	float PreviousX{ 0.0f };
	float PreviousY{ 0.0f };

	int m_TestInt;
};

#endif