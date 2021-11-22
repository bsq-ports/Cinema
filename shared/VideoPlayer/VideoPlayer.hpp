#include "main.hpp"
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(Cinema, VideoPlayer, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(bool, audio);
    DECLARE_INSTANCE_FIELD(bool, playing);

    DECLARE_INSTANCE_METHOD(void, Play);

public:
    void Preload(std::string const& filename, bool _audio);
)