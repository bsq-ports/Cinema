#include "main.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Component.hpp"
#include "VideoPlayer/VideoPlayer.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

MAKE_HOOK_MATCH(MainMenu, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    MainMenu(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    getLogger().info("yes");
	GameObject* Mesh = GameObject::CreatePrimitive(PrimitiveType::Plane);
	getLogger().info("yes 2");
	Mesh->GetComponent<Renderer*>()->set_material(Material::New_ctor(Shader::Find(il2cpp_utils::createcsstr("Unlit/Texture"))));
	getLogger().info("yes 3");
	Mesh->get_transform()->set_position(UnityEngine::Vector3{0.0f, 1.0f, 0.0f});
	getLogger().info("yes 4");
	Mesh->get_transform()->set_rotation(Quaternion::Euler(90.0f, 180.0f, 90.0f));
	getLogger().info("yes 5");

    Cinema::VideoPlayer* n = Mesh->AddComponent<Cinema::VideoPlayer*>();
    n->Preload("/sdcard/video.mp4", false);
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    INSTALL_HOOK(getLogger(), MainMenu);

	custom_types::Register::AutoRegister();
}