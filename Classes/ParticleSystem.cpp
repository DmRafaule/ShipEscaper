#include "ParticleSystem.h"
#include "Game.h"

#include <time.h>

float ParticleSystem::radius = 50;
std::vector<cocos2d::DrawNode*> ParticleSystem::engineEffect(28);
std::vector<cocos2d::DrawNode*> ParticleSystem::backgroundEffect(100);
std::vector<cocos2d::DrawNode*> ParticleSystem::backgroundEffect1(100);
std::vector<cocos2d::DrawNode*> ParticleSystem::pathEffect(15);
std::vector<cocos2d::DrawNode*> ParticleSystem::collisionEffect(30);


ParticleSystem::ParticleSystem(){
   
}
ParticleSystem::~ParticleSystem(){
   
}
void ParticleSystem::setPosPointInCircle(cocos2d::Vec2& point,cocos2d::Node* pointNode){
   float angle  = pointNode->getRotation();
   angle = angle * (M_PI/180);//Translate to radians
   point.y -= (radius * (cos(angle)));
   point.x -= (radius * (sin(angle))); 
}

void ParticleSystem::removeEffect(Effects typeEf, void* node){
   switch (typeEf){
      case Effects::START_ENGINE:{
         for (auto &particle : engineEffect)
            static_cast<cocos2d::Node*>(node)->removeChild(particle);
         break;
      }
      case Effects::LINE_PATH:{
         for (auto &particle : pathEffect){
            static_cast<cocos2d::Node*>(node)->removeChild(particle);
         }
         break;
      }
      case Effects::COLLISION:{
         break;
      }
   }
}
void ParticleSystem::createEffect(Effects typeEf,void* node){
   srand(time(0));
   switch (typeEf){
   case Effects::START_ENGINE:{
      cocos2d::Node* bb = static_cast<cocos2d::Node*>(node)->getChildByTag(LayerNode::ATLAS)->getChildByTag(AtlasNode::SHIP); 
      cocos2d::Vec2 posEng = bb->getPosition();
      radius = bb->getBoundingBox().size.width / 2.5f;
      setPosPointInCircle(posEng,bb);
      float speeOfFalling;
      cocos2d::Vec2 posEndParticleEff;
      float sizeX   = 8;
      float sizeY   = 8;
      /*Set dir of 'falling'*/
      setPosPointInCircle(posEndParticleEff,bb);

      for (uint i = 0; i < 28; i++){
         /*set up random speed for particle*/
         speeOfFalling = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/2));
         /*Clean up before start drawing*/
         if (!engineEffect.empty())
            static_cast<cocos2d::Node*>(node)->removeChild(engineEffect[i]);
         /*Create particle*/
         engineEffect[i] = cocos2d::DrawNode::create();
         engineEffect[i]->drawSolidRect(cocos2d::Vec2(posEng),cocos2d::Vec2(posEng.x+sizeX,posEng.y+sizeY),cocos2d::Color4F(0.78f,0.51f,0.1f,1.f));
         
         auto moves = cocos2d::EaseIn::create(cocos2d::MoveBy::create(speeOfFalling,cocos2d::Vec2(posEndParticleEff.x,posEndParticleEff.y)),2.f);
         auto fadeout = cocos2d::FadeOut::create(speeOfFalling);
         auto gaseout = cocos2d::Spawn::create(moves,fadeout,nullptr);
         auto fadein  = cocos2d::FadeIn::create(0.f);
         auto moveback  = cocos2d::MoveBy::create(0,cocos2d::Vec2(posEndParticleEff.x*(-1),posEndParticleEff.y*(-1)));
         auto appearnewgas = cocos2d::Spawn::create(fadein,moveback,nullptr); 
         auto action = cocos2d::Sequence::create(gaseout,appearnewgas,nullptr);
         engineEffect[i]->runAction(cocos2d::RepeatForever::create(action));
         /*Add particle to layer*/
         static_cast<cocos2d::Node*>(node)->addChild(engineEffect[i]);
      }
      break;
   }
   case Effects::BACKGROUND:{
      cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
      for (auto &ef : backgroundEffect){
         cocos2d::Vec2 posDot = cocos2d::Vec2(rand() % static_cast<int>(visibleSize.width),rand() % static_cast<int>(visibleSize.height));
         ef = cocos2d::DrawNode::create();
         ef->drawDot(posDot,3.f,cocos2d::Color4F(cocos2d::rand_0_1(),0.55f,cocos2d::rand_0_1(),cocos2d::rand_0_1()));
         auto blinkingIn = cocos2d::Spawn::create(cocos2d::MoveBy::create(1.f,cocos2d::Vec2(80,0)),cocos2d::FadeTo::create(1.f,rand()%100),nullptr);
         auto blinkingOut = cocos2d::Spawn::create(cocos2d::MoveBy::create(1.f,cocos2d::Vec2(80,0)),cocos2d::FadeTo::create(1.f,rand()%155+100),nullptr);
         auto action = cocos2d::Sequence::create(blinkingIn,blinkingOut,nullptr);
         ef->runAction(cocos2d::RepeatForever::create(action));
         static_cast<cocos2d::Node*>(node)->addChild(ef);
      }
      for (auto &ef : backgroundEffect1){
         cocos2d::Vec2 posDot = cocos2d::Vec2((rand() % static_cast<int>(visibleSize.width)) * -1,rand() % static_cast<int>(visibleSize.height));
         ef = cocos2d::DrawNode::create();
         ef->drawDot(posDot,3.f,cocos2d::Color4F(0.55f,cocos2d::rand_0_1(),cocos2d::rand_0_1(),cocos2d::rand_0_1()));
         auto blinkingIn = cocos2d::Spawn::create(cocos2d::MoveBy::create(1.f,cocos2d::Vec2(80,0)),cocos2d::FadeTo::create(1.f,rand()%100),nullptr);
         auto blinkingOut = cocos2d::Spawn::create(cocos2d::MoveBy::create(1.f,cocos2d::Vec2(80,0)),cocos2d::FadeTo::create(1.f,rand()%155+100),nullptr);
         auto action = cocos2d::Sequence::create(blinkingIn,blinkingOut,nullptr);
         ef->runAction(cocos2d::RepeatForever::create(action));
         static_cast<cocos2d::Node*>(node)->addChild(ef);
      }
      break;
   }
   case Effects::LINE_PATH:{
      cocos2d::Node* bb = static_cast<cocos2d::Node*>(node)->getChildByTag(LayerNode::ATLAS)->getChildByTag(AtlasNode::SHIP);
      float cat1 = static_cast<Game*>(node)->getTouchLocation().x - bb->getPosition().x;
      float cat2 = static_cast<Game*>(node)->getTouchLocation().y - bb->getPosition().y;
      float lenght = std::sqrt(pow(cat1,2)+pow(cat2,2));
      float stepBetPoint = lenght / pathEffect.size();
      for (int i = 0; i < pathEffect.size(); ++i){
         if (pathEffect[i] != nullptr)
            static_cast<cocos2d::Node*>(node)->removeChild(pathEffect[i]);   
         pathEffect[i] = cocos2d::DrawNode::create();
         cocos2d::Vec2 pos = static_cast<Game*>(node)->getTouchLocation();
         radius = 0 + stepBetPoint * i;
         setPosPointInCircle(pos,bb);
         pathEffect[i]->drawPoint(pos,8.f,cocos2d::Color4F(196.f/255.f,59.f/255.f,121.f/255.f,1.f - 0.065f*i));
         static_cast<cocos2d::Node*>(node)->addChild(pathEffect[i]);
      }
      break;
   }
   case Effects::COLLISION:{
      /*add some effects colors
         and attention*/
      cocos2d::Node* bb = static_cast<cocos2d::Node*>(node)->getChildByTag(LayerNode::ATLAS)->getChildByTag(AtlasNode::SHIP);
      cocos2d::Vec2 pos = static_cast<Game*>(node)->getCollisionLocation();
      for (int i = 0; i < collisionEffect.size(); i++){
         if (collisionEffect[i] != nullptr)
            static_cast<cocos2d::Node*>(node)->removeChild(collisionEffect[i]);
         collisionEffect[i] = cocos2d::DrawNode::create();
         collisionEffect[i]->drawPoint(pos,4.f + i/2.f,cocos2d::Color4F(251.f/255.f,cocos2d::random(100,255)/255.f,cocos2d::random(0,75)/255.f,1.f));
         auto move = cocos2d::MoveBy::create(1.f,cocos2d::Vec2(cocos2d::random(-100,100),cocos2d::random(-100,100)));
         auto fadeout = cocos2d::FadeOut::create(1.f);
         collisionEffect[i]->runAction(cocos2d::Spawn::create(move,fadeout,nullptr));
         static_cast<cocos2d::Node*>(node)->addChild(collisionEffect[i],7);
      }
      break;
   }
   }
}
