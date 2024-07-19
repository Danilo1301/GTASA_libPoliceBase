#pragma once

#include "pch.h"

struct CarLocation {
    int modelId;
    float angle;
    CVector position = CVector(0, 0, 0);
};

class CarsLocations {
public:
    static std::vector<CarLocation> m_Locations;

    static void Init();
    static void Update();
};