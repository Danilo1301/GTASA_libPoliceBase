#pragma once

#include "pch.h"

class Mod {
public:
    static unsigned int m_TimePassed;
    static bool m_Enabled;
    static bool m_DevModeEnabled;

    static void Update(int dt);
    static void Init();
    static void CleoInit();

    static void RequestModelsToLoad();
    static void LoadRequestedModels(std::function<void()> callback);
    static void AddModelToLoad(int modelId);
    static bool CheckModelsLoaded();

    /**
      *   \brief Calculate probability
      *
      *   Description
      *   
      *   \param chance Float from 0.0 to 1.0
      *   \return Bool
      **/
    static bool CalculateProbability(float chance);
    
    static void ToggleMod(bool enabled);
    static void ShowCredits();
};