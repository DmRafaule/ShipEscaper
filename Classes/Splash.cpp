#include "Splash.h"
#include "AudioEngine.h"
#include "Game.h"

bool Splash::init(){
   
   cocos2d::AudioEngine::play2d("music/bg_music.mp3",true);

   this->scheduleOnce(CC_SCHEDULE_SELECTOR(Splash::GoToMyLogo),0.f);
   this->scheduleOnce(CC_SCHEDULE_SELECTOR(Splash::GoToCocosLogo),4.f);
   this->scheduleOnce(CC_SCHEDULE_SELECTOR(Splash::GoToGame),10.f);

   return true;
}
void Splash::GoToMyLogo(float dt){
   cocos2d::Action* fadeout = cocos2d::FadeOut::create(2);
   MyLogo = cocos2d::Sprite::create("textures/MyLogo.jpg");
   MyLogo->setScale(visibleSize.height/MyLogo->getContentSize().height * 0.4);
   MyLogo->setPosition(cocos2d::Vec2(visibleSize.width/2,visibleSize.height/2));
   MyLogo->runAction(fadeout);
   this->addChild(MyLogo);
}
void Splash::GoToCocosLogo(float dt){
   cocos2d::FadeIn* fadein = cocos2d::FadeIn::create(2);
   cocos2d::FadeOut* fadeout = cocos2d::FadeOut::create(2);
   CocosLogo = cocos2d::Sprite::create("textures/CocosLogo.png");
   CocosLogo->setScale(visibleSize.height/CocosLogo->getContentSize().height * 0.4);
   CocosLogo->setPosition(cocos2d::Vec2(visibleSize.width/2,visibleSize.height/2));
   CocosLogo->setOpacity(0);
   CocosLogo->runAction(cocos2d::Sequence::create(fadein,1.f,fadeout,1.f,nullptr));
   this->addChild(CocosLogo);
}
void Splash::GoToGame(float dt){
   auto game = Game::createScene();
   _director->getInstance()->replaceScene(game);
}