//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//--------------------------------------------------------------------------//
//	     Made by: Muhammad Faisal Nisar Choudhry                        //
//	     student of, FAST Institute of Computer Science (NUECS).        //
//--------------------------------------------------------------------------//
//   	This file contains  C  functions to hook up the  Keyboard interrupt //
// and update a Key table which records the pressed and released keys which //
// then checked through the defined words to know the status of any key.    //
//--------------------------------------------------------------------------//
// @Copyright: Muhammad Faisal Nisar Choudhry.                              //
// You can freely use this code but I didnot except any claim of any type   //
// if this program hurts you or your computer in any manner.                //
// If you have any Question or Bug report just contact me via email at      //
// e-mail: talashme@yahoo.com                                               //
//--------------------------------------------------------------------------//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

//---Includes---
#include <dos.h>
#include <iostream.h>
#include <stdlib.h>

//---defines---
#define		KEYBOARD_INT	0x09  // Keyboard interupt
#define         KEY_BUFFER	0x60  // location of keyboard buffer
#define		KEY_CONTROL     0x61  // location of key controller
#define         INT_CONTROL	0x20  // location of interupt controller

#define         ESC		Key_table[1]
#define         KEY_1		Key_table[2]
#define         KEY_2		Key_table[3]
#define         KEY_3		Key_table[4]
#define         KEY_4		Key_table[5]
#define         KEY_5		Key_table[6]
#define         KEY_6		Key_table[7]
#define         KEY_7		Key_table[8]
#define         KEY_8		Key_table[9]
#define         KEY_9		Key_table[10]
#define         KEY_0		Key_table[11]
#define         MINUS		Key_table[12]
#define         EQUAL		Key_table[13]
#define         BACK_SPACE	Key_table[14]
#define         TAB		Key_table[15]
#define         KEY_Q		Key_table[16]
#define         KEY_W		Key_table[17]
#define         KEY_E		Key_table[18]
#define         KEY_R		Key_table[19]
#define         KEY_T		Key_table[20]
#define         KEY_Y		Key_table[21]
#define         KEY_U		Key_table[22]
#define         KEY_I		Key_table[23]
#define         KEY_O		Key_table[24]
#define         KEY_P		Key_table[25]
#define         KEY_[		Key_table[26]
#define         KEY_]		Key_table[27]
#define         ENTER		Key_table[28]
#define         Ctrl		Key_table[29]
#define         KEY_A		Key_table[30]
#define         KEY_S		Key_table[31]
#define         KEY_D		Key_table[32]
#define         KEY_F		Key_table[33]
#define         KEY_G		Key_table[34]
#define         KEY_H		Key_table[35]
#define         KEY_J		Key_table[36]
#define         KEY_K		Key_table[37]
#define         KEY_L		Key_table[38]
#define         SEMICOLON	Key_table[39]
#define         APOSTROPHE	Key_table[40]
#define         TILDE		Key_table[41]
#define         L_SHIFT		Key_table[42]
#define         BACK_SLASH	Key_table[43]
#define         KEY_Z		Key_table[44]
#define         KEY_X		Key_table[45]
#define         KEY_C		Key_table[46]
#define         KEY_V		Key_table[47]
#define         KEY_B		Key_table[48]
#define         KEY_N		Key_table[49]
#define         KEY_M		Key_table[50]
#define         COMMA		Key_table[51]
#define         PERIOD		Key_table[52]
#define         FORWARD_SLASH	Key_table[53]
#define         R_SHIFT		Key_table[54]
#define         PRINT_SCREEN	Key_table[55]
#define         ALT		Key_table[56]
#define         SPACEBAR	Key_table[57]
#define         CAPS_LOCK	Key_table[58]
#define         F1		Key_table[59]
#define         F2		Key_table[60]
#define         F3		Key_table[61]
#define         F4		Key_table[62]
#define         F5		Key_table[63]
#define         F6		Key_table[64]
#define         F7		Key_table[65]
#define         F8		Key_table[66]
#define         F9		Key_table[67]
#define         F10		Key_table[68]
#define         F11		Key_table[87]
#define         F12		Key_table[88]
#define         NUM_LOCK	Key_table[69]
#define         SCROLL_LOCK	Key_table[70]
#define         HOME		Key_table[71]
#define         UP_ARROW	Key_table[72]
#define         PAGE_UP		Key_table[73]
#define         NUM_MINUS	Key_table[74]
#define         LEFT		Key_table[75]
#define         CENTER		Key_table[76]
#define         RIGHT		Key_table[77]
#define         NUM_PLUS	Key_table[78]
#define         END		Key_table[79]
#define         DOWN		Key_table[80]
#define         PAGE_DOWN	Key_table[81]
#define         INSERT		Key_table[82]
#define         DELETE		Key_table[83]

#ifdef __cplusplus
	#define __CPPARGS ...
#else
	#define __CPPARGS
#endif

//---Global---
int  	Key_table[101]= { 0,
			  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
			  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
			  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
			  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
			  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0 };

// this pointer will hold the old keyboard interrupt handler
void  interrupt far ( *Old_keyboard_ISR)(__CPPARGS);

int	Raw_key=0;

//---Prototypes---
void  interrupt far New_keyboard_ISR (__CPPARGS);
void  Install_keyboard ();
void  Uninstall_keyboard ();

//----------------------------------------------------------------------------
//---Functions---
//----------------------------------------------------------------------------
void interrupt far New_keyboard_ISR (__CPPARGS)
	{
	 // read the key from keyboard and then renable it to read next input.
	 // its a total control ISR.

	 asm 	{
		 STI					; // re-enable interrupts
		 IN		AL,	KEY_BUFFER	; // get the key pressed
		 XOR		AH,	AH              ; // clear	AH
		 MOV            Raw_key,AX              ; // storing the key
		 IN		AL,	KEY_CONTROL     ; // set control register
		 OR		AL,	82h             ; // set the proper bits to
							 // reset the keyboard flip flop
		 OUT		KEY_CONTROL,AL          ; // send new data back to the
							 // control register
		 AND		AL,	7fh             ;
		 OUT		KEY_CONTROL,AL          ; // completing the reset
		 MOV            AL,	20h             ;
		 OUT		INT_CONTROL,AL          ; // re-enable interrupts (not complsery in C)
		} // end of asembly code.

	 // Processing and updating the Key_table
	 if ( Raw_key < 100 ) // make code
		Key_table [Raw_key] = 1;
	 else if ( Raw_key > 127 ) // break code
		Key_table[Raw_key-128] = 0;
	}
//----------------------------------------------------------------------------
void Install_keyboard ()
	{
	 // it will simply install the New keyboard ISR

	 Old_keyboard_ISR = _dos_getvect ( KEYBOARD_INT );
	 _dos_setvect ( KEYBOARD_INT, New_keyboard_ISR );
	}
//----------------------------------------------------------------------------
void Uninstall_keyboard ()
	{
	 // it will restore the dos keyboard interruput which was hooked in
	 // install keyboard function.

	 _dos_setvect ( KEYBOARD_INT, Old_keyboard_ISR );
	}
//----------------------------------------------------------------------------
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//--- @Copyright: Muhammad Faisal Nisar Choudhry (730) ---------------------//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//---THATS-ALL-FOLKS--------------------------------------------------------//