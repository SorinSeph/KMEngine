#include "OpenGLResource.h"

void COpenGLResource::Execute()
{
	for (auto It : m_pContextResourcePtr)
	{
		It();
	}
}
