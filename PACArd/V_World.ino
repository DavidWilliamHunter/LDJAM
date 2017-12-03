#define DOOR_COUNTDOWN 10
#define RENDER_INTERVAL 25

// Do the Doors
bool doorStatus = true;
unsigned int doorCountDown = DOOR_COUNTDOWN;



// food remaining
unsigned int foodRemaining = 0;

void initWorld(unsigned int level)
{
  doorStatus = false;
  doorCountDown = DOOR_COUNTDOWN;

  setCurrentLevel(level);
  foodRemaining = foodRemainingInLevel();
  
  initPlayer(level);  
  initGhosts(level);
  spawnGhosts(MAX_GHOSTS);
}

bool isObstacle(uVec2 &location)
{
  if(isLevelObstacle(location, doorStatus))
    return true;
  return false;    
}

unsigned long lastRenderWorld = 0;
void renderWorld()
{
  if(millis() - lastRenderWorld < RENDER_INTERVAL)
    return;
  lastRenderWorld = millis();    
  renderLevel(doorStatus);
  renderPlayer();
  renderGhosts();
}

void updateWorld()
{
  updatePlayer();
  updateGhosts();  

  // update doors
  if(doorCountDown==0)
   {
      doorStatus = !doorStatus;
      doorCountDown = DOOR_COUNTDOWN;
   }
   doorCountDown--;
}

