#pragma once

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

	void RayPicking(int MouseX, int MouseY);

	void RaycastOpenGL(int MouseX, int MouseY);

	void ImportGLTF(std::string& FilePath, const std::string& FileContent);

	void PlayGLTFAnimation(std::string& FilePath, const std::string& FileContent);

	bool m_bIsEntitySelected{ false };
	bool m_bIsDraggingGizmo{ false };
	bool m_bIsLeftMouseButtonDown{false};

	float m_MouseX;
	float m_MouseY;

	float PreviousX{ 0.0f };
	float PreviousY{ 0.0f };

	int m_TestInt;

	// @Temporary function used by the time manager in PlayGLTFAnimation, to check if the function is ticking during lifespan properly
	void TestPlayGLTFAnimation(); 
	float m_QuatX{ 0.0f };
	float m_QuatY{ 0.0f };
	float m_QuatZ{ 0.0f };
	float m_QuatW{ 0.0f };
};

#endif