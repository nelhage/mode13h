#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#pragma option -wsig

#include "mode13h.h"
#include "fixed.h"

#include <math.h>

#define H_Line_Clipped(a,b,c,d) H_Line_Clipped_(a,b,c,d,Video_Buffer)
#define H_Line_Clipped_DB(a,b,c,d) H_Line_Clipped_(a,b,c,d,Double_Buffer)


fixed sin_t[360],cos_t[360];                            //sin and cos tables

void Setup_Tables()                                     //init tables
{
	for(int i=0;i<360;i++)
   {
   	sin_t[i] = sin((M_PI*i)/180)*256;
      cos_t[i] = cos((M_PI*i)/180)*256;
   }
}

void H_Line_Clipped_(int x1, int x2, int y, BYTE color, BYTE far * buffer)
{
	if(y<0||y>199)                            //if off screen in y, return
   {
   	//Msg("offscrn in y");
      //Print(y);
      //Getkey();
      return;
   }

	if(x1>x2)                                 // make sure x1<x2
   {
   	//Msg("x1 > x2!");
      //Print(x1);
      //Print(x2);
      //Getkey();
      int tmp = x1;
      x1 = x2;
      x2 = tmp;
   }

   if(x1>319 || x2<0)                        //if offscrn in x, return
   {
   	//Msg("offscrn in x!");
      //Print(x1);
      //Print(x2);
      //Getkey();
     	return;
   }

   if(x1<0)
     	x1 = 0;                                //if either edge is offscrn, clip to scrn
   if(x2>319)
   	x2 = 319;
	asm{                                      //draw it
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
      //shr cx, 1

      mov al, color
      //mov ah,al

      rep stosb
      }
}

struct Point2d                                          //stores a 2d point
{
	fixed x,y;

   Point2d();
   Point2d(fixed X, fixed Y);
   Point2d(int X, int Y);

   Point2d operator+(Point2d & rhs);
   Point2d operator+=(Point2d & rhs);
   Point2d operator*(fixed rhs);
   Point2d operator*=(fixed rhs);
};

Point2d::Point2d()
{
	x = y = 0;
}

Point2d::Point2d(fixed X, fixed Y)
{
	x = X;
   y = Y;
}

Point2d::Point2d(int X, int Y)
{
	x = FX(X);
   y = FX(Y);
}

Point2d Point2d::operator+(Point2d & rhs)
{
	Point2d temp;
   temp.x = x+rhs.x;
   temp.y = y+rhs.y;
   return temp;
}

Point2d Point2d::operator+=(Point2d & rhs)
{
	x += rhs.x;
   y += rhs.y;
   return *this;
}

Point2d Point2d::operator*(fixed rhs)
{
	Point2d temp;
   temp.x = FXMUL(x,rhs);
   temp.y = FXMUL(y,rhs);
   return temp;
}

Point2d Point2d::operator*=(fixed rhs)
{
	x = FXMUL(x,rhs);
   y = FXMUL(y,rhs);
   return *this;
}


class Triangle2d
{
   private:
		Point2d Vertices[3];
   	Point2d Center;                 //actual coordinates of center

      void Sort();                    //sort Vertices by ascending y
   public:
   	Triangle2d();
   	~Triangle2d();

   	void Set_Pos(fixed x1, fixed y1, fixed x2, fixed y2,
   					 fixed x3, fixed y3, fixed x, fixed y);

	   void Draw(BYTE Color, BYTE far * dest);
      void Outline(BYTE Color, BYTE far * dest);
      
      void Translate(fixed t_x, fixed t_y);
      void Scale(fixed S);
      void Rotate(fixed theta);
};

void Triangle2d::Sort()
{
	Point2d temp;
   
	if(Vertices[0].y > Vertices[1].y)      //sort by y
   {
   	temp = Vertices[1];
      Vertices[1] = Vertices[2];
      Vertices[2] = temp;
   }

   if(Vertices[0].y > Vertices[2].y)
   {
   	temp = Vertices[0];
      Vertices[0] = Vertices[2];
      Vertices[2] = temp;
   }

     if(Vertices[1].y > Vertices[2].y)
   {
   	temp = Vertices[1];
      Vertices[1] = Vertices[2];
      Vertices[2] = temp;
   }

}

Triangle2d::Triangle2d()
{
	Vertices[0] =
   Vertices[1] =
   Vertices[2] =
   Center = Point2d(0,0);
}

Triangle2d::~Triangle2d()
{
}

void Triangle2d::Set_Pos(fixed x1, fixed y1, fixed x2, fixed y2,
   							 fixed x3, fixed y3, fixed x, fixed y)
{
	Vertices[0].x = x1;
   Vertices[0].y = y1;
   Vertices[1].x = x2;
   Vertices[1].y = y2;
   Vertices[2].x = x3;
   Vertices[2].y = y3;

   Sort();

   Center.x = x;
   Center.y = y;

}

void Triangle2d::Draw(BYTE Color, BYTE far * dest)
{
	Point2d t_Vertices[3];
   t_Vertices[0] = Vertices[0] + Center;
   t_Vertices[1] = Vertices[1] + Center;
   t_Vertices[2] = Vertices[2] + Center;

	fixed Left, Right;              //current edges
   fixed dl,dr;                    //delta left/right

   int y = INT(t_Vertices[0].y);  //current y
   /*Print(INT(Vertices[0].x));
   Print(INT(Vertices[0].y));
   Print(INT(Vertices[1].x));
   Print(INT(Vertices[1].y));
   Print(INT(Vertices[2].x));
   Print(INT(Vertices[2].y));
   Msg(" ")
   Print(INT(t_Vertices[0].x));
   Print(INT(t_Vertices[0].y));
   Print(INT(t_Vertices[1].x));
   Print(INT(t_Vertices[1].y));
   Print(INT(t_Vertices[2].x));
   Print(INT(t_Vertices[2].y));
   Getkey();   */
   if(Vertices[0].y != Vertices[1].y)                         //if !flat-topped
   {
   	Left = Right = t_Vertices[0].x;
	   dl = FXDIV(t_Vertices[0].x - t_Vertices[1].x,t_Vertices[0].y - t_Vertices[1].y);
   	dr = FXDIV(t_Vertices[0].x - t_Vertices[2].x,t_Vertices[0].y - t_Vertices[2].y);

	   for(;y != INT(t_Vertices[1].y);y++)
	   {
   		H_Line_Clipped_(INT(Left),INT(Right),y,Color,dest);
      	Left += dl;
	      Right += dr;
   	}
   }
   else
   {
   	Left = t_Vertices[0].x;
   	Right = t_Vertices[1].x;           //make edges edges of top
   }
   if(Vertices[1].y != Vertices[2].y)
   {
		dl = FXDIV(Left-t_Vertices[2].x,t_Vertices[1].y-t_Vertices[2].y);
   	dr = FXDIV(Right-t_Vertices[2].x,t_Vertices[1].y-t_Vertices[2].y);

	   for(;y != INT(t_Vertices[2].y);y++)
   	{
   		H_Line_Clipped_(INT(Left),INT(Right),y,Color,dest);
	      Right += dr;
   	   Left += dl;
	   }
   }
}

void Triangle2d::Outline(BYTE Color, BYTE far * Dest)  //outline a triangle
{
   Point2d t_Vertices[3];
   t_Vertices[0] = Vertices[0] + Center;
   t_Vertices[1] = Vertices[1] + Center;
   t_Vertices[2] = Vertices[2] + Center;

   /*Print(INT(t_Vertices[0].x));
   Print(INT(t_Vertices[0].y));
   Print(INT(t_Vertices[1].x));
   Print(INT(t_Vertices[1].y));
   Print(INT(t_Vertices[2].x));
   Print(INT(t_Vertices[2].y));
   Getkey(); */

	Line_(INT(t_Vertices[0].x),INT(t_Vertices[0].y),
   		INT(t_Vertices[1].x),INT(t_Vertices[1].y),Color,Dest);

   Line_(INT(t_Vertices[1].x),INT(t_Vertices[1].y),
   		INT(t_Vertices[2].x),INT(t_Vertices[2].y),Color,Dest);

   Line_(INT(t_Vertices[2].x),INT(t_Vertices[2].y),
   		INT(t_Vertices[0].x),INT(t_Vertices[0].y),Color,Dest);
}

void Triangle2d::Translate(fixed t_x, fixed t_y)
{
	Center.x += t_x;
   Center.y += t_y;
}

void Triangle2d::Scale(fixed S)
{
	Vertices[0] *= S;
   Vertices[1] *= S;
   Vertices[2] *= S;
}

void Triangle2d::Rotate(fixed theta)           //No, it isn't fast, but it works
{                                              //I'm too lazy to optimize at the moment
   fixed tmp;
   /*Print(INT(Vertices[0].x));
   Print(INT(Vertices[0].y));
   Print(INT(Vertices[1].x));
   Print(INT(Vertices[1].y));
   Print(INT(Vertices[2].x));
   Print(INT(Vertices[2].y));*/
      
	for(int i=0;i<3;i++)
   {
   	tmp = Vertices[i].x;
   	Vertices[i].x = FXMUL(tmp,cos_t[theta])-
      					 FXMUL(Vertices[i].y,sin_t[theta]);

      Vertices[i].y = FXMUL(tmp,sin_t[theta])+
                      FXMUL(Vertices[i].y,cos_t[theta]);
   }

   /*Msg(" ");
   Print(INT(Vertices[0].x));
   Print(INT(Vertices[0].y));
   Print(INT(Vertices[1].x));
   Print(INT(Vertices[1].y));
   Print(INT(Vertices[2].x));
   Print(INT(Vertices[2].y));
   Getkey();*/
   Sort();
}                                      
#endif








