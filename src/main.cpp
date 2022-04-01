#include "main.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "GlobalNamespace/GamePause.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Component.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Video/VideoPlayer.hpp"
#include "UnityEngine/Video/VideoClip.hpp"
#include "UnityEngine/Video/VideoRenderMode.hpp"
#include "UnityEngine/Video/VideoPlayer_EventHandler.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UI/VideoMenuViewController.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "VideoPlayer.hpp"
#include "custom-types/shared/coroutine.hpp"

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

custom_types::Helpers::Coroutine coroutine(Cinema::VideoPlayer* videoPlayer, AudioSource* audioSource) {
    while(!audioSource->get_isPlaying()) co_yield nullptr;
    videoPlayer->set_time(-2040);
    videoPlayer->Play();
    co_return;
}

Cinema::VideoPlayer* videoPlayer = nullptr;

MAKE_HOOK_MATCH(GamePause_Resume, &GlobalNamespace::GamePause::Resume, void, GamePause* self) {
    GamePause_Resume(self);
    if(videoPlayer)
        videoPlayer->Play();
    getLogger().info("resume");
}

MAKE_HOOK_MATCH(GamePause_Pause, &GamePause::Pause, void, GamePause* self) {
    GamePause_Pause(self);
    if(videoPlayer) {
        videoPlayer->Pause();
        getLogger().info("pause");
    }

}

MAKE_HOOK_MATCH(SetupSongUI, &GlobalNamespace::AudioTimeSyncController::StartSong, void, GlobalNamespace::AudioTimeSyncController* self, float startTimeOffset) {
    SetupSongUI(self, startTimeOffset);

    GameObject* Mesh = GameObject::CreatePrimitive(PrimitiveType::Plane);
    auto material = QuestUI::ArrayUtil::Last(Resources::FindObjectsOfTypeAll<Material*>(), [](Material* x) {
        return x->get_name() == "PyroVideo (Instance)";
    });
    if(material)
        Mesh->GetComponent<Renderer*>()->set_material(material);
    else
        Mesh->GetComponent<Renderer*>()->set_material(Material::New_ctor(Shader::Find(il2cpp_utils::newcsstr("Unlit/Texture"))));
    Mesh->get_transform()->set_position(Vector3{0.0f, 12.4f, 67.8f});
    Mesh->get_transform()->set_rotation(Quaternion::Euler(90.0f, 270.0f, 90.0f));
    Mesh->get_transform()->set_localScale(Vector3(5.11, 1, 3));

    auto cinemaScreen = Mesh->GetComponent<Renderer*>();

    videoPlayer = Mesh->AddComponent<Cinema::VideoPlayer*>();
    videoPlayer->set_isLooping(true);
    videoPlayer->set_playOnAwake(false);
    videoPlayer->set_renderMode(Video::VideoRenderMode::MaterialOverride);
    videoPlayer->set_audioOutputMode(Video::VideoAudioOutputMode::None);
    videoPlayer->set_aspectRatio(Video::VideoAspectRatio::FitInside);
    if(cinemaScreen)
        videoPlayer->set_renderer(cinemaScreen);
    videoPlayer->set_url("/sdcard/prince.mp4");

    videoPlayer->Prepare();

    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(coroutine(videoPlayer, self->dyn__audioSource())));
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    //INSTALL_HOOK(getLogger(), MainMenu);
    INSTALL_HOOK(getLogger(), SetupSongUI);
    INSTALL_HOOK(getLogger(), GamePause_Resume);
    INSTALL_HOOK(getLogger(), GamePause_Pause);

    QuestUI::Register::RegisterGameplaySetupMenu<Cinema::VideoMenuViewController*>(modInfo, "Cinema", QuestUI::Register::MenuType::Solo);

	custom_types::Register::AutoRegister();
}