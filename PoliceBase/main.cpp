#include "pch.h"

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "Log.h"
#include "Mod.h"
#include "ModConfig.h"

// ---------------------------------------

MYMODCFG(net.danilo1301.policeBase, PoliceBase, 1.0.0, Danilo1301)

// ---------------------------------------

// CLEO 2.0.1.5
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// SAUtils 1.6 (but it says 1.5.2)
#include "isautils.h"
ISAUtils* sautils = NULL;

#include "menu/IMenuVSL.h"
IMenuVSL* menuVSL = NULL;

#include "IMultiRemap.h"
IMultiRemap* multiRemap = NULL;

// ---------------------------------------

uintptr_t* pVehiclePool = 0;
uintptr_t* pPedPool = 0;

int (*GetVehicleRef)(int);
void* (*GetVehicleFromRef)(int);
int (*GetPedRef)(int);
void* (*GetPedFromRef)(int);

// ---------------------------------------

DECL_HOOK(void*, UpdateGameLogic, uintptr_t a1)
{
    Mod::Update(menuVSL->deltaTime);

    return UpdateGameLogic(a1);
}

// ---------------------------------------

std::string CheckModVersion(std::vector<std::string> GUIDs, std::vector<std::string> versions)
{
    for (int ii = GUIDs.size() - 1; ii >= 0; ii--)
    {
        auto GUID = GUIDs[ii];

        for (int i = versions.size() - 1; i >= 0; i--)
        {
            std::string version = versions[i];

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Checking " << GUID << " " << version << "..." << std::endl;

            if (aml->HasModOfVersion(GUID.c_str(), version.c_str()))
                return version;
        }
    }
    return "";
}

template <typename T>
T* LoadInterface(T** out, std::string name)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading interface: " << name << "..." << std::endl;

    void* interface = GetInterface(name.c_str());

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Interface: " << interface << std::endl;

    *out = static_cast<T*>(interface);

    if (*out) Log::Level(LOG_LEVEL::LOG_BOTH) << name << " loaded" << std::endl;
    else Log::Level(LOG_LEVEL::LOG_BOTH) << name << " was not loaded" << std::endl;

    return *out;
}

//---------------------------------------------------------------------------------------------------
//test

const char* optionsTest[] = {
    "test1",
    "test2"
};
void TestChanged(int oldVal, int newVal, void* data)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "TestChanged - changed to " << newVal << std::endl;
}

//---------------------------------------------------------------------------------------------------

extern "C" void OnModPreLoad()
{
    ModConfig::MakePaths();
    ModConfig::DefineVersions();
    
    char logPath[512];
	sprintf(logPath, "%s/policeBase/", aml->GetConfigPath());
    Log::Open(logPath, "policeBase");

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload()" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AML headers: 1.0.3.1" << std::endl;

    logger->SetTag("PoliceBase");

    logger->Info("Preload");

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload() END" << std::endl;
}

extern "C" void OnModLoad()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load()" << std::endl;

    cfg->Bind("Author", "", "About")->SetString("Danilo1301"); cfg->ClearLast();
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/mkCDRf4zJA"); cfg->ClearLast();
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/Danilo1301"); cfg->ClearLast();
    cfg->Save();

    //interfaces

    LoadInterface(&cleo, "CLEO");
    LoadInterface(&sautils, "SAUtils");
    LoadInterface(&menuVSL, "MenuVSL");
    LoadInterface(&multiRemap, "MultiRemap");

    Log::Level(LOG_LEVEL::LOG_BOTH) << "cleo: " << cleo << std::endl;

    if(sautils)
    {

    }

    //

    std::string cleoVersion = CheckModVersion(
        { "net.rusjj.cleolib", "net.rusjj.cleomod" },
        { "2.0.1", "2.0.1.1", "2.0.1.2", "2.0.1.3", "2.0.1.4", "2.0.1.5" }
    );

    std::string sautilsVersion = CheckModVersion(
        { "net.rusjj.gtasa.utils" },
        { "1.1", "1.2", "1.2.1", "1.3.0", "1.3.1", "1.4", "1.4.1", "1.5.1", "1.6"}
    );
    
    std::string amlVersion = CheckModVersion(
        { "net.rusjj.aml" },
        { "1.0.0.0", "1.0.0.1", "1.0.0.2", "1.0.0.3", "1.0.0.4", "1.0.0.5", "1.0.0.6", "1.0.1", "1.0.2", "1.0.2.1", "1.0.2.2", "1.0.3", "1.0.3.1", "1.1", "1.2", "1.2.1", "1.2.2"}
    );

    Log::Level(LOG_LEVEL::LOG_BOTH) << "----------------------------" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Police Base: v" << ModConfig::GetModVersion() << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "CLEO version: " << cleoVersion << " (recommended 2.0.1.3)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils version: " << sautilsVersion << " (recommended 1.3.1)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AML version: " << amlVersion << " (recommended 1.2.2)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "----------------------------" << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Finding symbols..." << std::endl;
    
    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t gameAddr = (uintptr_t)(cleo->GetMainLibraryLoadAddress());

    SET_TO(pVehiclePool, aml->GetSym(hGTASA, "_ZN6CPools15ms_pVehiclePoolE"));
    SET_TO(pPedPool, aml->GetSym(hGTASA, "_ZN6CPools11ms_pPedPoolE"));

    SET_TO(GetVehicleRef, aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));
    SET_TO(GetPedRef, aml->GetSym(hGTASA, "_ZN6CPools9GetPedRefEP4CPed"));
    SET_TO(GetPedFromRef, aml->GetSym(hGTASA, "_ZN6CPools6GetPedEi"));

    HOOKPLT(UpdateGameLogic, gameAddr + 0x66FE58);
    
    //

    ModConfig::DefineVersions();
    ModConfig::ProcessVersionChanges_PreConfigLoad();
    ModConfig::Load();
    ModConfig::ProcessVersionChanges_PostConfigLoad();
    ModConfig::Save();

    Mod::Init();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load() END" << std::endl;
}
