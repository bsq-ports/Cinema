#pragma once

#include "UnityEngine/GameObject.hpp"

#include "HMUI/ViewController.hpp"

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Cinema, VideoMenuViewController, UnityEngine::MonoBehaviour,
      DECLARE_INSTANCE_METHOD(void, DidActivate, bool firstActivation);
);