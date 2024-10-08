#pragma once

#include "pch.h"

#include "Vehicle.h"

class Vehicles {
public:
    static std::map<int, Vehicle*> m_Vehicles;

    static void Update(int dt);

    static void TryFindNewVehicles();

    static bool HasVehicleHandle(int hVehicle);
    static Vehicle* TryCreateVehicle(int hVehicle);
    static Vehicle* GetVehicleByHandle(int hVehicle);
    static void RemoveVehicle(int hVehicle);
    
    static std::vector<Vehicle*> GetAllCarsInSphere(CVector position, float radius);
    static int GetRandomCarInSphere(CVector position, float radius);
    static int GetClosestCar(CVector position, float radius);
    static std::vector<Vehicle*> GetDefinedVehicles();
    static bool HasVehicleOfModelIdInArea(int modelId, CVector position, float radius);
};