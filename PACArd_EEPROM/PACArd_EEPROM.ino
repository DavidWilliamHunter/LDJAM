#include <EEPROM.h>
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
"#P     #",
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

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  EEPROM.update(0,NO_LEVELS);
  for(int l=0; l<NO_LEVELS;++l)
    for(int i=0;i<8;++i)
      for(int j=0;j<8;++j)
        EEPROM.update(l*64 + i*8 + j + 1, levels[l][i][j]);

  int noLevels = EEPROM.read(0);
  Serial.println(noLevels);
  for(int l=0; l<NO_LEVELS;++l)
  {
    for(int i=0;i<8;++i)
    {
      for(int j=0;j<8;++j)
        Serial.print((char) EEPROM.read(l*64 + i*8 + j + 1));
      Serial.println("");
    }
    Serial.println("***");
  }        
}

void loop() {
  // put your main code here, to run repeatedly:

}
