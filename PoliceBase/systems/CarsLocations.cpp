#include "CarsLocations.h"

#include "Vehicles.h"
#include "CleoFunctions.h"
#include "ModConfig.h"

#include "Log.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

#include "IMultiRemap.h"
extern IMultiRemap* multiRemap;

std::vector<CarLocation> CarsLocations::m_Locations;

void CarsLocations::Init()
{
    /*
    CarLocation car1;
    car1.modelId = 523;
    car1.angle = 90.0f;
    car1.position = CVector(2555.39, -1768.24, 13.3879);
    m_Locations.push_back(car1);
    */
    //2555.39, -1768.24, 13.3879, 87.4177, 596
}

void CarsLocations::Update(int dt)
{
    auto playerActor = CleoFunctions::GetPlayerActor();
    auto playerPosition = CleoFunctions::GetPedPosition(playerActor);

    for (size_t i = 0; i < m_Locations.size(); ++i)
    {
        auto location = &m_Locations[i];

        auto position = location->position;

        auto distanceFromPlayer = DistanceBetweenPoints(playerPosition, position);
        bool isCloseToPlayer = distanceFromPlayer < 100.0f;

        bool canSpawnCar = true;

        if(location->prevVehicleSpawned > 0)
        {
            //TODO: check if vehicle exists, otherwise will crash
            if(CleoFunctions::CAR_DEFINED(location->prevVehicleSpawned))
            {
                auto prevVehiclePosition = CleoFunctions::GetCarPosition(location->prevVehicleSpawned);
                auto distanceFromLastVehicle = DistanceBetweenPoints(position, prevVehiclePosition);

                //menuVSL->debug->AddLine("distance: " + std::to_string(distanceFromLastVehicle));

                if(distanceFromLastVehicle < 20.0f) canSpawnCar = false;
            }
        }

        if(isCloseToPlayer && canSpawnCar)
        {
            if(!Vehicles::HasVehicleOfModelIdInArea(location->modelId, location->position, 3.0f))
            {
                //menuVSL->debug->m_Visible = true;
                //menuVSL->debug->AddLine("Spawning car " + std::to_string(location->modelId));

                auto carHandle = CleoFunctions::CREATE_CAR_AT(location->modelId, position.x, position.y, position.z);
                auto vehicle = Vehicles::TryCreateVehicle(carHandle);

                CleoFunctions::REMOVE_REFERENCES_TO_CAR(carHandle);
                CleoFunctions::SET_CAR_DOOR_STATUS(carHandle, 1);
                CleoFunctions::SET_CAR_Z_ANGLE(carHandle, location->angle);

                location->prevVehicleSpawned = carHandle;

                Log::Level(LOG_LEVEL::LOG_BOTH) << "Trying to set remap..." << std::endl;

                if(multiRemap)
                {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "Setting remap: " << location->remap << std::endl;
                    multiRemap->SetVehicleRemap(carHandle, location->remap);
                } else {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "Multi remap is not loaded" << std::endl;
                }
            }
        }
    }
}

void CarsLocations::Load()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "CarsLocations: Load" << std::endl;

    auto path = ModConfig::GetConfigFolder() + "/vehicles/";

    auto files = ModConfig::GetFilesName(path);
    for(auto file : files)
    {
        auto filePath = path + "/" + file;

        Log::Level(LOG_LEVEL::LOG_BOTH) << "Found file: " << file << std::endl;

        std::ifstream inputFile(filePath);
        if (!inputFile) {
            continue;
        }

        std::string line;
        while (std::getline(inputFile, line))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << line << std::endl;

            int modelId;
            float x, y, z, angle;
            char remap[50];

            if (sscanf(line.c_str(), "%d %f %f %f %f %49s", &modelId, &x, &y, &z, &angle, remap) == 6)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "modelId: " << modelId << std::endl;
                Log::Level(LOG_LEVEL::LOG_BOTH) << "remap: " << remap << std::endl;

                CarLocation car1;
                car1.modelId = modelId;
                car1.angle = angle;
                car1.position = CVector(x, y, z);
                car1.remap = remap;
                m_Locations.push_back(car1);

            } else {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Error analyzing line" << std::endl;
            }
        }

        inputFile.close();
    }
}