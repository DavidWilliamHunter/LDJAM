/**
   Ghosts
*/

#define MAX_GHOSTS     16

#define GHOST_DEAD     0
#define GHOST_GROW_LR     1
#define GHOST_GROW_UD     2
#define GHOST_PATROL_UP     3
#define GHOST_PATROL_DOWN   4
#define GHOST_PATROL_LEFT   5
#define GHOST_PATROL_RIGHT  6
#define GHOST_CHASE    7
#define GHOST_DIEING   8

#define GHOST_GROW_TIME 10
#define GHOST_PATROL_TIME 1000
#define GHOST_CHASE_TIME 50
#define GHOST_DIEING_TIME 2
#define GHOST_ANGRY 10

#define GHOST_UPDATE_OFFSET 200
#define GHOST_UPDATE_FACTOR 10
#define GHOST_UPDATE_MIN    50

unsigned char ghostLevel = 0;

struct Ghost {
  uVec2 location;
  unsigned char state;
  unsigned int countDown;
};

Ghost ghosts[MAX_GHOSTS];

void initGhosts(unsigned char level_)
{
  setGhostLevel(level_);
  for (int i = 0; i < MAX_GHOSTS; ++i)
  {
    ghosts[i].location = uVec2(BAD_LOC, BAD_LOC);
    setState(i, GHOST_DEAD);
  }
}

void spawnGhosts(unsigned int no)
{
  for (int i = 0; i < no; ++i)
  {
    uVec2 newLoc;
    unsigned char type;
    getGhostSpawn(newLoc, type);      // watch out, newLoc and type are altered by this function!        
    if (newLoc.isValid())
    {         
      for (int i = 0; i < MAX_GHOSTS; ++i)
      {
        if (ghosts[i].state == GHOST_DEAD)
        {
          ghosts[i].location = newLoc;
          switch (type)
          {
            case GHOST_SPAWN_UD :           setState(i, GHOST_GROW_UD); break;
            case GHOST_SPAWN_LR :           setState(i, GHOST_GROW_LR); break;
          }
          break;
        }
      }
    }
  }
}

unsigned long lastGhostUpdate = 0;
unsigned long ghostUpdateInterval = GHOST_UPDATE_OFFSET;
void updateGhosts()
{
  if (millis() - lastGhostUpdate < ghostUpdateInterval)
    return;
  lastGhostUpdate = millis();

  for (int i = 0; i < MAX_GHOSTS; ++i)
  {
    switch (ghosts[i].state)
    {
      case GHOST_DEAD: break;   // ghost dead or no ghost do nothing
      case GHOST_PATROL_UP:
      case GHOST_PATROL_DOWN:
      case GHOST_PATROL_LEFT:
      case GHOST_PATROL_RIGHT:
        if (ghosts[i].countDown == 0)
          setState(i, GHOST_CHASE);
        else
          doPatrol(i);
        ghosts[i].countDown--;
        break;
      case GHOST_CHASE:
        if (ghosts[i].countDown == 0)
          setState(i, random(0, 2) == 0 ? GHOST_PATROL_UP : GHOST_PATROL_LEFT);
        else
          doChase(i);
        ghosts[i].countDown--;

        break;
      case GHOST_GROW_LR:
        if (ghosts[i].countDown == 0)
          setState(i, GHOST_PATROL_LEFT);
        ghosts[i].countDown--;
        break;
      case GHOST_GROW_UD:
        if (ghosts[i].countDown == 0)
          setState(i, GHOST_PATROL_UP);
        ghosts[i].countDown--;
        break;
      case GHOST_DIEING:
      default: Serial.println("Invalid Ghost state");
    }
  }
}

void doChase(unsigned int i)
{
  uVec2 ownLoc = ghosts[i].location;
  uVec2 pLoc = getPlayerLocation();
  Vec2 dir(pLoc.x - ownLoc.x, pLoc.y - ownLoc.y);
  if (abs(dir.x) > abs(dir.y))      // if player is further away horizontally move left right, otherwise move up down
  {
    if (dir.x < 0)
      ownLoc.x--;
    else if (dir.x > 0)
      ownLoc.x++;
  } else {
    if (dir.y < 0)
      ownLoc.y--;
    else if (dir.y > 0)
      ownLoc.y++;
  }

  if (ownLoc.isValid() && !isObstacle(ownLoc) && !isGhost(ownLoc))
    ghosts[i].location = ownLoc;
  /*else   // move sideways if forward is blocked.
    {
    uVec2 ownLoc = ghosts[i].location;
    if (abs(dir.x) > abs(dir.y))
    {
      if (random(0, 2) == 0)
      {
        ownLoc.y --;
      } else {
        ownLoc.y++;
      }
    } else {
      if (random(0, 2) == 0)
      {
        ownLoc.x --;
      } else {
        ownLoc.x++;
      }
    }
    if (ownLoc.isValid() && !isObstacle(ownLoc) && !isGhost(ownLoc))
      ghosts[i].location = ownLoc;
    } */

}

void doPatrol(unsigned int i)
{
  uVec2 ownLoc = ghosts[i].location;
  switch (ghosts[i].state)
  {
    case GHOST_PATROL_UP :
      ownLoc.y--;
      break;
    case GHOST_PATROL_DOWN  :
      ownLoc.y++;
      break;
    case GHOST_PATROL_LEFT :
      ownLoc.x--;
      break;
    case GHOST_PATROL_RIGHT :
      ownLoc.x++;
      break;
    default:
      Serial.println("Not a patrol state!");
  }

  if (ownLoc.isValid() && !isObstacle(ownLoc)  && !isGhost(ownLoc))
    ghosts[i].location = ownLoc;
  else if (random(0, 5) == 0) // randomly change direction
  {
    switch (ghosts[i].state)
    {
      case GHOST_PATROL_UP:
        ghosts[i].state = GHOST_PATROL_LEFT;
        break;
      case GHOST_PATROL_DOWN:
        ghosts[i].state = GHOST_PATROL_RIGHT;
        break;
      case GHOST_PATROL_LEFT:
        ghosts[i].state = GHOST_PATROL_UP;
        break;
      case GHOST_PATROL_RIGHT:
        ghosts[i].state = GHOST_PATROL_RIGHT;
        break;
    }
  } else { // simply return the way you came
    switch (ghosts[i].state)
    {
      case GHOST_PATROL_UP:
        ghosts[i].state = GHOST_PATROL_DOWN;
        break;
      case GHOST_PATROL_DOWN:
        ghosts[i].state = GHOST_PATROL_UP;
        break;
      case GHOST_PATROL_LEFT:
        ghosts[i].state = GHOST_PATROL_RIGHT;
        break;
      case GHOST_PATROL_RIGHT:
        ghosts[i].state = GHOST_PATROL_LEFT;
        break;
    }
  }
}

void renderGhosts()
{
  for (int i = 0; i < MAX_GHOSTS; ++i)
  {
    uVec2 loc = ghosts[i].location;
    switch (ghosts[i].state)
    {
      case GHOST_DEAD :
        break;
      case GHOST_GROW_LR :
      case GHOST_GROW_UD :
        {
          unsigned int count = ghosts[i].countDown;
          count = GHOST_GROW_TIME - count;
          count = (count * 4) / GHOST_GROW_TIME;
          AberLED.set(loc.x, loc.y, 0, tickCount % 2 ? count : 0);
        }
        break;
      case GHOST_PATROL_UP :
      case GHOST_PATROL_DOWN :
      case GHOST_PATROL_LEFT :
      case GHOST_PATROL_RIGHT :
        if (ghosts[i].countDown > GHOST_ANGRY)
        {
          AberLED.set(loc.x, loc.y, 0, ((tickCount / 20) % 2) + 2);
          break;
        } else {
          AberLED.set(loc.x, loc.y, 0, ((tickCount / 2) % 2) == 0 ? 3 : 0);
          break;
        }
      case GHOST_CHASE :
        AberLED.set(loc.x, loc.y, 0, ((tickCount) % 2) == 0 ? 3 : 0);
        break;
      case GHOST_DIEING :
        AberLED.set(loc.x, loc.y, 0, 2);
        break;
    }
  }
}

bool setState(unsigned int i, unsigned int s)
{
  ghosts[i].state = s;
  switch (s)
  {
    case GHOST_GROW_LR  :
    case GHOST_GROW_UD  :
      ghosts[i].countDown =   GHOST_GROW_TIME;
      break;
    case GHOST_PATROL_UP :
    case GHOST_PATROL_DOWN  :
    case GHOST_PATROL_LEFT :
    case GHOST_PATROL_RIGHT :
      ghosts[i].countDown = GHOST_PATROL_TIME;
      break;
    case GHOST_CHASE :
      ghosts[i].countDown = GHOST_CHASE_TIME;
      break;
    case GHOST_DIEING :
      ghosts[i].countDown = GHOST_DIEING_TIME;
      break;
  }
}
void setGhostLevel(unsigned int level)
{
  ghostLevel = 0;
  ghostUpdateInterval = GHOST_UPDATE_OFFSET - GHOST_UPDATE_FACTOR * ghostLevel;
  ghostUpdateInterval = max(ghostUpdateInterval, GHOST_UPDATE_MIN);
}

bool isGhost(const uVec2 &loc)
{
  for (int i = 0; i < MAX_GHOSTS; ++i)
    if (loc.x == ghosts[i].location.x && loc.y == ghosts[i].location.y)
      return true;
  return false;
}

void angerGhosts(unsigned int amount)
{
  for (int i = 0; i < MAX_GHOSTS; ++i)
    switch (ghosts[i].state)
    {
      case GHOST_PATROL_UP :
      case GHOST_PATROL_DOWN  :
      case GHOST_PATROL_LEFT :
      case GHOST_PATROL_RIGHT :
        {
          ghosts[i].countDown -= amount;
          int temp = (int) ghosts[i].countDown - (int) amount; // avoid negatives
          temp = max(GHOST_ANGRY, temp);  // no smaller than GHOST_ANGRY
          ghosts[i].countDown = temp;
        }
        break;
      case GHOST_CHASE :
        {
          ghosts[i].countDown += amount;
          int temp = (int) ghosts[i].countDown - (int) amount; // avoid negatives
          temp = min(GHOST_CHASE_TIME, temp);  // no longer than patrol time to avoid rediculous anger
          ghosts[i].countDown = temp;
        }
        break;
    }
}

