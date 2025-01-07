#include <any>
#include "ViewportMessage.h"
#include "UIModule.h"
#include "GraphicsModule.h"

void CViewportMessage::SendToUIModule(int MouseX, int MouseY)
{		
    CLogger& Logger = CLogger::GetLogger();
    if (m_pUIModule)
    {
        m_pUIModule->TestLog(MouseX, MouseY);
        Logger.Log("ViewportMessage.cpp, SendToUIModule(): Pointer to UI Module for test logging true");
    }
    else
    {
        Logger.Log("ViewportMessage.cpp, SendToUIModule(): Pointer to UI Module for test logging is null");
    }

    if (m_pUIModule)
    {
        if (m_pUIModule->m_Mediator)
        {
            try
            {
                auto graphicsModule = std::any_cast<CGraphicsModule*>(m_pUIModule->m_Mediator->m_ModuleVector[1]);
                if (graphicsModule)
                {
                    graphicsModule->m_Renderer.Raycast(MouseX, MouseY);
                }
                else
                {
                    Logger.Log("ViewportMessage.cpp, SendToUIModule(): Cast to GraphicsModule failed");
                }
            }
            catch (const std::bad_any_cast& e)
            {
                Logger.Log("ViewportMessage.cpp, SendToUIModule(): Bad any cast: ", e.what());
            }
        }
        else
        {
			Logger.Log("ViewportMessage.cpp, SendToUIModule(): Pointer to Mediator in UI Module is null");
        }
    }
}