//
//  FlappyMove.h
//  HelloCocos
//
//  Created by 刘国兴 on 2020/6/18.
//

#ifndef FlappyMove_h
#define FlappyMove_h

#include <stdio.h>
#include "cocos2d.h"
//一些全局常量
const int BIRD_RADIUS = 15; //小鸟半径
const int PIPE_HEIGHT = 320;//半根管道长度
const int PIPE_WIDTH = 52; //管道宽度
const int PIPE_SPACE = 100; //上下管之间的缝隙
const int PIPE_INTERVAL = 170;//横向两根管子之间的间距,288/2+52/2
const int WAIT_DISTANCE = 380;//等待距离
extern int bestScore; 

enum GAME_STATUS  //游戏状态,准备，开始，结束
{
    GAME_READY,
    GAME_START,
    GAME_OVER
};

class FlappyMove : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
//    void initAfterPhysicWorldSetted();
    void update(float dt);
    void scrollLand(float dt);
    void setPhysicWorld(cocos2d::PhysicsWorld *world);
    virtual bool onContactBegin(const cocos2d::PhysicsContact& contact);
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void createPipes();   //创建管道
    int getRandomHeight();
    void gameStart();
    void gameOver();
    void gameRetart(Ref *sender);
    void gamePanelAppear();
//    int bestScore;
    // a selector callback
    //    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(FlappyMove);
//    static FlappyMove* createWithPhysicWorld();
//    bool initWithPhysicWorld();

    
private:
    cocos2d::Sprite *birdSprite; //小鸟
    cocos2d::RepeatForever *swingAction; //小鸟的晃动动画
    cocos2d::Label *scoreLabel; //计分
    cocos2d::Sprite *land1, *land2;  //地板
    cocos2d::Vector<cocos2d::Node*> pipes; //管道，用容器装起来
    cocos2d::PhysicsWorld *m_physicWorld; //游戏层物理世界
    cocos2d::MenuItemImage *restartItem;
    GAME_STATUS gameStatus; //游戏状态变量
    
    int score;   //游戏当前分数和最好分数
    int touchX;  //触摸点横坐标
    int count = 0;
};
#endif /* FlappyMove_h */
