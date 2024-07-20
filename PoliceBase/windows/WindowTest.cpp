#include "WindowTest.h"

#include "Log.h"
#include "CleoFunctions.h"
#include "Mod.h"
#include "Vehicles.h"

extern IMenuVSL* menuVSL;

IWindow* WindowTest::m_Window = NULL;

void WindowTest::Create()
{
    if(m_Window) return;

	auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Test Window";

    window->AddText("Vehicles: " + std::to_string(Vehicles::GetDefinedVehicles().size()), CRGBA(255, 255, 255));

    auto save_car_position = window->AddButton("Save car position", CRGBA(0, 0, 0, 0));
    save_car_position->onClick = [window]() {
        
        auto playerActor = CleoFunctions::GetPlayerActor();
        auto vehicleHandle = CleoFunctions::GetVehiclePedIsUsing(playerActor);
        if(!vehicleHandle)
        {
            return;
        }

        auto position = CleoFunctions::GetCarPosition(vehicleHandle);
        auto angle = CleoFunctions::GET_CAR_Z_ANGLE(vehicleHandle);
        auto modelId = CleoFunctions::GET_CAR_MODEL(vehicleHandle);

        Log::Level(LOG_LEVEL::LOG_BOTH) << "---- SAVED POSITION ----" << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << position.x << ", " << position.y << ", " << position.z << ", " << angle << ", " << modelId << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "------------------------" << std::endl;
        Remove();
    };

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