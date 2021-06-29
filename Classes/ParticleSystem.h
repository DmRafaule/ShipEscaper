#pragma once 

#include "cocos2d.h"

#include <vector>


enum Effects{
   START_ENGINE,
   BACKGROUND,
   LINE_PATH,
   COLLISION,
};


class ParticleSystem : public cocos2d::Scene{
public:
   ParticleSystem();
   ~ParticleSystem();
   
   static void createEffect(Effects typeEf, void* node);
   static void removeEffect(Effects typeEf, void* node);
private:
      /** Set position of point related to pointNode like pointNode is a center of circle
      @param point is a point will be changed to new position depends on pos of pointNOde and angle of texture of point node
      @param pointNode is a center of entity node which have to have some kind of position and angle 
      **/
   static void setPosPointInCircle(cocos2d::Vec2& point,cocos2d::Node* pointNode);
   
public:
   static float   radius;
   static std::vector<cocos2d::DrawNode*> engineEffect;
   static std::vector<cocos2d::DrawNode*> backgroundEffect;
   static std::vector<cocos2d::DrawNode*> backgroundEffect1;
   static std::vector<cocos2d::DrawNode*> pathEffect;
   static std::vector<cocos2d::DrawNode*> collisionEffect;

};