#include "CarsLocations.h"

#include "Vehicles.h"
#include "CleoFunctions.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

std::vector<CarLocation> CarsLocations::m_Locations;

void CarsLocations::Init()
{
    CarLocation car1;
    car1.modelId = 596;
    car1.angle = 269.0f;
    car1.position = CVector(1546.0135, -1653.3127, 13.1718);
    m_Locations.push_back(car1);
    //AddStaticVehicle(596,1546.0135,-1653.3127,13.1718,269.4309,168,162); // 
}

void CarsLocations::Update()
{
    auto playerActor = CleoFunctions::GetPlayerActor();
    auto playerPosition = CleoFunctions::GetPedPosition(playerActor);
    
    for(auto location : m_Locations)
    {
        auto position = location.position;

        auto distanceFromPlayer = DistanceBetweenPoints(playerPosition, position);
        bool isCloseToPlayer = distanceFromPlayer < 20.0f;

        if(isCloseToPlayer)
        {
            if(!Vehicles::HasVehicleOfModelIdInArea(location.modelId, location.position, 3.0f))
            {
                menuVSL->debug->m_Visible = true;
                menuVSL->debug->AddLine("Spawning car " + std::to_string(location.modelId));

                auto carHandle = CleoFunctions::CREATE_CAR_AT(location.modelId, position.x, position.y, position.z);
                auto vehicle = Vehicles::TryCreateVehicle(carHandle);

                CleoFunctions::SET_CAR_DOOR_STATUS(carHandle, 1);
                CleoFunctions::SET_CAR_Z_ANGLE(carHandle, location.angle);
            }
        }
    }
}