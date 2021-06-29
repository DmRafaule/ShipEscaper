#pragma once 

#include "cocos2d.h"

class Splash : public cocos2d::Scene{
public:
   virtual bool init();
   CREATE_FUNC(Splash);
private:
   void GoToMyLogo(float dt);
   void GoToCocosLogo(float dt);
   void GoToGame(float dt);
private:
   cocos2d::Sprite* MyLogo;
   cocos2d::Sprite* CocosLogo;
   cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
};