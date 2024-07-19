#include "Vehicle.h"

#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"
#include "Widgets.h"

extern void* (*GetVehicleFromRef)(int);

Vehicle::Vehicle(int hVehicle)
{
    this->hVehicle = hVehicle;
    this->pVehicle = (CVehicle*)GetVehicleFromRef(hVehicle);
    this->modelId = CleoFunctions::GET_CAR_MODEL(hVehicle);
}

Vehicle::~Vehicle()
{
    
}

void Vehicle::Update(int dt)
{
    
}