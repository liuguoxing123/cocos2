//
//  FlappyBird.cpp
//  HelloCocos
//
//  Created by 刘国兴 on 2020/6/18.
//

#include "FlappyBird.hpp"
#include "SimpleAudioEngine.h"
#include "FlappyMove.h"

USING_NS_CC;

Scene* FlappyBird::createScene()
{
    return FlappyBird::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in FlappyBirdScene.cpp\n");
}

// on "init" you need to initialize your instance
bool FlappyBird::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    startItem =MenuItemImage::create(
                                          "button_play.png",
                                          "button_play.png",
                                          CC_CALLBACK_1(FlappyBird::menuStartCallback, this));
    scoreItem =MenuItemImage::create(
                                          "button_score.png",
                                          "button_score.png",
                                          CC_CALLBACK_1(FlappyBird::menuScoreCallback, this));
    startItem->setPosition(Vec2(visibleSize.width/4, visibleSize.height/3));
    scoreItem->setPosition(Vec2(visibleSize.width * 3/4, visibleSize.height/3));
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(startItem, scoreItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    /////////////////////////////
    // 3. add your codes below...
    
    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Welcome To Flappy", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));
        
        // add the label as a child to this layer
        this->addChild(label, 1);
    }
    
    // add "FlappyBird" splash screen"
    spritebg = Sprite::create("bg_day.png");
    spritebg->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    this->addChild(spritebg, 0);
    
    sprite = Sprite::create("bird0_0.png");
    if (sprite == nullptr)
    {
        problemLoading("'bird0_0.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y + 100));
        
        // add the sprite as a child to this layer
        this->addChild(sprite, 1);
    }
    
    return true;
}


void FlappyBird::menuStartCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(FlappyMove::createScene());
}
void FlappyBird::menuScoreCallback(Ref* pSender)
{
    startItem->setVisible(false);
    scoreItem->setVisible(false);
    auto panel = Sprite::create("score_panel.png");
    panel->setPosition(visibleSize.width / 2 , visibleSize.height / 3);
    
    //传FlappyMove文件里的变量bestScore,
    bestScore = UserDefault::getInstance()->getIntegerForKey("BEST");
    
    auto bestScoreTTF = Label::createWithTTF(std::to_string(bestScore), "fonts/Marker Felt.ttf", 20);
    bestScoreTTF->setPosition(panel->getContentSize().width - 40, panel->getContentSize().height - 90);
    bestScoreTTF->setColor(Color3B(0, 255, 0));
    panel->addChild(bestScoreTTF);
    
    returnItem =MenuItemImage::create(
                                     "button_ok.png",
                                     "button_ok.png",
                                     CC_CALLBACK_1(FlappyBird::menuReturnCallback, this));
    returnItem->setPosition(visibleSize.width * 3 / 4 , visibleSize.height / 7);
    auto menu = Menu::create(returnItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    this->addChild(panel);
//    Director::getInstance()->end();
}
void FlappyBird::menuReturnCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(FlappyBird::createScene());
}
