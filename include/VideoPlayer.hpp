#pragma once
#include "main.hpp"
#include "UnityEngine/Video/VideoPlayer.hpp"
#include "UnityEngine/Video/VideoRenderMode.hpp"
#include "UnityEngine/Video/VideoAudioOutputMode.hpp"
#include "UnityEngine/Video/VideoAspectRatio.hpp"
#include "UnityEngine/Renderer.hpp"

using namespace UnityEngine;

namespace Cinema {
    class VideoPlayer : public UnityEngine::Video::VideoPlayer {

    public:

        void set_time(double time) {
            static auto setTime = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*, double>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_time"));
            setTime(this, time);
        }

        void set_aspectRatio(Video::VideoAspectRatio ratio) {
            static auto aspectRatio = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*, Video::VideoAspectRatio>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_aspectRatio"));
            aspectRatio(this, ratio);
        }

        bool get_isPrepared() {
            static auto isPrepared = reinterpret_cast<function_ptr_t<bool, Video::VideoPlayer*>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::get_isPrepared"));
            return isPrepared(this);
        }

        void Pause() {
            static auto pauseVideo = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::Pause"));
            pauseVideo(this);
        }

        void Prepare() {
            static auto prepareVideo = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::Prepare"));
            prepareVideo(this);
        }

        void set_audioOutputMode(Video::VideoAudioOutputMode mode) {
            static auto audioOutputMode = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*, Video::VideoAudioOutputMode>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_audioOutputMode"));
            audioOutputMode(this, mode);
        }

        void set_playOnAwake(bool value) {
            static auto playOnAwake = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*, bool>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_playOnAwake"));
            playOnAwake(this, value);
        }

        void set_isLooping(bool value) {
            static auto isLooping = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*, bool>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_isLooping"));
            isLooping(this, value);

        }

        void set_renderMode(Video::VideoRenderMode renderMode) {
            static auto setRenderMode = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*, Video::VideoRenderMode>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_renderMode"));
            setRenderMode(this, renderMode);
        }

        void set_renderer(Renderer* renderer) {
            static auto set_targetRenderer = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*, Renderer*>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_targetMaterialRenderer"));
            set_targetRenderer(this, renderer);
        }

        void set_url(StringW url) {
            static auto setUrl = reinterpret_cast<function_ptr_t<void, Video::VideoPlayer*, StringW>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_url"));
            setUrl(this, url);
        }
    };
}
DEFINE_IL2CPP_ARG_TYPE(Cinema::VideoPlayer*, "UnityEngine.Video", "VideoPlayer");