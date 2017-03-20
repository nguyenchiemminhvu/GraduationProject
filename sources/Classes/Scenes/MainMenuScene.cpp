#include "MainMenuScene.h"
#include "Toast.h"
#include "GameSettings.h"
#include "Definition.h"
#include "Scenes\CreditsScene.h"
#include "Scenes\LevelSelectionBoard.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "pluginfacebook\PluginFacebook.h"
#endif


cocos2d::Scene * MainMenu::createScene()
{
	auto scene = cocos2d::Scene::create();
	auto mainMenu = MainMenu::create();
	scene->addChild(mainMenu, (int)ZOrderLayer::LAYER_1);
	return scene;
}


bool MainMenu::init()
{
	if (!cocos2d::Layer::init())
		return false;

	origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	initBackground();
	initButtons();

	return true;
}


void MainMenu::initBackground()
{
	auto background = cocos2d::Sprite::create("images/UI/main_menu_ui/main_menu_background.png");
	background->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 2
		)
	);
	this->addChild(background);
}


void MainMenu::initButtons()
{
	// create button play
	auto buttonPlay = cocos2d::ui::Button::create("images/UI/main_menu_ui/play_button.png");
	buttonPlay->addTouchEventListener(CC_CALLBACK_2(MainMenu::onButtonPlayTouched, this));
	buttonPlay->setScaleY(BUTTON_SIZE_SCALE_Y);

	// create FAQ button
	auto buttonFAQ = cocos2d::ui::Button::create("images/UI/main_menu_ui/FAQ_button.png");
	buttonFAQ->addTouchEventListener(CC_CALLBACK_2(MainMenu::onButtonFAQTouched, this));
	buttonFAQ->setScaleY(BUTTON_SIZE_SCALE_Y);

	// create toggle button to setting sound
	auto soundEffectToggle = cocos2d::ui::CheckBox::create("images/UI/main_menu_ui/sound_off_button.png", "", "images/UI/main_menu_ui/sound_on_button.png", "", "");
	soundEffectToggle->setSelected(GameSettings::getInstance()->isSoundEffectEnabled());
	soundEffectToggle->addEventListener(CC_CALLBACK_2(MainMenu::onSoundEffectStateChanged, this));
	soundEffectToggle->setScaleY(BUTTON_SIZE_SCALE_Y);

	// create button share facebook
	auto buttonFShare = cocos2d::ui::Button::create("images/UI/main_menu_ui/fb_button.png");
	buttonFShare->addTouchEventListener(CC_CALLBACK_2(MainMenu::onButtonFacebookTouched, this));
	buttonFShare->setScaleY(BUTTON_SIZE_SCALE_Y);

	// add button play to main menu scene
	buttonPlay->setPosition(
		cocos2d::Vec2(
			origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 2
		)
	);
	this->addChild(buttonPlay);

	// create group button (button faq, sound setting, facebook share)
	auto menuToggle = cocos2d::ui::VBox::create();
	menuToggle->setAnchorPoint(cocos2d::Vec2(0.5F, 0.5F));
	menuToggle->addChild(buttonFAQ);
	menuToggle->addChild(cocos2d::ui::VBox::create(cocos2d::Size(0, buttonFShare->getContentSize().height / 2)));
	menuToggle->addChild(soundEffectToggle);
	menuToggle->addChild(cocos2d::ui::VBox::create(cocos2d::Size(0, buttonFShare->getContentSize().height / 2)));
	menuToggle->addChild(buttonFShare);

	// add group button at top left corner of main menu scene
	this->addChild(menuToggle);
	menuToggle->setPosition(
		cocos2d::Vec2(
			origin.x + buttonFShare->getContentSize().width / 4,
			origin.y + visibleSize.height - buttonFShare->getContentSize().height * 2
		)
	);
}


void MainMenu::replaceCreditsScene()
{
	auto credits = Credits::createScene();
	cocos2d::Director::getInstance()->replaceScene(credits);
}


void MainMenu::replaceLevelSelectionBoard()
{
	auto levelBoard = LevelSelectionBoard::createScene();
	cocos2d::Director::getInstance()->replaceScene(levelBoard);
}


void MainMenu::onButtonFAQTouched(cocos2d::Ref * ref, cocos2d::ui::Button::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		replaceCreditsScene();
		break;
	
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	
	default:
		break;
	}
}


void MainMenu::onSoundEffectStateChanged(cocos2d::Ref * ref, cocos2d::ui::CheckBox::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
		GameSettings::getInstance()->enableSoundEffect();
		break;

	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		GameSettings::getInstance()->disableSoundEffect();
		break;

	default:
		break;
	}
}


void MainMenu::onButtonFacebookTouched(cocos2d::Ref * ref, cocos2d::ui::Button::TouchEventType type)
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	sdkbox::FBShareInfo info;
#endif

	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:

		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:

		break;

	case cocos2d::ui::Widget::TouchEventType::ENDED:
		//////////////////////////////////////////////
		// share game on facebook
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		// request permission from user
		sdkbox::PluginFacebook::requestReadPermissions({ sdkbox::FB_PERM_READ_PUBLIC_PROFILE, sdkbox::FB_PERM_READ_USER_FRIENDS });
		sdkbox::PluginFacebook::requestPublishPermissions({ sdkbox::FB_PERM_PUBLISH_POST });

		// check if user isn't logged in yet
		if (!sdkbox::PluginFacebook::isLoggedIn())
		{
			sdkbox::PluginFacebook::login();
		}

		// share content
		info.type = sdkbox::FB_LINK;
		info.link = "http://www.cocos2d-x.org";
		info.title = "cocos2d-x";
		info.text = "Great 2D Game Engine";
		info.image = "http://cocos2d-x.org/images/logo.png";
		sdkbox::PluginFacebook::share(info);

		Toast::displayToast(this, "Share successfully!", 2.0F);
#else
		Toast::displayToast(this, "This feature is not implemented on this OS!", 2.0F);
#endif
		break;

	case cocos2d::ui::Widget::TouchEventType::CANCELED:

		break;

	default:
		break;
	}
}


void MainMenu::onButtonPlayTouched(cocos2d::Ref * ref, cocos2d::ui::Button::TouchEventType type)
{
	cocos2d::Scene *levelBoard = nullptr;

	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;

	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;

	case cocos2d::ui::Widget::TouchEventType::ENDED:
		//////////////////////////////////////////////
		// replace level selection board
		levelBoard = LevelSelectionBoard::createScene();
		cocos2d::Director::getInstance()->replaceScene(levelBoard);
		break;
	
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	
	default:
		break;
	}
}

