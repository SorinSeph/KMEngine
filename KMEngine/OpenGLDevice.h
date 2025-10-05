#pragma once

#include <windows.h>
#include <glad/glad.h>

class COpenGLDevice
{
public:
	COpenGLDevice() = default;

    COpenGLDevice(HWND hwnd, HDC hdc, HGLRC hglrc)
        : m_Hwnd{ hwnd }
        , m_HDC{ hdc }
        , m_HGLRC{ hglrc }
    {}

    void SetOpenGLContext()
    {
        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        int pixelFormat = ChoosePixelFormat(m_HDC, &pfd);
        SetPixelFormat(m_HDC, pixelFormat, &pfd);

        m_HGLRC = wglCreateContext(m_HDC);
        wglMakeCurrent(m_HDC, m_HGLRC);

        // Initialize glad after context is current!
        if (!gladLoadGL()) {
            MessageBoxA(0, "Failed to initialize GLAD", "Error", 0);
            exit(-1);
        }
    }

    HWND m_Hwnd;
    HDC m_HDC = nullptr;
    HGLRC m_HGLRC = nullptr;
};