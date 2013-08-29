#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <dos.h>

struct Mouse_Status
{                                //struct to hold mouse info
	int x,y;
   bool L_Button;                 //is it pressed?
   bool M_Button;
   bool R_Button;
   Mouse_Status()
   {
   	x = y = 0;
      L_Button = M_Button = R_Button = false;
   }
};

int Setup_Mouse()                         //returns # of usable buttons
{

	REGS regs;
   regs.x.ax = 0x0000;
   int86(0x33,&regs,&regs);
   if(regs.x.ax == 0xFFFF)                 //if it finds a mouse...
   	return regs.x.bx;                    //return # buttons
   else
   	return 0;                            // else, no mouse, so 0 for err
}

void Show_Mouse()                          //increment mouse show counter -
{                                          //when it reaches 0 the mouse will be shown
	REGS regs;

   regs.x.ax = 0x0001;
   int86(0x33,&regs,&regs);
}

void Hide_Mouse()                         //dec counter, mouse will be hidden when it reches -1
{
	REGS regs;

   regs.x.ax = 0x0002;
   int86(0x33,&regs,&regs);
}

void Get_Mouse_Status(Mouse_Status &Status)            //set status = current status
{                                                      //call by reference so we can change it
	REGS regs;

   regs.x.ax = 0x0003;
   int86(0x33,&regs,&regs);

   Status.x = (regs.x.cx)/2;                         //scale  to the screen
   Status.y = regs.x.dx;

   Status.L_Button = regs.x.bx & 0x0001;
   Status.R_Button = regs.x.bx & 0x0002;
   Status.M_Button = regs.x.bx & 0x0004;
}

void Set_Mouse_Position(int x, int y)                 //set mouse position to (x,y)
{
	REGS regs;

   regs.x.ax = 0x0004;

   regs.x.cx = x*2;
   regs.x.dx = y;

   int86(0x33,&regs,&regs);
}

#endif


