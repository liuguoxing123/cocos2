//
//  FlappyBird.hpp
//  HelloCocos
//
//  Created by 刘国兴 on 2020/6/18.
//

#ifndef FlappyBird_hpp
#define FlappyBird_hpp

#include <stdio.h>
#include "cocos2d.h"
#include "FlappyMove.h"
class FlappyBird : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    // a selector callback
//    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuStartCallback(cocos2d::Ref* pSender);
    void menuScoreCallback(cocos2d::Ref* pSender);
    void menuReturnCallback(cocos2d::Ref* pSender);
    
    
    // implement the "static create()" method manually
    CREATE_FUNC(FlappyBird);
private:
    cocos2d::Sprite *sprite, *spritebg;
    cocos2d::MenuItemImage *startItem, *scoreItem, *returnItem;
    cocos2d::Size visibleSize;
    
};



#endif /* FlappyBird_hpp */
