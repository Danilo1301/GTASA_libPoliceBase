#include "WindowTest.h"

#include "Log.h"
#include "CleoFunctions.h"
#include "Mod.h"

extern IMenuVSL* menuVSL;

IWindow* WindowTest::m_Window = NULL;

void WindowTest::Create()
{
    if(m_Window) return;

	auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Test Window";

    auto close = window->AddButton("> ~r~Close", CRGBA(0, 0, 0, 0));
    close->onClick = [window]() {
        Remove();
    };
}

void WindowTest::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}