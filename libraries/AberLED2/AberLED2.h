/**
 * @file AberLED.h
 * @author Jim Finnis (jcf1@aber.ac.uk)
 * @version 2.6
 * @date 7 December 2015
 * @copyright 2014-2015 Jim Finnis, Aberstwyth University. Licenced under the MIT license (see below).
 * @brief The declaration for the AberLED class.
 * Many implementation details are inside the AberLED implementation file
 * AberLED.cpp. Note that there must be only one of these objects, but there's
 * no code to enforce this. Weird things will happen if you have
 * more than one.
 * 
 */

#ifndef __ABERLED_H
#define __ABERLED_H

#include "Arduino.h"

/// the "off" colour for pixels, used in set()
#define BLACK 0
/// the green colour for pixels, used in set()
#define GREEN 1
/// the red colour for pixels, used in set()
#define RED 2
/// the yellow colour for pixels, used in set()
#define YELLOW 3

///////////////// board revision codes

// beige board
#define REV00 0
// black board
#define REV01 1



/// the number for button S1, the "up" button
extern int UP;
/// the number for button S2, the "down" button
extern int DOWN;
/// the number for button S3, the "right" button
extern int LEFT;
/// the number for button S4, the "left" button
extern int RIGHT;
/// the number for button S5, the "action" or "fire" button
extern int FIRE;

#define MAX_INTENSITY 3
/// The class for the AberLED shield. One object of this class, called
/// **AberLED**, is automatically created (similar to how Serial works).
/// Yes, all this time you've been writing C++, not C. You should put
/// `AberLED.` before every function, so `AberLED.begin()` instead
/// of just `begin()`.
///
/// 
/// Calling begin() will initialise the LED matrix , setting all the pins to
/// the appropriate mode and setting up the interrupt handler.
/// Thereafter, every nth of a second the Arduino will automatically
/// send the front buffer to the matrix.
/// Calling getBuffer() will give a pointer to the back buffer, which
/// can be be written to. Alternatively, pixels can be written by calling
/// set() with x,y coordinates and a colour (GREEN, RED, YELLOW or BLACK).
/// Once writing is complete, calling swap() will
/// exchange the front and back buffers, so that the Arduino will
/// now write the new front buffer to the matrix every interrupt tick.
///
/// The format of the buffers is 8 16-bit integers: unsigned shorts.
/// Each integer represents a single row. Each pixel - individual LED -
/// is represented by 2 adjacent bits with the most significant bit
/// being red and the least significant bit being green.

class AberLEDClass {
public:
    /// Initialises all pin modes, clears the buffers,
    /// starts the interrupt and begins to output data to the LED.
    /// If interrupt is false, the interrupt is not used and
    /// the display must be refreshed manually by calling
    /// refresh() frequently!
    void begin(bool interrupt=true);
    
    /// The user calls this code when they have finished writing to
    /// the back buffer. It swaps the back and front buffer, so that
    /// the newly written buffer becomes the front buffer and is
    /// displayed.

    void swap();
    
    /// This sets the given pixel in the front buffer to the given value.
    /// The pixel values are 0 (off), 1 (green), 2 (red) or 3 (yellow),
    /// but the macros BLACK, GREEN, RED and YELLOW should be used.
    void set(int x, int y, unsigned char colR);
	
    void set(int x, int y, unsigned char colR, unsigned char colG);
	
	void setRed(int x, int y, unsigned char colR);
	
	void setGreen(int x, int y, unsigned char colG);
	
	void setRedBlend(int x, int y, unsigned char colR);

	void setGreenBlend(int x, int y, unsigned char colG);

	unsigned char getRed(int x, int y);
	
	unsigned char getGreen(int x, int y);
    
    /// set all pixels in the back buffer to black
    
    void clear();
    
    /// return nonzero if a given switch is pressed - switches are numbered
    /// 1 to 5, which is against my Golden Rule of computers numbering
    /// things from zero, but that's how they're labelled on the physical
    /// board.
    
    int getButton(unsigned char c);
    
    /// return nonzero if the button has been pressed since the
    /// last swap()
    
    int getButtonDown(unsigned char c);

    /// The user can call this to write to the back buffer directly.
    /// This returns a pointer to the buffer: a set of 8 16-bit ints,
    /// each pair of bits representing a pixel.

    uint32_t *getBuffer();
    
    /// return the number of interrupt ticks which occurred in the
    /// last swap()-swap() cycle
    int getTicks();
    
    /// use only when interrupts are disabled - copies the front
    /// buffer to the display
    static void refresh();
    
    /// call this to set the revision, after AberLED.begin() (which
    /// will set it to REV01 by default)
    static void setRevision(int rev);
};

/// this is the single instance of the LED class - for documentation see AberLEDClass.

extern AberLEDClass AberLED;


#endif /* __ABERLED_H */
