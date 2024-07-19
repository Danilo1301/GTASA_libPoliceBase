#include "Mod.h"

#include "Widgets.h"
#include "Log.h"
#include "CleoFunctions.h"
#include "ModConfig.h"

#include "windows/WindowTest.h"

unsigned int Mod::m_TimePassed = 0;
bool Mod::m_Enabled = false;
bool Mod::m_DevModeEnabled = false;

bool hasCleoInitialized = false;
std::vector<int> modelsToLoad;

void Mod::Update(int dt)
{
    m_TimePassed += dt;

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "cleofunctions -----------------" << std::endl;

    CleoFunctions::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "widgets" << std::endl;

    Widgets::Update(dt);
    
    if(CleoFunctions::PLAYER_DEFINED(0))
    {
        if(!hasCleoInitialized)
        {
            hasCleoInitialized = true;

            CleoInit();
        }
    }

    if(!WindowTest::m_Window)
    {
        if(Widgets::IsWidgetJustPressed(40)) //40 = blue joystick button
        {
            WindowTest::Create();
        }
    }

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "end ---------" << std::endl;
}

void Mod::Init()
{
    
}

void Mod::CleoInit()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Cleo init" << std::endl;
}

void Mod::RequestModelsToLoad()
{
    AddModelToLoad(596); //copcarla
    
    LoadRequestedModels([] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: Models loaded!" << std::endl;
    });
}

void Mod::LoadRequestedModels(std::function<void()> callback)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: load requested models" << std::endl;

    CleoFunctions::LOAD_REQUESTED_MODELS();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "AddWaitForFunction" << std::endl;

    CleoFunctions::AddWaitForFunction([] () {
        return CheckModelsLoaded();
    }, [callback] () {
        callback();
    });
}

void Mod::AddModelToLoad(int modelId)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Mod: loading model " << modelId << std::endl;

    modelsToLoad.push_back(modelId);

    CleoFunctions::LOAD_MODEL(modelId);
}

bool Mod::CheckModelsLoaded()
{
    if(modelsToLoad.size() == 0) return true;

    std::vector<int> newLoadedModels;
    for(auto modelId : modelsToLoad)
    {
        if(CleoFunctions::MODEL_AVAILABLE(modelId))
        {
            newLoadedModels.push_back(modelId);
            
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Model " << modelId << " has been loaded" << std::endl;
        }
    }

    for(auto modelId : newLoadedModels)
    {
        auto it = std::find(modelsToLoad.begin(), modelsToLoad.end(), modelId);
        modelsToLoad.erase(it);
    }

    return false;
}

bool Mod::CalculateProbability(float chance)
{
    int i = GetRandomNumber(0, 100);
    return i <= (int)(chance * 100.0f);
}

void Mod::ToggleMod(bool enabled)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Toggle mod: " << (enabled ? "Enabled" : "Disabled") << std::endl;

    m_Enabled = enabled;
}

void Mod::ShowCredits()
{
    CleoFunctions::SHOW_TEXT_BOX("MPFX140"); //mod policia credits
}