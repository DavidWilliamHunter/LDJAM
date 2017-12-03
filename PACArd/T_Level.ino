/*
 * The levels
 */
//

char currentLevel[8][9];


#ifdef USE_EEPROM
// read current level from EEPROM
void setCurrentLevel(unsigned int level)
{
  int noLevels = EEPROM.read(0);
  int l = level%noLevels;
    for(int i=0;i<8;++i)
      for(int j=0;j<8;++j)
        currentLevel[i][j] = (char) EEPROM.read(l*64 + i*8 + j + 1);    // get from EEPROM storage
}

#else
#define NO_LEVELS 6


const char levels[NO_LEVELS][8][9] = {{
"########",
"########",
"########",
"#P.....#",
"########",
"########",
"########",
"########" 
},{
"########",
"#.....P#",
"#.##/#.#",
"#.##.#.#",
"#.##.#.#",
"#.##/#.#",
"#G.....#",
"########"
},{
"########",
"#P     #",
"#.#### #",
"# #G.# #",
"# #..# #",
"# #//# #",
"#      #",
"########" 
},{
"########",
"#.....G#",
"#/####/#",
"#      #",
"#      #",
"#/####/#",
"#.....G#",
"########"
},                         { "########",
                            "#......#",
                            "#.#GH#.#",
                            "#/####.#",
                            "#.####/#",                            
                            "#.#PP#.#",
                            "#......#",
                            "########"}
,{
"########",
"#.    G#",
"#      #",                            
"#      #",                            
"#      #",                            
"#      #",                            
"#P    .#",                            
"########",                            
}};
                            
const char level1[8][9] = { "########",
                            "#......#",
                            "#.#GH#.#",
                            "#/####.#",
                            "#.####/#",                            
                            "#.#PP#.#",
                            "#......#",
                            "########"};


void setCurrentLevel(unsigned int level)
{
  for(int i=0; i<8; ++i)
    for(int j=0; j<8; ++j)
      currentLevel[i][j] = levels[level%NO_LEVELS][i][j];
}
#endif

bool isFood(unsigned char x, unsigned char y)
{
  return currentLevel[x][y]==FOOD;
}

bool isLevelObstacle(unsigned char x, unsigned y, bool doorClosed)
{
    switch(currentLevel[x][y])
    {
      case WALL:  return true;
      case DOOR:  return doorClosed;
      default: return false;
    }
}

bool isLevelObstacle(uVec2 loc, bool doorClosed)
{
    return isLevelObstacle(loc.x, loc.y, doorClosed);
}

void renderLevel(bool doorClosed)
{
  for(int x=0; x<8 ;++x)
    for(int y=0; y<8; ++y)
      switch(currentLevel[x][y])
      {
        case WALL :   AberLED.set(x,y,RED); break;    // walls are full red
        case FOOD :   AberLED.set(x,y,1,1); break;    // food is pale orange
        case DOOR :   if(doorClosed) { AberLED.set(x,y,RED);} break;
      }                                  
}

uVec2 getPlayerSpawn()
{
  for(int i=0; i<8; ++i)
    for(int j=0; j<8; ++j)
      if(currentLevel[i][j]==PLAYER_SPAWN)
        return uVec2(i,j);
  return uVec2(BAD_LOC,BAD_LOC);        
}

void getGhostSpawn(uVec2 &loc, unsigned char &type)
{
  for(int i=0; i<8; ++i)
    for(int j=0; j<8; ++j)
      if(currentLevel[i][j]==GHOST_SPAWN_UD || currentLevel[i][j]==GHOST_SPAWN_LR)
      {
        loc.x = i; loc.y=j;
        type = currentLevel[i][j];
        currentLevel[i][j] = SPACE; // this ghost has spawned so erase its spawn location        
        return;
      }
        
  loc = uVec2(BAD_LOC,BAD_LOC);        
}

bool isFoodPlusClear(uVec2 &loc)
{
  if(currentLevel[loc.x][loc.y]==FOOD)
  {
    currentLevel[loc.x][loc.y]=SPACE;
    return true;
  }
  return false;
}

unsigned char foodRemainingInLevel()
{
  unsigned char count = 0;
  for(int i=0; i<8; ++i)
    for(int j=0; j<8;++j)
      if(currentLevel[i][j]==FOOD)
        count++;
  return count;        
}

