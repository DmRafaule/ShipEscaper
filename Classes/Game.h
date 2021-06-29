#pragma once

#include "cocos2d.h"
#include <ui/CocosGUI.h>

enum Buttons{
   RESUME,
   PLAY_AGAIN,
   QUIT,
};
enum GameStages{
   PLAYING,
   WIN_GAME,
   LOSE_GAME,
};
enum AtlasNode{//Tags for all entities of attlass
   SHIP,
   ASTEROID,
   ATTENTION
};
enum LayerNode{//Tags for all entities of current layer
   ATLAS,
};



class Game : public cocos2d::Scene{
public:
   friend class Asteroid;
   friend class GUI;

   virtual bool init();
   virtual ~Game();
   static cocos2d::Scene* createScene();
   CREATE_FUNC(Game);
   cocos2d::Vec2 getTouchLocation();
   cocos2d::Vec2 getCollisionLocation();
private:
   void initShipInstance();
   void initWorldInstance();
   void initEventsInstance();
   void updateAllInstance(float dt);
   void updateRocket(float dt);
   void updateAsteroids(float dt);
   void updateGUI(float dt);
   void updateBackground(float dt);
   void updateGameStages(float dt);
   bool onTouchBegan(cocos2d::Touch* touch,cocos2d::Event* event);
   void onTouchMove(cocos2d::Touch* touch,cocos2d::Event* event);
   void onTouchEnded(cocos2d::Touch* touch,cocos2d::Event* event);
   bool onContactBegin(cocos2d::PhysicsContact &contact);
private:
   class GUI {
   friend class Game;
   public:
      void initGUI(GameStages stage, Game* obj);
   private:
      void initMenuButton(Game* obj);
      void initPopUpMenu(Game* obj);
      void initCounters(Game* obj);
      cocos2d::Label* initEndGameMessage(std::string str,Game* obj);
   private:
      cocos2d::Label* endMessage;
      cocos2d::Label* lifes;
      cocos2d::Label* time_remain;
      cocos2d::ui::Button* menu_buttom;
      std::vector<cocos2d::ui::Button*> pop_up_menu;
      cocos2d::Vec2 start_pos; 
      bool usedOnce;
      bool isInit = false;
   }gui;
   /*entinty for handling asteroids*/
   class Asteroid {
   public:
      friend class GUI;
      /*create instance*/
      void createAsteroid(Game* obj,int positionInPool);
      /*create instance attention*/
      void createAsteroidAttention(Game* obj,int positionInPool);
      /*clean up instance of asteroid*/
      void removeAsteroid(Game* obj,int positionInPool);
      /*clean up instance of attention*/
      void removeAttention(Game* obj,int positionInPool);
      /*update each asteroid(it's life time, positions and stats*/
      void updateAsteroid(Game* obj,int positionInPool,float dt);
      /*return size of pool*/
      void cleanAll(Game* obj);
      int getSizePool();
   private:
      std::vector<cocos2d::Sprite*> sprite;/*sprite texture for each asteroid*/
      std::vector<cocos2d::Sprite*> spriteAttention;/*sprite texture for each asteroid*/
      std::vector<cocos2d::PhysicsBody*> spriteBody; /*physical body for each asteroid*/
      std::vector<cocos2d::Vec2> spawnPos;/*spawn position for each asteroid*/
      std::vector<float> lifeTime;//How long will be live asteroid in pool
      std::vector<bool>  isUpdateAble;//Is this asteroid in pool updateable
      std::vector<bool>  isAttentionRemoveable;
      int            currentNumOfAsteroids = 0; /*how many asteroids contain this obj pull*/
   }asteroid;
   
   /*Contain all needed sprites*/
   cocos2d::SpriteBatchNode*  shipAtlas;
   /*physical body for ship*/
   cocos2d::PhysicsBody*      shipBody;
   /*Texture holder for ship*/
   cocos2d::Sprite*           ship;

   cocos2d::DrawNode*         line = nullptr;
   cocos2d::Size              visibleSize;
   cocos2d::Vec2              cursorPos;
   cocos2d::Vec2              contactPos;

   int                        lifes;
   float                      timeRemain;
   float                      spawnTimer;//Timer for spawning new interactive obj (asteroids ...)
   float                      frequencyOfSpawning;//How fast asteroids will be spawning

   bool                       isOnTouchMove;//If you touch and move
   bool                       isEffects;//Is some effects on screen (for engine eff)
   bool                       isAttention;//Is attention on screen
   bool                       isLine;//Is line drawed on screen
   bool                       isPause;
   bool                       isHit;
   bool                       isEndGame;
};


