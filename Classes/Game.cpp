#include "Game.h"
#include "ParticleSystem.h"
#include "AudioEngine.h"


Game::~Game(){
}
bool Game::init(){

   visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
   isPause = false;
   isHit = false;
   lifes = 10;
   timeRemain = 120.f;
   frequencyOfSpawning = 5.f;
   isEndGame = false;

   initShipInstance();
   initWorldInstance();
   initEventsInstance();
   gui.initGUI(GameStages::PLAYING,this);

   return true;
}
void Game::initShipInstance(){
   //Init atlas of ship
   cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("textures/Atlas.plist");
   shipAtlas = cocos2d::SpriteBatchNode::create("textures/Atlas.png");
   this->addChild(shipAtlas,5,LayerNode::ATLAS);
   
   ship = cocos2d::Sprite::createWithSpriteFrameName("shipStill.png");
   ship->setScale(visibleSize.height/ship->getContentSize().height * 0.2);
   ship->setPosition(visibleSize.width/2,visibleSize.height/2);
   /*Physics stuff*/
   cocos2d::Vec2* poly = new cocos2d::Vec2[4];
   poly[0] = cocos2d::Vec2(0,ship->getContentSize().height/2 * -1 + 5);
   poly[1] = cocos2d::Vec2(ship->getContentSize().width*0.1 * -1,0);
   poly[2] = cocos2d::Vec2(0,ship->getContentSize().height/2 - 5);
   poly[3] = cocos2d::Vec2(ship->getContentSize().width*0.1,0);

   shipBody = cocos2d::PhysicsBody::createEdgePolygon(poly,4,cocos2d::PhysicsMaterial(1.0f,1.0f,1.0f));
   shipBody->setContactTestBitmask(0x02);
   shipBody->setCategoryBitmask(0x01);
   shipBody->setGravityEnable(false);
   shipBody->setRotationEnable(true);
   shipBody->setDynamic(true);
   ship->setPhysicsBody(shipBody);
   
   
   shipAtlas->addChild(ship,4,AtlasNode::SHIP);
}
void Game::initWorldInstance(){
   ParticleSystem::createEffect(Effects::BACKGROUND,this);
   isAttention = true;
   spawnTimer = 0.f;
}
void Game::initEventsInstance(){
   auto listener = cocos2d::EventListenerTouchOneByOne::create();
   listener->setSwallowTouches(true);
   listener->onTouchBegan = CC_CALLBACK_2(Game::onTouchBegan,this);
   listener->onTouchMoved = CC_CALLBACK_2(Game::onTouchMove,this);
   listener->onTouchEnded = CC_CALLBACK_2(Game::onTouchEnded,this);
   _eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
   //Create listener for contacts physics body
   auto contactListener = cocos2d::EventListenerPhysicsContact::create();
   contactListener->onContactBegin = CC_CALLBACK_1(Game::onContactBegin,this);
   _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener,this);
   this->schedule(CC_SCHEDULE_SELECTOR(Game::updateAllInstance),0.1f,CC_REPEAT_FOREVER,0.1f);
}
void Game::updateAllInstance(float dt){
   /*Stop update gui like time_remain lifes and menu_button*/
   updateBackground(dt);
   updateGameStages(dt);
   if (!isEndGame){
      updateAsteroids(dt); 
      updateRocket(dt);
      updateGUI(dt);
   }
}
void Game::updateRocket(float dt){
   if ( isOnTouchMove ){ 
      float distanceX =  cursorPos.x - ship->getPosition().x;//Find distance from ship to touch point by OX
      float distanceY =  cursorPos.y - ship->getPosition().y;//Find distance from ship to touch point by OY
      float hypotenuse = sqrt(pow(distanceX,2) + pow(distanceY,2));//Find straight distance from ship to touch point
      float rad =  asin(distanceX/hypotenuse);//Find angle by radian between projection of straight  and straight by itself
      float angle = rad * (180/M_PI);//Convert radian to degrees
      //If our touch point bellow anchor point of ship anf in positive half of ship circle
      if (distanceY <= 0 && angle > 1)
         angle = 90 + (90 - angle);
      //If our touch point bellow anchor point of ship anf in negative half of ship circle
      else if (distanceY <= 0 && angle <= 1)
         angle = -90 + (-90 + (angle*(-1)));
      //Make some good transition
      ParticleSystem::createEffect(Effects::LINE_PATH,this);
      ship->runAction(cocos2d::EaseInOut::create(cocos2d::RotateTo::create(0.1f,angle),3.f));
   }
   if (isEffects){
      ParticleSystem::createEffect(Effects::START_ENGINE,this);
      if (ship->getBoundingBox().containsPoint(cursorPos)){
         isEffects = false;
         ParticleSystem::removeEffect(Effects::START_ENGINE,this);
      }
   }
   if (ship->getPosition().y > visibleSize.height + 25)
      ship->setPosition(ship->getPosition().x,-10);
   else if (ship->getPosition().y < -25)
      ship->setPosition(ship->getPosition().x,visibleSize.height + 10);
   else if (ship->getPosition().x < -25)
      ship->setPosition(visibleSize.width + 10,ship->getPosition().y);
   else if (ship->getPosition().x > visibleSize.width + 25)
      ship->setPosition(-10,ship->getPosition().y);
}
void Game::updateAsteroids(float dt){
   /*If pause event activate we will not anymore spawn(delete) new asteroids*/
   if (!isPause){
      spawnTimer += dt;
      if (spawnTimer >= frequencyOfSpawning){
         frequencyOfSpawning -= 0.05;
         asteroid.createAsteroid(this,asteroid.getSizePool());
         spawnTimer = 0.f;
         isAttention = true;
      }
      else if (spawnTimer >= (frequencyOfSpawning - 3.f) && isAttention){
         asteroid.createAsteroidAttention(this,asteroid.getSizePool());
         isAttention = false;
      }
      for (int i = 0; i < asteroid.getSizePool(); i++)
         asteroid.updateAsteroid(this,i,dt);
   }
}
void Game::updateBackground(float dt){
   if (ParticleSystem::backgroundEffect[0]->getPositionX() >= visibleSize.width){
      for (int i = 0; i < ParticleSystem::backgroundEffect.size(); ++i ){
         ParticleSystem::backgroundEffect[i]->setPosition(visibleSize.width*-1,ParticleSystem::backgroundEffect[i]->getPositionY());
      }
   }
   if (ParticleSystem::backgroundEffect1[0]->getPositionX() >= visibleSize.width*2){
      for (int i = 0; i < ParticleSystem::backgroundEffect1.size(); ++i ){
         ParticleSystem::backgroundEffect1[i]->setPosition(0,ParticleSystem::backgroundEffect1[i]->getPositionY());
      }
   }
}
void Game::updateGUI(float dt){
   if (!isPause){
      if (isHit){
         isHit = false;
         lifes--;
         std::string str = std::to_string(lifes);
         gui.lifes->setString("Lifes: " + str);
      }
      timeRemain -= dt;
      std::string str = std::to_string(int(timeRemain));
      gui.time_remain->setString("Time remain: " + str + " sec");
   }
}
void Game::updateGameStages(float dt){
   if (!isEndGame){
      //Win condition
      if (int(timeRemain) == 0){
         isEndGame = true;
         gui.initGUI(GameStages::WIN_GAME,this);
      }
      //Lose condition
      if (lifes == 0){
         isEndGame = true;
         gui.initGUI(GameStages::LOSE_GAME,this);
      }
   }
}

bool Game::onContactBegin(cocos2d::PhysicsContact &contact){
   cocos2d::PhysicsBody *a = contact.getShapeA()->getBody();
   cocos2d::PhysicsBody *b = contact.getShapeB()->getBody();
   //Check if body was collided
   if ((a->getCategoryBitmask() & b->getContactTestBitmask() == 0 ) || (b->getCategoryBitmask() & a->getContactTestBitmask() == 0)){
      return false;
   }
   else{
      isHit = true;
      a->setVelocity(cocos2d::Vec2(-100,0));
      contactPos = contact.getContactData()->points[0];
      ParticleSystem::createEffect(Effects::COLLISION,this);
      cocos2d::AudioEngine::play2d("music/collision_with_asteroid.mp3");
      return true;
   }
}
bool Game::onTouchBegan(cocos2d::Touch* touch,cocos2d::Event* event){
   /*If pause event activate we will not any more interact with ship*/
   if (ship->getBoundingBox().containsPoint(touch->getLocation()) && !isPause && !isEndGame){
      cursorPos = touch->getLocation();//I'm duplicate this here because some issue with disappearing ship and particles
      isOnTouchMove = true;
      isEffects = false;
      ParticleSystem::removeEffect(Effects::START_ENGINE,this);
      return true;   
   }
   return false;
}
void Game::onTouchMove(cocos2d::Touch* touch,cocos2d::Event* event){
   if (isOnTouchMove && !isEndGame){
      cursorPos = touch->getLocation();
      ParticleSystem::createEffect(Effects::LINE_PATH,this);
   }
}
void Game::onTouchEnded(cocos2d::Touch* touch,cocos2d::Event* event){
   if (isOnTouchMove && !isEndGame){
      isOnTouchMove = false;
      isEffects = true;
      ParticleSystem::removeEffect(Effects::LINE_PATH,this);
      ship->runAction(cocos2d::EaseInOut::create(cocos2d::MoveTo::create(2.5f,cursorPos),1.5f));
      /*I do not know why it is happening but it happening MoveTo class just wont work correctly(ship sprite just disappearing) or I'm just dumb*/   
   }
}

cocos2d::Scene* Game::createScene(){
   
   cocos2d::Scene* scene = cocos2d::Scene::createWithPhysics();
   cocos2d::Node* layer = Game::create();
   scene->addChild(layer);

   //Debug shit
   //cocos2d::PhysicsWorld* scenePh = scene->getPhysicsWorld();
   //scenePh->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
   
   return scene;
}
cocos2d::Vec2 Game::getTouchLocation(){
   return cursorPos;
}
cocos2d::Vec2 Game::getCollisionLocation(){
   return contactPos;
}






void Game::Asteroid::createAsteroid(Game* obj,int positionInPool){
   /*Increase pool of asteroids*///HERE seg fault
   currentNumOfAsteroids++;
   /*Push in to pool new object*/
   sprite.push_back(cocos2d::Sprite::createWithSpriteFrameName("asteroid"+std::to_string(rand()%5+1)+".png"));
   lifeTime.push_back(15.f);
   /*Now this asteroid is updatable, for now*/
   isUpdateAble.push_back(true);
   /*Sprite body*/
   sprite[positionInPool]->setAnchorPoint(cocos2d::Vec2(0.5,0.5));
   sprite[positionInPool]->setPosition(spawnPos[positionInPool]);
   sprite[positionInPool]->setScale((rand()%3 + 2) + ((rand()%10 + 1) * 0.1));
   sprite[positionInPool]->setRotation(rand()%360);
   /*Physic body*/
   /*Init and set up to sprite physics body*/
   spriteBody.push_back(cocos2d::PhysicsBody::createCircle(MIN(sprite[positionInPool]->getContentSize().width*0.4,sprite[positionInPool]->getContentSize().height*0.4),cocos2d::PhysicsMaterial(1.f,1.f,0.5f)));
   /*Set up group with wich this asteroid can interact*/
   spriteBody[positionInPool]->setContactTestBitmask(0x01);
   /*Set up group itself for this asteroid*/
   spriteBody[positionInPool]->setCategoryBitmask(0x02);
   spriteBody[positionInPool]->setGravityEnable(false);
   spriteBody[positionInPool]->setRotationEnable(true);
   spriteBody[positionInPool]->setDynamic(true);
   /*Init initial velocity of asteroid*/
   spriteBody[positionInPool]->setVelocity(cocos2d::Vec2(100,0));
   sprite[positionInPool]->setPhysicsBody(spriteBody[positionInPool]);
   /*Add this asteroid to layer*/
   obj->shipAtlas->addChild(sprite[positionInPool],5,AtlasNode::ASTEROID);  
}
void Game::Asteroid::createAsteroidAttention(Game* obj,int positionInPool){
   /*Pull out from common atlas sprite atention.png and create sprite instanse*/
   spriteAttention.push_back(cocos2d::Sprite::createWithSpriteFrameName("atention.png"));
   /*Ser up position for asteroid and attention spawn*/
   spawnPos.push_back(cocos2d::Vec2(-150,rand() % int((obj->visibleSize.height))));
   /*Set up updatable status to attention*/
   isAttentionRemoveable.push_back(true);
   spriteAttention[positionInPool]->setPosition(25.f,spawnPos[positionInPool].y);
   /*add this sprite to layer*/
   obj->shipAtlas->addChild(spriteAttention[positionInPool],10,AtlasNode::ATTENTION);
}
void Game::Asteroid::removeAsteroid(Game* obj,int positionInPool){
   isUpdateAble[positionInPool] = false;//set asteroid not updateable(live)
   obj->shipAtlas->removeChild(sprite[positionInPool],true);//Remove texture from layer
   spriteBody[positionInPool]->removeFromWorld();//Remove physical body
}
void Game::Asteroid::removeAttention(Game* obj,int positionInPool){
   isAttentionRemoveable[positionInPool] = false;/*set attention no longer updatable*/
   obj->shipAtlas->removeChild(spriteAttention[positionInPool],true);//Remove texture from layer
}
void Game::Asteroid::updateAsteroid(Game* obj,int positionInPool,float dt){
   /*If updated asteroid still life*/
   if (isUpdateAble[positionInPool]){
      /*decrease his life time*/
      lifeTime[positionInPool] -= dt;
      /*and if he lost all his time delete it*/
      if (lifeTime[positionInPool] <= 0)
         removeAsteroid(obj,positionInPool);
      /*if attention still on screen and he live longer than 2 sec after asteroid was created, delete it*/
      if (lifeTime[positionInPool] <= 13.f && isAttentionRemoveable[positionInPool])
         removeAttention(obj,positionInPool);
   }
}
void Game::Asteroid::cleanAll(Game* obj){
   for (int i = 0; i < obj->asteroid.getSizePool(); i++){
      if (isUpdateAble[i]){
         removeAsteroid(obj,i);
         if (isAttentionRemoveable[i]){
            isAttentionRemoveable[i] = false;/*set attention no longer updatable*/
            obj->shipAtlas->removeChild(spriteAttention[i],true);//Remove texture from layer
         }
      }  
   }
}
int Game::Asteroid::getSizePool(){
   /*return how many asteroid was created*/
   return currentNumOfAsteroids;
}





void Game::GUI::initGUI(GameStages stage, Game* obj){
   /*Clean up before create new gui stuff(only if it's already allocated*/
   if (isInit){
      obj->removeChild(menu_buttom);
      obj->removeChild(lifes);
      obj->removeChild(time_remain);
      for (auto &item_of_pop_up_menu : pop_up_menu)
         obj->removeChild(item_of_pop_up_menu);
      pop_up_menu.clear();
   }
   switch (stage){
   case GameStages::PLAYING:{
      isInit = true;
      usedOnce = true;

      initPopUpMenu(obj);

      initMenuButton(obj);

      initCounters(obj);

      break;
   }
   case GameStages::WIN_GAME:{
      isInit = true;
      
      endMessage = initEndGameMessage("You beat my game !!!",obj);

      pop_up_menu.push_back(cocos2d::ui::Button::create("textures/StartShipTex/PA.png","textures/StartShipTex/PA_s.png"));
      pop_up_menu.push_back(cocos2d::ui::Button::create("textures/StartShipTex/E.png","textures/StartShipTex/E_s.png"));

      for (int i = 0; i < pop_up_menu.size(); i++){
         float item_pos = 0.8f;
         item_pos -= 0.3f * i;
         pop_up_menu[i]->setOpacity(0);
         pop_up_menu[i]->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height * item_pos));
         pop_up_menu[i]->runAction(cocos2d::FadeTo::create(2.f,255));
         obj->addChild(pop_up_menu[i]);
      }
      pop_up_menu[0]->addTouchEventListener([&,obj](cocos2d::Ref *sender,cocos2d::ui::Widget::TouchEventType type){
         obj->removeChild(endMessage);
         initGUI(GameStages::PLAYING,obj);
         obj->isEndGame = false;

         cocos2d::Size vs = cocos2d::Director::getInstance()->getVisibleSize();
         obj->isPause = false;
         usedOnce = true;
         menu_buttom->runAction(cocos2d::FadeTo::create(2.f,255));
         /*Continue updating all neccessary stuff*/
         for (int i = 0; i < obj->asteroid.getSizePool(); ++i)
            if (obj->asteroid.isUpdateAble[i])
               obj->asteroid.spriteBody[i]->setVelocity(cocos2d::Vec2(100,0));
         /*Set up starting positions*/
         pop_up_menu[Buttons::RESUME]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.8));
         pop_up_menu[Buttons::PLAY_AGAIN]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.6));
         pop_up_menu[Buttons::QUIT]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.2));
         obj->ship->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height/2));
         obj->ship->setRotation(0);
         obj->frequencyOfSpawning = 5.f;
         obj->asteroid.cleanAll(obj);

         obj->timeRemain = 120;
         time_remain->setString("Time remain: 120 sec");


         obj->lifes = 10;
         lifes->setString("Lifes: 10");
      });
      pop_up_menu[1]->addTouchEventListener([&](cocos2d::Ref *sender,cocos2d::ui::Widget::TouchEventType type){
         cocos2d::Director::getInstance()->end();
      });



      break;
   }
   case GameStages::LOSE_GAME:{
      isInit = true;

      endMessage = initEndGameMessage("My game too hard !!!",obj);

      pop_up_menu.push_back(cocos2d::ui::Button::create("textures/StartShipTex/PA.png","textures/StartShipTex/PA_s.png"));
      pop_up_menu.push_back(cocos2d::ui::Button::create("textures/StartShipTex/E.png","textures/StartShipTex/E_s.png"));

      for (int i = 0; i < pop_up_menu.size(); i++){
         float item_pos = 0.8f;
         item_pos -= 0.3f * i;
         pop_up_menu[i]->setOpacity(0);
         pop_up_menu[i]->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height * item_pos));
         pop_up_menu[i]->runAction(cocos2d::FadeTo::create(2.f,255));
         obj->addChild(pop_up_menu[i]);
      }
      pop_up_menu[0]->addTouchEventListener([&,obj](cocos2d::Ref *sender,cocos2d::ui::Widget::TouchEventType type){
         obj->removeChild(endMessage);
         initGUI(GameStages::PLAYING,obj);
         obj->isEndGame = false;

         cocos2d::Size vs = cocos2d::Director::getInstance()->getVisibleSize();
         obj->isPause = false;
         usedOnce = true;
         menu_buttom->runAction(cocos2d::FadeTo::create(2.f,255));
         /*Continue updating all neccessary stuff*/
         for (int i = 0; i < obj->asteroid.getSizePool(); ++i)
            if (obj->asteroid.isUpdateAble[i])
               obj->asteroid.spriteBody[i]->setVelocity(cocos2d::Vec2(100,0));
         /*Set up starting positions*/
         pop_up_menu[Buttons::RESUME]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.8));
         pop_up_menu[Buttons::PLAY_AGAIN]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.6));
         pop_up_menu[Buttons::QUIT]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.2));
         obj->ship->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height/2));
         obj->ship->setRotation(0);
         obj->asteroid.cleanAll(obj);

         obj->timeRemain = 120;
         time_remain->setString("Time remain: 120 sec");


         obj->lifes = 10;
         lifes->setString("Lifes: 10");
      });
      pop_up_menu[1]->addTouchEventListener([&](cocos2d::Ref *sender,cocos2d::ui::Widget::TouchEventType type){
         cocos2d::Director::getInstance()->end();
      });
      
      break;
   }
   }

}

void Game::GUI::initMenuButton(Game* obj){
   menu_buttom = cocos2d::ui::Button::create("textures/StartShipTex/menu_uns.png","textures/StartShipTex/menu_s.png");
   menu_buttom->setScale(obj->visibleSize.width/menu_buttom->getContentSize().width * 0.15);
   menu_buttom->setPosition(cocos2d::Vec2(menu_buttom->getBoundingBox().size.width,
                                          menu_buttom->getBoundingBox().size.height));
   menu_buttom->addTouchEventListener([&,obj](cocos2d::Ref *sender,cocos2d::ui::Widget::TouchEventType type){
      switch (type){
         /*Pause all neccessary stuff*/
         case cocos2d::ui::Widget::TouchEventType::ENDED:{
            obj->isPause = true;
            for (int i = 0; i < obj->asteroid.getSizePool(); ++i)
               if (obj->asteroid.isUpdateAble[i])
                  obj->asteroid.spriteBody[i]->setVelocity(cocos2d::Vec2(0,0));
            break;
         }
         case cocos2d::ui::Widget::TouchEventType::BEGAN:{
            if (usedOnce){
               usedOnce = false;
               auto sneekOut = cocos2d::FadeTo::create(1.f,50);
               menu_buttom->runAction(sneekOut);
               for (int i = 0; i < pop_up_menu.size(); i++){
               
                  float item_pos = 0.8f;
                  item_pos -= 0.2f * i;
                  pop_up_menu[i]->runAction(cocos2d::MoveTo::create(2.f,cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height * item_pos)));
               }
            }
            break;
         }
      }
   });
   obj->addChild(menu_buttom,11); 
}
void Game::GUI::initPopUpMenu(Game* obj){
   pop_up_menu.push_back(cocos2d::ui::Button::create("textures/StartShipTex/R.png","textures/StartShipTex/R_s.png"));
   pop_up_menu.push_back(cocos2d::ui::Button::create("textures/StartShipTex/PA.png","textures/StartShipTex/PA_s.png"));
   pop_up_menu.push_back(cocos2d::ui::Button::create("textures/StartShipTex/E.png","textures/StartShipTex/E_s.png"));
   
   pop_up_menu[Buttons::RESUME]->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height + obj->visibleSize.height * 0.8));
   pop_up_menu[Buttons::PLAY_AGAIN]->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height + obj->visibleSize.height * 0.6));
   pop_up_menu[Buttons::QUIT]->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height + obj->visibleSize.height * 0.2));
   
   /*Unpause all neccessary stuff*/
   pop_up_menu[Buttons::RESUME]->addTouchEventListener([&,obj](cocos2d::Ref *sender,cocos2d::ui::Widget::TouchEventType type){
      if (type == cocos2d::ui::Widget::TouchEventType::BEGAN){
         usedOnce = true;
         cocos2d::Size vs = cocos2d::Director::getInstance()->getVisibleSize();
         obj->isPause = false;
         menu_buttom->runAction(cocos2d::FadeTo::create(2.f,255));
         /*Continue updating all neccessary stuff*/
         for (int i = 0; i < obj->asteroid.getSizePool(); ++i)
            if (obj->asteroid.isUpdateAble[i])
               obj->asteroid.spriteBody[i]->setVelocity(cocos2d::Vec2(100,0));
         /*Set up starting positions*/
         pop_up_menu[Buttons::RESUME]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.8));
         pop_up_menu[Buttons::PLAY_AGAIN]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.6));
         pop_up_menu[Buttons::QUIT]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.2));
      }
   });
   pop_up_menu[Buttons::PLAY_AGAIN]->addTouchEventListener([&,obj](cocos2d::Ref *sender,cocos2d::ui::Widget::TouchEventType type){
      if (type == cocos2d::ui::Widget::TouchEventType::BEGAN){
         cocos2d::Size vs = cocos2d::Director::getInstance()->getVisibleSize();
         obj->isPause = false;
         usedOnce = true;
         menu_buttom->runAction(cocos2d::FadeTo::create(2.f,255));
         /*Continue updating all neccessary stuff*/
         for (int i = 0; i < obj->asteroid.getSizePool(); ++i)
            if (obj->asteroid.isUpdateAble[i])
               obj->asteroid.spriteBody[i]->setVelocity(cocos2d::Vec2(100,0));
         /*Set up starting positions*/
         pop_up_menu[Buttons::RESUME]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.8));
         pop_up_menu[Buttons::PLAY_AGAIN]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.6));
         pop_up_menu[Buttons::QUIT]->setPosition(cocos2d::Vec2(vs.width/2,vs.height + vs.height * 0.2));
         obj->ship->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height/2));
         obj->ship->setRotation(0);
         obj->asteroid.cleanAll(obj);

         obj->timeRemain = 120;
         time_remain->setString("Time remain: 120 sec");


         obj->lifes = 10;
         lifes->setString("Lifes: 10");
      }
   });
   pop_up_menu[Buttons::QUIT]->addTouchEventListener([&](cocos2d::Ref *sender,cocos2d::ui::Widget::TouchEventType type){
      cocos2d::Director::getInstance()->end();
   });
   for (auto &menu_item : pop_up_menu)
      obj->addChild(menu_item,10);
}
void Game::GUI::initCounters(Game* obj){
   lifes = cocos2d::Label::createWithTTF("Lifes: 10","fonts/spaceAge.ttf",25);
   lifes->setPosition(cocos2d::Vec2(obj->visibleSize.width*0.1,obj->visibleSize.height*0.9));
   obj->addChild(lifes,11);
   
   time_remain = cocos2d::Label::createWithTTF("Time remain: 120 sec","fonts/spaceAge.ttf",25);
   time_remain->setAnchorPoint(cocos2d::Vec2(1,0));
   time_remain->setPosition(cocos2d::Vec2(obj->visibleSize.width*0.9,obj->visibleSize.height*0.9));
   obj->addChild(time_remain,11);
}

cocos2d::Label* Game::GUI::initEndGameMessage(std::string str,Game* obj){
   cocos2d::Label* lab = cocos2d::Label::create();
   lab->setSystemFontName("fonts/spaceAge.ttf");
   lab->setString(str);
   lab->setSystemFontSize(40);
   lab->setPosition(cocos2d::Vec2(obj->visibleSize.width/2,obj->visibleSize.height*0.1));
   obj->addChild(lab);

   return lab;
}