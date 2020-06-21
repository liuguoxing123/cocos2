//
//  FlappyMove.cpp
//  HelloCocos
//
//  Created by 刘国兴 on 2020/6/18.
//

#include "FlappyMove.h"
#include "SimpleAudioEngine.h"
#include "FlappyBird.hpp"
USING_NS_CC;
using namespace CocosDenshion;
int bestScore;

Scene* FlappyMove::createScene()
{
    auto flappyMove = Scene::createWithPhysics(); //创建一个包含有PhysicsWorld的Scene对象
    //设置Debug模式，你会看到物体的表面被线条包围，主要为了在调试中更容易地观察
//    flappyMove->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    
    flappyMove->getPhysicsWorld()->setGravity(Vec2(0, -98*4)); //设置重力场,重力加速度可以根据手感改小点
    
    //添加主游戏层
    auto gameLayer = FlappyMove::create();
    gameLayer->setPhysicWorld(flappyMove->getPhysicsWorld()); //绑定物理世界
    flappyMove->addChild(gameLayer);
    return flappyMove;
//    return gameLayer;
//    return FlappyMove::create();
}

bool FlappyMove::init() {
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
//
//    setPhysicWorld(getPhysicsWorld());

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    
    //初始化游戏状态
    gameStatus = GAME_READY;
    score = 0;
    
    // add "FlappyMove" splash screen"
    auto spritebg = Sprite::create("bg_day.png");
    spritebg->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    this->addChild(spritebg, 0);
    
    auto spritebg2 = Sprite::create("land.png");
    spritebg2->setPosition(Vec2(visibleSize.width/2, visibleSize.height/9));
    this->addChild(spritebg2, 1);
    
    auto spritebg3 = Sprite::create("text_ready.png");
    spritebg3->setPosition(Vec2(visibleSize.width/2, visibleSize.height * 2/3));
    spritebg3->setName("ready");
    this->addChild(spritebg3, 1);
    
    //
    // Manual animation
    //
    birdSprite = Sprite::create("bird0_0.png");
    birdSprite->setPosition(Vec2(visibleSize.width/2 , visibleSize.height/2 ));
    
    // add the sprite as a child to this layer
    this->addChild(birdSprite);
    auto animation = Animation::create();
    for( int i=0;i<3;i++)
    {
        char szName[100] = {0};
        sprintf(szName, "bird0_%d.png", i);
        animation->addSpriteFrameWithFile(szName);
    }
    // should last 2.8 seconds. And there are 14 frames.
    animation->setDelayPerUnit(1.8f / 14.0f);
    animation->setRestoreOriginalFrame(true);
    
    auto action = Animate::create(animation);
    //    auto seq = Sequence::create(action, action->reverse(), nullptr);
    //    swingAction = RepeatForever::create(seq);    //翅膀上下摆动
    
    if (gameStatus == GAME_READY)
    {
        swingAction = RepeatForever::create(Sequence::create(action, action->reverse(), nullptr));
        birdSprite->runAction(swingAction); //上下晃动动画
    }
    
    //小鸟绑定刚体
    auto birdBody = PhysicsBody::createCircle(BIRD_RADIUS); //将小鸟当成一个圆
    birdBody->setDynamic(true);   //设置为可以被物理场所作用而动作
    birdBody->setContactTestBitmask(1); //必须设置这项为1才能检测到不同的物体碰撞
    birdBody->setGravityEnable(false);   //设置是否被重力影响,准备画面中不受重力影响
    birdSprite->setPhysicsBody(birdBody); //为小鸟设置刚体
    
    //添加两个land
    land1 = Sprite::create("land.png");
    land1->setAnchorPoint(Point::ZERO);
    land1->setPosition(Point::ZERO);
    this->addChild(land1, 10);  //置于最顶层
    land2 = Sprite::create("land.png");
    land2->setAnchorPoint(Point::ZERO);
    land2->setPosition(Point::ZERO);
    this->addChild(land2, 10);
    
    
    //设置地板刚体
    Node *groundNode = Node::create();
    auto groundBody = PhysicsBody::createBox(Size(visibleSize.width, land1->getContentSize().height));
    groundBody->setDynamic(false);
    groundBody->setContactTestBitmask(1);
    groundNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE); //物理引擎中的刚体只允许结点锚点设置为中心
    groundNode->setPhysicsBody(groundBody);
    groundNode->setPosition(origin.x+visibleSize.width/2,land1->getContentSize().height/2);
    this->addChild(groundNode);
    
    //添加碰撞监测
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(FlappyMove::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    //添加管子
    createPipes();
    
    //添加触摸监听
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(FlappyMove::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(FlappyMove::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    //记分牌,在最外层
    scoreLabel = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 30);
    scoreLabel->setPosition(origin.x+visibleSize.width / 2, origin.x+visibleSize.height / 2 + 200);
    scoreLabel->setVisible(false); //一开始隐藏
    this->addChild(scoreLabel);
    
    return true;
    
}


//设置层的物理世界
void FlappyMove::setPhysicWorld(PhysicsWorld *world)
{
    m_physicWorld = world;
}
//游戏开始
void FlappyMove::gameStart()
{
    gameStatus = GAME_START;
    score = 0;//重置分数
    scoreLabel->setString(std::to_string(score));
    this->getChildByName("ready")->setVisible(false); //logo消失
    scoreLabel->setVisible(true); //计分开始
    this->scheduleUpdate();//启动默认更新
    this->schedule(schedule_selector(FlappyMove::scrollLand), 0.01f); //启动管子和地板滚动
//    birdSprite->stopAction(swingAction); //游戏开始后停止上下浮动
    birdSprite->getPhysicsBody()->setGravityEnable(true); //开始受重力作用
}
//游戏结束
void FlappyMove::gameOver()
{
    gameStatus = GAME_OVER;
    //获取历史数据
    bestScore = UserDefault::getInstance()->getIntegerForKey("BEST");
    if (score > bestScore)
    {
        bestScore = score;  //更新最好分数
        UserDefault::getInstance()->setIntegerForKey("BEST", bestScore);
    }
    
    
//    SimpleAudioEngine::getInstance()->playEffect("hit.mp3");
    //游戏结束后停止地板和管道的滚动
    this->unschedule(schedule_selector(FlappyMove::scrollLand));
    gamePanelAppear();  //弹出记分牌
    
}

//触摸监听
bool FlappyMove::onTouchBegan(Touch *touch, Event *event)
{
    CCLOG("onToucheBegan");
    switch (gameStatus)
    {
        case GAME_OVER:
            CCLOG("onToucheBegan1");
            break;
        case GAME_READY:
        {
            CCLOG("onToucheBegan2");
            gameStart();
            birdSprite->getPhysicsBody()->setVelocity(Vec2(0, 180)); //给一个向上的初速度
//            SimpleAudioEngine::getInstance()->playEffect("wing.mp3");
            //这里也要写，不然touchx的值是未知值，负无穷，导致bug
//            touchX = touch->getLocation().x;
        }
            break;
        case GAME_START:
        {
            CCLOG("onToucheBegan3");
            auto curVelocity = birdSprite->getPhysicsBody()->getVelocity();
//
            birdSprite->getPhysicsBody()->setVelocity(Vec2(0, 180>(curVelocity.y + 180) ? (curVelocity.y + 180):180));
//            birdSprite->getPhysicsBody()->setVelocity(Vec2(0, 50)); //向上的速度受下降影响
//            SimpleAudioEngine::getInstance()->playEffect("wing.mp3");
//            auto actionBy = MoveBy::create(0.5, Vec2(0, 50));
//            birdSprite->runAction(actionBy);
//            touchX = touch->getLocation().x;
        }
            break;
        default:
            CCLOG("onToucheBegan4");
            break;
    }
    
    return true;
}
void FlappyMove::onTouchEnded(Touch *touch, Event *event)
{
    //当触摸点滑动超过100，分数瞬间涨100
//    if (touch->getLocation().x - touchX > 100)
//    {
//        score += 100;
//        scoreLabel->setString(String::createWithFormat("%d", score)->getCString());
////        SimpleAudioEngine::getInstance()->playEffect("point.mp3");
//    }

}
//地板滚动自定义计时器回调
void FlappyMove::scrollLand(float dt)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    //两个图片循环移动
    land1->setPositionX(land1->getPositionX() - 1.0f);
    land2->setPositionX(land1->getPositionX() + land1->getContentSize().width - 2.0f);
    if (land2->getPositionX() <= 0)
        land1->setPosition(Point::ZERO);
    
    //管子滚动
    for (auto &singlePipe : pipes)
    {
        singlePipe->setPositionX(singlePipe->getPositionX() - 1.0f);
        if (singlePipe->getPositionX() < -PIPE_WIDTH/2)
        {
            singlePipe->setPositionX(visibleSize.width);
            singlePipe->setPositionY(getRandomHeight());
            singlePipe->setName("newPipe");  //每次重设一根管子，标为new
        }
    }
}


//获取随机高度，用于管道
int FlappyMove::getRandomHeight()
{
    auto size = Director::getInstance()->getVisibleSize();
    return size.height / 2 - 40 - CCRANDOM_0_1() * (270 - 40);
}
//创建管道
void FlappyMove::createPipes()
{
    //同屏幕出现的只有两根管子，放到容器里面，上下绑定为一根
    for (int i = 0; i < 2; i++)
    {
//        auto visibleSize = Director::getInstance()->getVisibleSize();
        Sprite *pipeUp = Sprite::create("pipe_down.png");
        pipeUp->setPosition(0, PIPE_HEIGHT + PIPE_SPACE);
//        pipeUp->setContentSize(Size(1, PIPE_HEIGHT));
        Sprite *pipeDown = Sprite::create("pipe_up.png");
        pipeDown->setPosition(0, 0);
//        pipeDown->setContentSize(Size(1, PIPE_HEIGHT));
        Node *singlePipe = Node::create();
        //给上管绑定刚体
        auto pipeUpBody = PhysicsBody::createBox(pipeUp->getContentSize());
        pipeUpBody->setDynamic(false);
        pipeUpBody->setContactTestBitmask(1);
        pipeUp->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        pipeUp->setPhysicsBody(pipeUpBody);
//        给两个管子分开设置刚体，可以留出中间的空隙使得小鸟通过
//        给下管绑定刚体
        auto pipeDownBody = PhysicsBody::createBox(pipeDown->getContentSize());
        pipeDownBody->setDynamic(false);
        pipeDownBody->setContactTestBitmask(1);
        pipeDown->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        pipeDown->setPhysicsBody(pipeDownBody);
//
        
//        pipeUp->setPosition(WAIT_DISTANCE, 0);
        singlePipe->addChild(pipeUp);
        singlePipe->addChild(pipeDown);  //pipeDown默认加到（0,0），上下合并，此时singlePipe以下面的管子中心为锚点
//        singlePipe->setIgnoreAnchorPointForPosition(false);
        singlePipe->setPosition(i*PIPE_INTERVAL + WAIT_DISTANCE, getRandomHeight() ); //设置初始高度
//        singlePipe->setContentSize(Size(1, PIPE_HEIGHT));
        singlePipe->setName("newPipe");
        this->addChild(singlePipe);  //把两个管子都加入到层
        pipes.pushBack(singlePipe);  //两个管子先后添加到容器
    }
    
}
//默认的更新函数
void FlappyMove::update(float dt)
{
    //当游戏开始时，判断得分，这个其实也可以写在其他地方，比如管子滚动的更新函数里面或者触摸监测里面
    if (gameStatus == GAME_START)
    {
        for (auto &pipe : pipes)
        {
            if (pipe->getName() == "newPipe") //新来一根管子就判断
            {
                if (pipe->getPositionX() < birdSprite->getPositionX())
                {
                    score++;
                    scoreLabel->setString(std::to_string(score));
//                    scoreLabel->setString(String::createWithFormat("%d", score)->getCString());
//                    SimpleAudioEngine::getInstance()->playEffect("point.mp3");
                    pipe->setName("passed"); //标记已经过掉的管子
                }
            }
        }
    }
    //小鸟的旋转
    auto curVelocity = birdSprite->getPhysicsBody()->getVelocity();
    birdSprite->setRotation(-curVelocity.y*0.1 - 20);  //根据竖直方向的速度算出旋转角度，逆时针为负
    
    if (birdSprite->getPositionY() <= land1->getContentSize().height+BIRD_RADIUS)
    {
        birdSprite->stopAllActions(); //小鸟挂了就不能再扇翅了
        birdSprite->setRotation(70);  //设置为嘴朝下，顺时针70度旋转
        birdSprite->getPhysicsBody()->setDynamic(false);  //设置为不动了
//        SimpleAudioEngine::getInstance()->playEffect("die.mp3");
        this->unscheduleUpdate();  //在小鸟掉到地面上再停止默认更新
//        gamePanelAppear();  //弹出记分牌
    }
    
}
//碰撞监测
bool FlappyMove::onContactBegin(const PhysicsContact& contact)
{
    if (count <= 1) {
        count++;
        return true;
    }
    
    else{
        CCLOG("on1");
        if (gameStatus == GAME_OVER)  //当游戏结束后不再监控碰撞
            return false;
        
        gameOver();
        
        return true;
    }
    
}
////加入记分板和重玩菜单
void FlappyMove::gamePanelAppear()
{
    Size size = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    //用node将gameoverlogo和记分板绑在一起
    Node *gameOverPanelNode = Node::create();
    auto gameOverLabel = Sprite::create("text_game_over.png");
    gameOverPanelNode->addChild(gameOverLabel);
    auto panel = Sprite::create("score_panel.png");//注意这里是大写PNG，原图片用什么后缀这里就用什么，区分大小写
    gameOverLabel->setPositionY(panel->getContentSize().height); //设置一下坐标
    gameOverPanelNode->addChild(panel);
    //记分板上添加两个分数
//    createWithTTF("Multi touch the screen", "fonts/Marker Felt.ttf", 36.0f);
    auto curScoreTTF = Label::createWithTTF(std::to_string(score), "fonts/Marker Felt.ttf", 20);
    curScoreTTF->setPosition(panel->getContentSize().width-40, panel->getContentSize().height-45);
    curScoreTTF->setColor(Color3B(255, 0, 0));
    panel->addChild(curScoreTTF);
    auto bestScoreTTF = Label::createWithTTF(std::to_string(bestScore), "fonts/Marker Felt.ttf", 20);
    bestScoreTTF->setPosition(panel->getContentSize().width - 40, panel->getContentSize().height - 90);
    bestScoreTTF->setColor(Color3B(0, 255, 0));
    panel->addChild(bestScoreTTF);
    this->addChild(gameOverPanelNode);
    gameOverPanelNode->setPosition(origin.x + size.width / 2, origin.y + size.height );
    //滑入动画
    gameOverPanelNode->runAction(MoveTo::create(0.5f, Vec2(origin.x + size.width / 2, origin.y + size.height / 2)));
//    SimpleAudioEngine::getInstance()->playEffect("swooshing.mp3");
    //添加菜单
//    restartItem = MenuItemImage::create("button_play.png", "button_play.png", this,menu_selector(FlappyMove::gameRetart));
    restartItem = MenuItemImage::create("button_play.png", "button_play.png", CC_CALLBACK_1(FlappyMove::gameRetart, this));
    
    auto menu = Menu::createWithItem(restartItem);
    menu->setPosition(origin.x + size.width / 2, 150);
    this->addChild(menu);
}
//游戏重新开始
void FlappyMove::gameRetart(Ref *sender)
{
    //重新回到初始画面
    auto FlappyMove = FlappyMove::createScene();
    Director::getInstance()->replaceScene(FlappyMove);
}
