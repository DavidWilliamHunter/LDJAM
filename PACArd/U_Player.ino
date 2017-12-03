/*
   Create a player i.e. PACArd
*/

#define PLAYER_UPDATE_INTERVAL 100

uVec2 playerLocation = uVec2(BAD_LOC, BAD_LOC);


// create a player, assuming the world has been initialised.
bool initPlayer(unsigned int level)
{
  foodEaten = 0;
  uVec2 loc = getPlayerSpawn();
  // TODO: check that there is nothing in the way
  if (loc.x == BAD_LOC || loc.y == BAD_LOC)
    return false;
  playerLocation = loc;
  currentDifficulty = level;
}

unsigned long lastPlayerUpdate = 0;
void updatePlayer()
{
  if(millis() - lastPlayerUpdate < PLAYER_UPDATE_INTERVAL)
    return;
  lastPlayerUpdate = millis();
      
  uVec2 newLoc(playerLocation);
  if (AberLED.getButtonDown(UP))
    newLoc.y--;
  if (AberLED.getButtonDown(DOWN))
    newLoc.y++;
  if (AberLED.getButtonDown(LEFT))
    newLoc.x--;
  if (AberLED.getButtonDown(RIGHT))
    newLoc.x++;

  if(!isObstacle(newLoc) && newLoc.isValid())
    playerLocation = newLoc;

  // check for food
  if(isFoodPlusClear(newLoc))
  {
    foodEaten++;
    angerGhosts(GHOST_FOOD_ANGER_START + currentDifficulty * GHOST_FOOD_ANGER_FACTOR);
    if(foodRemainingInLevel()==0)
      setGameState(GAME_STATE_WIN);
  }

  if(isGhost(playerLocation))
    setGameState(GAME_STATE_LOSE);
}

void renderPlayer()
{
  AberLED.set(playerLocation.x, playerLocation.y, GREEN);
}

uVec2 getPlayerLocation() {
  return playerLocation;
}

