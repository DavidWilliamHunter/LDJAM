#include <AberLED2.h>

#define USE_EEPROM
#ifdef USE_EEPROM
#include <EEPROM.h>
#endif

#define WALL '#'
#define DOOR '/'
#define FOOD '.'
#define SPACE ' '
#define PLAYER_SPAWN  'P'
#define GHOST_SPAWN_UD 'G'
#define GHOST_SPAWN_LR 'H'

#define GHOST_FOOD_ANGER_START 100
#define GHOST_FOOD_ANGER_FACTOR 1

#define SCREEN_WIPE_TILES 1

#define PLAYER_LIVES 3

#define GAME_STATE_START  0
#define GAME_STATE_PLAYING  1
#define GAME_STATE_WIN      2
#define GAME_STATE_LOSE     3
#define GAME_STATE_LIFE_DOWN  4

#define WORLD_UPDATE_INTERVAL 100


unsigned int lives = PLAYER_LIVES;
unsigned int foodEaten = 0;
unsigned int currentDifficulty = 0;

template<typename T>
class Vec2T;
typedef Vec2T<unsigned char> uVec2;
typedef Vec2T<signed char> Vec2;

// The world strobe
// a long updated every update tick, subsystems can use modulo arithmetic to implement their strobers
unsigned long lastTickTime = 0;
unsigned long tickCount = 0;
unsigned char gameState = 0;

void setup() {
  // put your setup code here, to run once:
  AberLED.begin();
  Serial.begin(9600);

  setGameState(GAME_STATE_START);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - lastTickTime < WORLD_UPDATE_INTERVAL)
    return;
  lastTickTime = millis();
  AberLED.clear();
  switch (gameState)
  {
    case GAME_STATE_START :
      displayStartScreen();
      if (AberLED.getButtonDown(FIRE) || AberLED.getButtonDown(UP) || AberLED.getButtonDown(DOWN) || AberLED.getButtonDown(LEFT) || AberLED.getButtonDown(RIGHT))
      {
        setGameState(GAME_STATE_PLAYING);
      }
      break;
    case GAME_STATE_PLAYING :
      updateWorld();    
      renderWorld();      
      break;
    case GAME_STATE_WIN :
      displayWinScreen();
      if (AberLED.getButtonDown(FIRE) || AberLED.getButtonDown(UP) || AberLED.getButtonDown(DOWN) || AberLED.getButtonDown(LEFT) || AberLED.getButtonDown(RIGHT))
      {
        setGameState(GAME_STATE_PLAYING);
      }
      break;
    case GAME_STATE_LOSE :
      displayDeathScreen();
      if (AberLED.getButtonDown(FIRE) || AberLED.getButtonDown(UP) || AberLED.getButtonDown(DOWN) || AberLED.getButtonDown(LEFT) || AberLED.getButtonDown(RIGHT))
      {
        if(lives>0)
          {
            setGameState(GAME_STATE_PLAYING);
          } else {
            setGameState(GAME_STATE_START);
          }
      }
      break;
  }
  tickCount++;
  /*  updatePlayer();
    uVec2(true);
    renderPlayer(); */

  AberLED.swap();
}

void setGameState(unsigned char gs)
{
  gameState = gs;
  switch(gs)
  {
    case GAME_STATE_START :
      doTransition(GAME_STATE_START);
      currentDifficulty = 0;
      lives = PLAYER_LIVES;
      foodEaten = 0;
      break;
    case GAME_STATE_PLAYING :
      initWorld(currentDifficulty);
      break;
    case GAME_STATE_WIN :
          renderWorld();      
          doTransition(GAME_STATE_WIN);
      currentDifficulty++;
      break;
    case GAME_STATE_LOSE :  
          renderWorld();      
      doTransition(GAME_STATE_LOSE);
      lives--;  
      break;  
  }
}

