/**
 * @file AberLED.cpp
 * @brief Implementation of the bicolor LED library.
 * @author Jim Finnis (jcf1@aber.ac.uk)
 * @version 2.6
 * @date 7 December 2015
 * @copyright Aberstwyth University
 * 
 * This file implements the AberLED class. Stylistically it's somewhat horrific,
 * using global variables all over the place. This is because (a) it keeps
 * implementation details (private variables) out of the include file, and (b)
 * the class is always used as a singleton.
 */

#include "AberLED2.h"

//
// Altered Libary to handle dimming of LEDs by pulse width modulation.


// pin mappings - the actual refresh code uses direct
// port manipulation, so YOU MUST CHANGE THAT TOO IF YOU
// CHANGE THESE!


// pins for the row shift register
#define RDATA 2
#define RLATCH 3
#define RCLOCK 4

// pins for the column shift registers
#define CDATA 5
#define CCLOCK 6
#define CLATCH 7

int UP,DOWN,LEFT,RIGHT,FIRE;


// these are the two buffers.
// altered to store 4 bits per colour per LED
static uint32_t bufferA[8];
static uint32_t bufferB[8];

// these are pointers to the back and front buffers
static uint32_t *backBuffer;
static uint32_t *frontBuffer;

// button variables
static byte buttonStates[]={0,0,0,0,0}; // set in swap()
static byte debouncedButtonStates[]={0,0,0,0,0}; // set in interrupt
static byte buttonDebounceCounters[]={0,0,0,0,0};
static byte buttonWentDown[] = {0,0,0,0,0};
static byte buttonWentDownInLastLoop[] = {0,0,0,0,0};
static int boardRev=-1;

int ticks=0;
bool interruptRunning=false;
volatile int interruptTicks=0;

AberLEDClass AberLED;

void AberLEDClass::setRevision(int rev){
    boardRev = rev;
    if(rev == REV01){
        UP = 1;
        DOWN = 2;
        LEFT = 4;
        RIGHT = 3;
        FIRE = 5;
    } else {
        UP = 2;
        DOWN = 4;
        LEFT = 1;
        RIGHT = 3;
        FIRE = 5;
    }
}


int AberLEDClass::getTicks(){
    return ticks;
}

// these are faster routines for doing the shift register writes

static void fastShiftOutRows(byte n){
    PORTD &= ~(1<<2); // data off
    for(int i=7;i>=0;i--){
        PORTD &= ~(1<<4); // clock off
        if(n & (1<<i))
            PORTD |= 1<<2; // data on if true
        else
            PORTD &= ~(1<<2); // data off if true
        PORTD |= 1<<4; // clock on
        
        PORTD &= ~(1<<2); // data off (to prevent bleed through)
    }
    PORTD &= ~(1<<4); // clock off
}

static void fastShiftOutCols(uint16_t n){
    PORTD &= ~(1<<5); // data off
    for(int i=15;i>=0;--i){
        PORTD &= ~(1<<6); // clock off
        if(n & (1<<i))
            PORTD |= 1<<5; // data on if true
        else
            PORTD &= ~(1<<5); // data off if true
        PORTD |= 1<<6; // clock on
        PORTD &= ~(1<<5); // data off (to prevent bleed through)
        
    }
    PORTD &= ~(1<<6); // clock off
}


static void setupInterrupt();


void AberLEDClass::begin(bool interrupt) {
    setRevision(REV01);
    // set all the shift register pins to output
    pinMode(CLATCH,OUTPUT);
    pinMode(CDATA,OUTPUT);
    pinMode(CCLOCK,OUTPUT);
    pinMode(RLATCH,OUTPUT);
    pinMode(RDATA,OUTPUT);
    pinMode(RCLOCK,OUTPUT);
    
    // set up the switch inputs
    pinMode(A0,INPUT_PULLUP);
    pinMode(A1,INPUT_PULLUP);
    pinMode(A2,INPUT_PULLUP);
    pinMode(A3,INPUT_PULLUP);
    pinMode(9,INPUT_PULLUP);
    // and the default LED
    pinMode(13,OUTPUT);
    
    // clear the SRs
    
    digitalWrite(RLATCH,LOW);
    fastShiftOutRows(0);
    digitalWrite(RLATCH,HIGH);
    
    digitalWrite(CLATCH,LOW);
    fastShiftOutCols(0);
    digitalWrite(CLATCH,HIGH);
    
    // set up the initial buffers and clear them
    
    backBuffer = bufferA;
    frontBuffer = bufferB;
    
    memset(backBuffer,0,16);
    memset(frontBuffer,0,16);
    
    if(interrupt)
        setupInterrupt();
}





// The user calls this code when they have finished writing to
// the back buffer. It swaps the back and front buffer, so that
// the newly written buffer becomes the front buffer and is
// displayed. This is done by swapping the pointers, not the data.
// Interrupts are disabled to avoid the "tearing" effect produced
// by the buffers being swapped during redraw.

void AberLEDClass::swap(){
    uint32_t *t;
    
    cli();
    for(int i=0;i<5;i++){
        buttonWentDownInLastLoop[i] = buttonWentDown[i];
        buttonWentDown[i]=0;
        buttonStates[i]=debouncedButtonStates[i];
    }
    
    ticks=interruptTicks;
    interruptTicks=0;
    
    t = frontBuffer;
    frontBuffer = backBuffer;
    backBuffer = t;
    
    sei();
    if(interruptRunning){
        while(interruptTicks<2){}
    }
}

int AberLEDClass::getButton(unsigned char c){
    return buttonStates[c-1];
}

int AberLEDClass::getButtonDown(unsigned char c){
    return buttonWentDownInLastLoop[c-1];
}



// The user calls this before writing to the back buffer, to
// get its pointer to write to.

uint32_t *AberLEDClass::getBuffer(){
    return backBuffer;
}

// set a pixel in the back buffer to a given colour 
void AberLEDClass::set(int x, int y, unsigned char col){
    set(x,y,(col & RED)? MAX_INTENSITY:0, (col & GREEN)? MAX_INTENSITY:0);
}

void AberLEDClass::set(int x, int y, unsigned char colR, unsigned char colG){
    if(x<8 && y<8 && x>=0 && y>=0 && colR<=MAX_INTENSITY && colG<=MAX_INTENSITY) { // check we're in range
        uint32_t *p = backBuffer+y; // get row pointer
        x *= 4; // double x to get the column bit index  (DAH - altered for new intensity range)
        // clear the bits first
        *p &= ~((uint32_t) 15<<x);
        // then set the new colour
        *p |= ((uint32_t) colR)<<x;
		*p |= ((uint32_t) colG)<<(x+2);
    }
}

void AberLEDClass::setRed(int x, int y, unsigned char colR)
{
    if(x<8 && y<8 && x>=0 && y>=0 && colR<=MAX_INTENSITY) { // check we're in range
        uint32_t *p = backBuffer+y; // get row pointer
        x *= 4; // double x to get the column bit index  (DAH - altered for new intensity range)
        // clear the bits first
        *p &= ~((uint32_t) 3<<x);
        // then set the new colour
        *p |= ((uint32_t) colR)<<x;
    }
}

void AberLEDClass::setRedBlend(int x, int y, unsigned char colR)
{
    if(x<8 && y<8 && x>=0 && y>=0 && colR<=MAX_INTENSITY) { // check we're in range
        uint32_t *p = backBuffer+y; // get row pointer
        x *= 4; // double x to get the column bit index  (DAH - altered for new intensity range)
		
		
        unsigned int red = (*p & (uint32_t) 3<<x)>>x;
		red = min((red+colR),3);
		
        // then set the new colour
        *p &= ~((uint32_t) 3<<x);		
        *p |= ((uint32_t) red)<<x;
    }
}

unsigned char AberLEDClass::getRed(int x, int y)
{
    if(x<8 && y<8 && x>=0 && y>=0) { // check we're in range
        const uint32_t *p = backBuffer+y; // get row pointer
        x *= 4; // double x to get the column bit index  (DAH - altered for new intensity range)
		
		
        unsigned int red = (*p & (uint32_t) 3<<x)>>x;
		
		return red;
    }
	return 0;
}	
	
	

void AberLEDClass::setGreen(int x, int y, unsigned char colG)
{
    if(x<8 && y<8 && x>=0 && y>=0 && colG<=MAX_INTENSITY) { // check we're in range
        uint32_t *p = backBuffer+y; // get row pointer
        x *= 4; // double x to get the column bit index  (DAH - altered for new intensity range)
		x += 2;
        // clear the bits first
        *p &= ~((uint32_t) 3<<x);
        // then set the new colour
		*p |= ((uint32_t) colG)<<x;
    }
}


void AberLEDClass::setGreenBlend(int x, int y, unsigned char colG)
{
    if(x<8 && y<8 && x>=0 && y>=0 && colG<=MAX_INTENSITY) { // check we're in range
        uint32_t *p = backBuffer+y; // get row pointer
        x *= 4; // double x to get the column bit index  (DAH - altered for new intensity range)
		x += 2;
        unsigned int green = (*p & (uint32_t) 3<<x)>>x;
		green = min((green+colG),4);		
        // clear the bits first
        *p &= ~((uint32_t) 3<<x);
        // then set the new colour
		*p |= ((uint32_t) green)<<x;
    }
}

unsigned char AberLEDClass::getGreen(int x, int y)
{
    if(x<8 && y<8 && x>=0 && y>=0) { // check we're in range
        uint32_t *p = backBuffer+y; // get row pointer
        x *= 4; // double x to get the column bit index  (DAH - altered for new intensity range)
		x += 2;
        unsigned int green = (*p & (uint32_t) 3<<x)>>x;
		return green;
    }
	return 0;
}
// sets the entire back buffer to zero

void AberLEDClass::clear(){
    memset(backBuffer,0,32);
}

//uint16_t PWM[16] = {0, 1, 129,1092,2185,18724,37449,42277,43690,43691,43863, 32752,60783,61167,63423, 65535};
bool PWM[4][4] = {{0,0,0,0},{1,0,0,0},{1,0,1,0},{1,1,1,1}};

// calculate the pulse-width moduled LED strobes
uint16_t getStrobe(uint32_t levels, unsigned int t)
{
	uint16_t ret = 0;
	unsigned int x;
	for(int i=0; i<8; ++i)
	{
		x = i*4;
		unsigned char red = (levels>>x)& 3;
		unsigned char green = (levels>>(x+2))& 3;
		//ret |= (green > t ? 1 : 0) << i*2;
		//ret |= (red > t ? 1 : 0) << (i*2+1);
		//ret |= (PWM[green]>>t & 1)  << i*2;
		//ret |= (PWM[red]>>t & 1)  << (i*2+1);
		ret |= (PWM[green][t])  << i*2;
		ret |= (PWM[red][t])  << (i*2+1);
	}
	return ret;
}

// called periodically by the interrupt, this writes the front
// buffer data out to the screen. The front buffer is the one
// which is not being drawn to.
// We use port manipulation here for speed!

static int refrow =0;
unsigned int blink_count = 0;

inline void refreshNextRow(){
    if(!refrow)
        PORTD |= 1<<2; // turn on the row data line to get the first bit set
    
    // set latches low
    PORTD &= ~( (1<<3) | (1<<7) );
    
    // tick the row clock to move the next bit in (high on
    // the first row, low after that)
    PORTD |= (1<<4);
    PORTD &= ~(1<<4);
    
    // and turn off the row data line
    PORTD &= ~(1<<2);
    
		// get the pulse width modulated strobes
		
		//getStrobe(frontBuffer[refrow], blink_count);
		uint16_t leds = getStrobe(frontBuffer[refrow], blink_count);
	
		// now the appropriate row is set high, set the column
		// bits low for the pixels we want.
    
		fastShiftOutCols(~(leds));

    // and latch the registers
    
    PORTD |= ( (1<<3) | (1<<7) );
    
    //refrow = (refrow+1)%8;
	refrow ++;
	if(refrow>=8)
	{
		blink_count = (blink_count+1)%4;
		refrow = 0;
	}
}


void AberLEDClass::refresh(){
    refrow = 0;
    refreshNextRow();
    refreshNextRow();
    refreshNextRow();
    refreshNextRow();
    refreshNextRow();
    refreshNextRow();
    refreshNextRow();
    refreshNextRow();
    // hold the last line for a little while
    for(int volatile i=0;i<30;i++){
        __asm__ __volatile__ ("nop\n\t");
    }
    
    //    // latch off values into the columns, to avoid last row bright.
    PORTD &= ~( (1<<3) | (1<<7) );
    fastShiftOutCols(0xffff);
    PORTD |= ( (1<<3) | (1<<7) );
	
}    

// this is the interrupt service routine for the timer interrupt

ISR(TIMER1_COMPA_vect){
    interruptTicks++;
    
    // draw the next row
    refreshNextRow();
    
    static byte trueButtonStates[] = {0,0,0,0,0};
    static byte button = 0;
    // and process the next button
    
    byte bstate;
    if(boardRev == REV00){
        switch(button){
        case 0:bstate = PINC&2;break;
        case 1:bstate = PINC&1;break;
        case 2:bstate = PINC&4;break;
        case 3:bstate = PINC&8;break;
        case 4:bstate = PINB&2;break;
        }
    } else {
        switch(button){
        case 0:bstate = PINC&1;break;
        case 1:bstate = PINC&8;break;
        case 2:bstate = PINC&4;break;
        case 3:bstate = PINC&2;break;
        case 4:bstate = PINB&2;break;
        }
    }
    
    bstate = bstate?0:1;// make booleanesque and invert
    
    if(bstate != trueButtonStates[button]){
        buttonDebounceCounters[button]=0;
        if(bstate)
            buttonWentDown[button]=1;
    }
    else if(++buttonDebounceCounters[button]==4){
        buttonDebounceCounters[button]=0;
        debouncedButtonStates[button] = bstate;
    }
    trueButtonStates[button]=bstate;
    
    button = (button+1)%5;
}

// Set up a 1kHz interrupt handler - the code for the interrupt
// is in the TIMER1_COMPA_vect() function.

static void setupInterrupt(){
    cli(); // disable all interrupts
    
    // some very hardware-specific code, setting registers
    // inside the ATMega328p processor.
    
    //set timer1 interrupt at 500Hz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for correct frequency 
    // (16*10^6) / (500*8) - 1 gives 3999 (for 500Hz)
    //OCR1A = 3999;
	OCR1A = 999;
    
    
    
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12 and CS10 bits for 8 prescaler
    TCCR1B |= (1 << CS11);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    interruptRunning = true;
    
    sei(); // re-enable interrupts
}

