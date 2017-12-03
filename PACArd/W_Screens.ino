


const char redDead[8][9] = { "...22...",
                             "..0330..",
                             "..2002..",
                             ".030030.",
                             ".20..020",
                             ".3....30",
                             "2.....02",
                             "33333333"
                           };

const  char greenDead[8][9] = { "........",
                                "........",
                                "...33...",
                                "...33...",
                                "...33...",
                                "........",
                                "...33...",
                                "........"
                              };

const  char redWin[8][9] = {  "........",
                              "........",
                              "........",
                              "........",
                              "........",
                              "........",
                              "........",
                              "........"
                           };

const  char greenWin[8][9] = { "........",
                               ".......3",
                               "......33",
                               ".....33.",
                               "3...33..",
                               "33.33...",
                               ".333....",
                               "..3....."
                             };

const  char redStart[3][8][9] = {{
    ".3333...",
    "333333..",
    "3332....",
    "333..3.3",
    "3332....",
    "333333..",
    ".3333...",
    "......."
  }
  , {
    ".3333...",
    "333333..",
    "333322..",
    "33323.3.",
    "333322..",
    "333333..",
    ".3333...",
    "......."
  }
  , {
    ".3333...",
    "333333..",
    "333333..",
    "333333..",
    "333333..",
    "333333..",
    ".3333...",
    "......."
  }
};

const  char greenStart[3][8][9] =  {{
    ".3333...",
    "330333..",
    "3332....",
    "333..3.3",
    "3332....",
    "333333..",
    ".3333...",
    "......."
  }
  , {
    ".3333...",
    "330333..",
    "333322..",
    "33323.3.",
    "333322..",
    "333333..",
    ".3333...",
    "......."
  }
  , {
    ".3333...",
    "330333..",
    "333333..",
    "333333..",
    "333333..",
    "333333..",
    ".3333...",
    "......."
  }
};


void displayScreen(const  char red[8][9], const  char green[8][9])
{
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
    {
      unsigned char r = 0;
      unsigned char g = 0;
      if (red[i][j] >= 48 && red[i][j] < 52)
        r = red[i][j] - 48;
      if (green[i][j] >= 48 && green[i][j] < 52)
        g = green[i][j] - 48;

      AberLED.set(j, i, r, g);
    }
}

void displayDeathScreen()
{
  displayScreen(redDead, greenDead);
}

void displayWinScreen()
{
  displayScreen(redWin, greenWin);
}

void displayStartScreen()
{
  unsigned char offset = tickCount % 3;
  displayScreen(redStart[offset], greenStart[offset]);
}

void getImageBuffer(unsigned char red[8][9], unsigned char green[8][9])
{
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
    {
      red[i][j] = AberLED.getRed(i, j);
      green[i][j] = AberLED.getGreen(i, j);

    }
}

void displayScreen(unsigned char red[8][9], unsigned char green[8][9])
{
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      AberLED.set(i, j, red[i][j], green[i][j]);
}

void getFillScreen(unsigned char red[8][9], unsigned char green[8][9], unsigned char r, unsigned char g )
{
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
    {
      red[i][j] = r;
      green [i][j] = g;
    }
}

void screenWipe(unsigned char redIn[8][9], unsigned char greenIn[8][9], unsigned char redOut[8][9], unsigned char greenOut[8][9], unsigned char wipeType, unsigned int offset)
{
  switch (wipeType)
  {
    case SCREEN_WIPE_TILES :
      {
        unsigned int tileNo = offset / 4;
        unsigned int tileOff = offset % 4;
        float tileF = (float) tileOff / 4.0;
        for (int i = 0; i < 8; ++i)
        {
          int i2 = i / 2;
          for (int j = 0; j < 8; ++j)
          {
            int j2 = j / 2;
            switch (tileNo)
            {
              case 5:
              case 4:
                if (i2 % 2 == 0 && j2 % 2 == 0)
                  AberLED.set(i, j, redOut[i][j], greenOut[i][j]);
              case 3:
                if (i2 % 2 == 1 && j2 % 2 == 0)
                  AberLED.set(i, j, redOut[i][j], greenOut[i][j]);
              case 2:
                if (i2 % 2 == 0 && j2 % 2 == 1)
                  AberLED.set(i, j, redOut[i][j], greenOut[i][j]);
              case 1:
                if (i2 % 2 == 1 && j2 % 2 == 1)
                  AberLED.set(i, j, redOut[i][j], greenOut[i][j]);
            }
            switch (tileNo)
            {
              case 4:
              case 3:
                if (i2 % 2 == 0 && j2 % 2 == 0)
                  AberLED.set(i, j, (float) redIn[i][j] * (1.0f - tileF) + (float) redOut[i][j]*tileF, (float) greenIn[i][j] * (1.0f - tileF) + (float) greenOut[i][j]*tileF);
                break;
              case 2:
                if (i2 % 2 == 1 && j2 % 2 == 0)
                  AberLED.set(i, j, (float) redIn[i][j] * (1.0f - tileF) + (float) redOut[i][j]*tileF, (float) greenIn[i][j] * (1.0f - tileF) + (float) greenOut[i][j]*tileF);
                break;
              case 1:
                if (i2 % 2 == 0 && j2 % 2 == 1)
                  AberLED.set(i, j, (float) redIn[i][j] * (1.0f - tileF) + (float) redOut[i][j]*tileF, (float) greenIn[i][j] * (1.0f - tileF) + (float) greenOut[i][j]*tileF);
                break;
              case 0:
                if (i2 % 2 == 1 && j2 % 2 == 1)
                  AberLED.set(i, j, (float) redIn[i][j] * (1.0f - tileF) + (float) redOut[i][j]*tileF, (float) greenIn[i][j] * (1.0f - tileF) + (float) greenOut[i][j]*tileF);
                break;
            }
          }
        }
      }
  }
}

void doTransition(unsigned int state)
{
  unsigned char redOut[8][9];
  unsigned char greenOut[8][9];
  getImageBuffer(redOut, greenOut);
  unsigned char redIn[8][9];
  unsigned char greenIn[8][9];
  switch (state)
  {
    case GAME_STATE_START :
      displayStartScreen();
      getImageBuffer(redIn, greenIn);
      break;
    case GAME_STATE_PLAYING :
      return;
    case GAME_STATE_WIN :
      displayWinScreen();
      getImageBuffer(redIn, greenIn);
      break;
    case GAME_STATE_LOSE :
      displayDeathScreen();
      getImageBuffer(redIn, greenIn);
      break;
    default:
      return;
  }

  for (int i = 0; i < 16; ++i)
  {
    AberLED.clear();
    displayScreen(redOut,greenOut);
    screenWipe(redOut, greenOut, redIn, greenIn, SCREEN_WIPE_TILES, i);
    AberLED.swap();
    delay(5);
  }
}

