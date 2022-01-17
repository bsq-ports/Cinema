#include "shared/VideoPlayer/VideoPlayer.hpp"
#include "UnityEngine/Transform.hpp"

#include <vlcpp/vlc.hpp>
#include <jni.h>
#include <vlc/vlc.h>
#include <vlc/libvlc_media_player.h>

DEFINE_TYPE(Cinema, VideoPlayer);

void Cinema::VideoPlayer::Play()
{
    playing = true;
}

void Cinema::VideoPlayer::Preload(std::string const &filename, bool _audio) {

}
