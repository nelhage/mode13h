#ifndef __MODE13H_H__
#define __MODE13H_H__

//#define __Debug

#ifdef __Debug
	#define Msg(x) cout << x << endl;
	#define Print(x) cout << #x << ": " << x << endl;
	#define Getkey() if(!getch())getch()
#else
	#define Msg(x)
   #define Print(x)
   #define Getkey()
#endif

#include <dos.h>
#include <iostream.h>
#include <malloc.h>
#include <stdlib.h>
#include <conio.h>

typedef unsigned char BYTE;

BYTE far * Video_Buffer = (BYTE*)MK_FP(0xA000,0x0000);               //screen
BYTE far * Double_Buffer = 0;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define MCGA 0x13                                            //defines for Set_Mode()
//#define 13h 0x13
#define TEXT 0x03


/*Note: In order to use a double buffer with as much ease and as little code replication as
possible, every function has a version ending w/ a '_' that takes a buffer for a parm,
and defines for DB and no-DB version, so the user doesn't have to worry about the buffer
too much, but it also allows the user to, by calling the funcs directly, create extra
buffers and write to them*/

#define Plot_Pixel(x,y,c) Plot_Pixel_(x,y,c,Video_Buffer)
#define Plot_Pixel_DB(x,y,c) Plot_Pixel_(x,y,c,Double_Buffer)

#define Get_Pixel(x,y) Get_Pixel_(x,y,Video_Buffer)
#define Get_Pixel_DB(x,y) Get_Pixel_(x,y,Video_Buffer)

#define Clear_Scrn(a) Clear_Scrn_(a,Video_Buffer)
#define Clear_Scrn_DB(a) Clear_Scrn_(a,Double_Buffer)

#define H_Line(a,b,c,d) H_Line_(a,b,c,d,Video_Buffer)
#define H_Line_DB(a,b,c,d) H_Line_(a,b,c,d,Double_Buffer)

#define V_Line(a,b,c,d) V_Line_(a,b,c,d,Video_Buffer)
#define V_Line_DB(a,b,c,d) V_Line_(a,b,c,d,Double_Buffer)

#define Line(a,b,c,d,e) Line_(a,b,c,d,e,Video_Buffer)
#define Line_DB(a,b,c,d,e) Line_(a,b,c,d,e,Double_Buffer)

#define Rect(a,b,c,d,e) Rect_(a,b,c,d,e,Video_Buffer)
#define Rect_DB(a,b,c,d,e) Rect_(a,b,c,d,e,Double_Buffer)

#define Bar(a,b,c,d,e) Bar_(a,b,c,d,e,Video_Buffer)
#define Bar_DB(a,b,c,d,e) Bar_(a,b,c,d,e,Double_Buffer)

#define Show_DB() Copy_Buffer_(Video_Buffer, Double_Buffer)

#define Plot_Pixel_(x,y,c,b) b[x+((y)<<8)+((y)<<6)] = c

#define Get_Pixel_(x,y,b) (b[x+((y)<<6)+(((y)<<8))])

void Set_Mode(int Mode)                                     //set a given mode
{
	REGS regs;
   regs.x.ax = Mode;
   int86(0x10,&regs,&regs);
}

void Create_DB()                                            //alloc DB mem
{
	if((Double_Buffer = (BYTE far *)_fmalloc(64000L)) == 0)
   {
   	cout << "Err: Insufficient mem to create double buffer";
      getch();
      exit(1);
   }
}

void Delete_DB()                                            //free DB mem
{
	if(Double_Buffer)
		_ffree(Double_Buffer);
   Double_Buffer = 0;
}

void Clear_Scrn_(BYTE color, BYTE far * buffer)
{
	asm{
   	mov ah, color
      mov al, ah
      mov cx, 32000
      les di, buffer
      cld
      rep stosw
      }
}

void H_Line_(int x1, int x2, int y, BYTE color, BYTE far * buffer)
{
	asm{
   	cld

      les di, buffer            //es:di = start of buffer

      add di, x1                //}
      mov ax, y                 // }
      shl ax, 8                 //   }
      add di, ax                //    }add x1+y<<8+y<<6 to the offset
      mov ax, y                 //  /
      shl ax, 6                 // /
      add di, ax                ///

      mov cx, x2
      sub cx, x1
      inc cx                    //cx = length of line
      shr cx, 1

      mov ah, color
      mov al,ah

      rep stosw
      }
}

void V_Line_(int y0, int y1, int x0, unsigned char color, BYTE far* buffer)
{
	unsigned char far* offset = (y0<<6)+(y0<<8)+x0+buffer;  //position of first pixel

   for(;y0<y1;y0++,offset += SCREEN_WIDTH)
   	*offset = color;
}

  // Draw an arbitrary line on the screen.
void Line_(int x1, int y1, int x2, int y2, unsigned color, BYTE far* buffer)
{

    // Algorithm ripped wholeheartedly from B. Stroustrup's _The C++
    // Programming Language_, Second Edition.  Page 195.
    //
    register int dx  = 1;
    int a = x2 - x1;
    if (a < 0) dx = -1, a = -a;

    register int dy  = 1;
    int b = y2 - y1;
    if (b < 0) dy = -1, b = -b;

    int two_a = a + a, two_b = b + b;
    int xcrit = -b + two_a;

    register int eps = 0;

    for (;;) {
    	if(x1>0&&x1<320&&y1>0&&y1<200)
      	Plot_Pixel_(x1,y1,color,buffer);
      if (x1 == x2 && y1 == y2) break;
      if (eps <= xcrit) x1 += dx, eps += two_b;
      if (eps >= a || a <= b) y1 += dy, eps -= two_a;
    }
}

void Copy_Buffer_(BYTE far* dest, BYTE far* src)
{
	asm{
   	push ds                                 //save the data segment
      cld
   	mov cx, 32000

      lds si, src
      les di, dest

      rep movsw
      pop ds                                 //restore data segment
   	}      
}

void Wait_VTrace()
{
	int foo;

   foo = inportb(0x3DA) & 0x08;
   if (foo)
   	while(foo)
      {
      	foo = inportb(0x3DA) & 0x08;
      }
   while(!foo)
   {
   	foo = inportb(0x3DA) & 0x08;
   }
}

void Set_Palette(BYTE * Pal)
{
	outp(0x03C6,0xFF);
	outp(0x03C8,0);
   short i;
   for(i=0;i<768;i++)
   	outp(0x03C9,Pal[i]);
}

void Rect_(int Left, int Top, int Right, int Bottom, BYTE color, BYTE far * buffer)
{
	BYTE far * dest1 = buffer + Left + (Top<<6) + (Top<<8);
   BYTE far * dest2 = buffer + Right + (Bottom<<6) + (Bottom<<8);
   int i;
   for(i=Left;i<Right;i++)
     	*(dest1++) = *(dest2--) = color;
   for(i=Top;i<Bottom;i++)
   {
   	*dest1 = *dest2 = color;
      dest1 += SCREEN_WIDTH;
      dest2 -= SCREEN_WIDTH;
	}
}

void Bar_(int Left, int Top, int Right, int Bottom, BYTE color, BYTE far * buffer)
{
	BYTE far * dest = buffer + Left + (Top<<6) + (Top<<8);
   int xcnt,ycnt;
   int xinc = SCREEN_WIDTH - (Right-Left);
   for(ycnt=Top;ycnt<Bottom;ycnt++)
   {
   	for(xcnt=Left;xcnt<Right;xcnt++)
      	*(dest++) = color;
      dest += xinc;
   }
}



#endif









