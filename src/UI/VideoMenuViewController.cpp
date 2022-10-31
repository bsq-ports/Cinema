#include "VideoMenuViewController.hpp"
#include "main.hpp"
#include "Sprites.hpp"

#include "questui/shared/ArrayUtil.hpp"

#include "questui_components/shared/reference_comp.hpp"
#include "questui_components/shared/components/layouts/VerticalLayoutGroup.hpp"
#include "questui_components/shared/components/layouts/HorizontalLayoutGroup.hpp"
#include "questui_components/shared/components/Text.hpp"
#include "questui_components/shared/components/Button.hpp"
#include "questui_components/shared/components/Image.hpp"
#include "questui_components/shared/components/Backgroundable.hpp"
#include "questui_components/shared/components/misc/Utility.hpp"

#include "UnityEngine/Rect.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Events/UnityAction.hpp"

using namespace QUC;
using namespace Cinema;
using namespace UnityEngine;
DEFINE_TYPE(Cinema, VideoMenuViewController);

static auto CreateButtonWithIcon(RenderContext& ctx, RenderContextChildData& data) {
    UnityEngine::UI::Button* templateButton = QuestUI::ArrayUtil::Last(UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::UI::Button*>(), [](UnityEngine::UI::Button* x) {
        return x->get_name() == "PracticeButton";
    });

    auto& button = data.getData<UnityEngine::UI::Button*>();
    if (!button) {

        if(templateButton) {
            auto buttonGameObject = GameObject::Instantiate(templateButton, &ctx.parentTransform, false)->get_gameObject();
            auto buttonTransform = buttonGameObject->get_transform();
            buttonGameObject->set_name("DeleteButton");
            static ConstString contentName("Content");
            static ConstString textName("Text");
            auto contentTransform = buttonTransform->Find(contentName);
            Object::Destroy(contentTransform->Find(textName)->get_gameObject());
            Object::Destroy(contentTransform->GetComponent<UI::LayoutElement*>());
            static ConstString underlineName("Underline");
            Object::Destroy(buttonTransform->Find(underlineName)->get_gameObject());

            static ConstString iconName("Icon");
            auto iconGameObject = GameObject::New_ctor(iconName);
            auto imageView = iconGameObject->AddComponent<HMUI::ImageView*>();
            auto iconTransform = imageView->get_rectTransform();
            iconTransform->SetParent(contentTransform, false);
            imageView->set_material(Resources::FindObjectsOfTypeAll<Material*>().First([] (Material* x) { return x->get_name() == u"UINoGlow"; }));
            imageView->set_sprite(QuestUI::BeatSaberUI::Base64ToSprite(Sprites::DeleteLevelButtonIcon));
            imageView->set_preserveAspect(true);

            float scale = 1.7f;
            iconTransform->set_localScale(UnityEngine::Vector3(scale, scale, scale));

            UI::ContentSizeFitter* contentSizeFitter = buttonGameObject->AddComponent<UI::ContentSizeFitter*>();
            contentSizeFitter->set_verticalFit(UI::ContentSizeFitter::FitMode::Unconstrained);
            contentSizeFitter->set_horizontalFit(UI::ContentSizeFitter::FitMode::Unconstrained);

            buttonGameObject->GetComponent<UI::LayoutElement*>()->set_preferredWidth(10.0f);

            buttonTransform->SetAsFirstSibling();

            buttonGameObject->GetComponent<UI::Button*>()->set_interactable(true);
            buttonGameObject->GetComponent<UI::Button*>()->set_onClick(UI::Button::ButtonClickedEvent::New_ctor());

            //std::function<void()> function = []() {
                //DELETE VIDEO
            //};

            //buttonGameObject->GetComponent<UI::Button*>()->get_onClick()->AddListener(il2cpp_utils::MakeDelegate<Events::UnityAction*>(classof(Events::UnityAction*), function));
            button = buttonGameObject->GetComponent<UI::Button*>();
        }

    }


    return button->get_transform();
}

static auto ButtonWithIcon() {
    FunctionalComponent([](RenderContext& ctx, RenderContextChildData& data){
        return CreateButtonWithIcon(ctx, data);
    });

    return FunctionalComponent(CreateButtonWithIcon);
}

void VideoMenuViewController::DidActivate(bool firstActivation) {
    static RenderContext ctx(nullptr);

    static detail::HorizontalLayoutGroup topLine(
            Text("VIDEO TITLE"),
            Button("DELETE", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                //DELETE VIDEO
            })
    );

    static ModifyLayoutElement topLineElement(detail::refComp(topLine));
    topLineElement.preferredWidth = 100;
    static ModifyContentSizeFitter topLineFitter(detail::refComp(topLine));
    topLineFitter.horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
    topLine.childAlignment = UnityEngine::TextAnchor::MiddleCenter;


    static detail::HorizontalLayoutGroup videoDetails(
            Image(QuestUI::BeatSaberUI::Base64ToSprite(Cinema::Sprites::ImagePreviewPlaceholder),UnityEngine::Vector2(40,25)),
            VerticalLayoutGroup(
                    Text("VIDEO AUTHOR", true, std::nullopt, 3),
                    Text("VIDEO DURATION", true, std::nullopt, 3),
                    Text("VIDEO STATUS", true, std::nullopt, 3),
                    Button("DOWNLOAD", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                        //DOWNLOAD VIDEO
                    })
            )
    );

    static ModifyLayoutElement videoDetailsElement(detail::refComp(videoDetails));
    videoDetailsElement.preferredWidth = 50;
    videoDetailsElement.preferredHeight = 25;
    static ModifyContentSizeFitter videoDetailsFitter(detail::refComp(videoDetails));
    videoDetailsFitter.verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
    videoDetailsFitter.horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
    videoDetails.childForceExpandHeight = false;
    videoDetails.childForceExpandWidth = false;

    static detail::VerticalLayoutGroup displayValue(
            Text("OFFSET VALUE", true, std::nullopt, 5)
    );

    static ModifyLayoutElement displayValueElement(detail::refComp(displayValue));
    displayValueElement.preferredWidth = 16;

    static detail::HorizontalLayoutGroup offsetSettings(
                    Button("---", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                //Subtract 100 ms
            }),
                    Button("--", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                //Subtract 10 ms
            }),
                    Button("-", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                //Subtract 1 ms
            }),
            displayValue,
                    Button("+", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                //Add 1 ms
            }),
                    Button("++", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                //Add 10 ms
            }),
                    Button("+++", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                //Add 100 ms
            })
    );

    static ModifyContentSizeFitter offsetSettingsFitter(detail::refComp(offsetSettings));
    offsetSettingsFitter.horizontalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;
    offsetSettings.childAlignment = UnityEngine::TextAnchor::MiddleCenter;
    offsetSettings.spacing = 1;

    static detail::VerticalLayoutGroup bottomLine(
            VerticalLayoutGroup(
                    Text("Video Offset", true, std::nullopt, 3),
                    offsetSettings,
                    Button("Preview", [](Button &button, UnityEngine::Transform *, RenderContext &ctx)mutable {
                        //Preview
                    })
            )
    );

    bottomLine.padding = std::array<float, 4>{0,0,2,0};

    static detail::VerticalLayoutGroup screen(
            topLine,
            videoDetails,
            bottomLine
    );

    static detail::BackgroundableContainer rootContainer("round-rect-panel",
            screen
    );

    if(firstActivation) {
        //Master View

        screen.childForceExpandHeight = false;
        screen.spacing = 0;
        screen.padding = std::array<float, 4>{0,0,-3,0};
        screen.childAlignment = UnityEngine::TextAnchor::UpperCenter;
        static ModifyContentSizeFitter screenFitter(detail::refComp(screen));
        screenFitter.verticalFit = UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize;

        ctx = RenderContext(this->get_transform());
    }

    detail::renderSingle(rootContainer, ctx);
}