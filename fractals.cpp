#include "mode13h.h"                        //my 13h library
#include <conio.h>                          //getch()
#include <math.h>                           //M_PI, sin(), cos()

int owenmap();

#define RAD(x) ((x)*M_PI/180)               //convert x to radians

#define COLOR ((int)(128*sin((25*sin(RAD(j)))+i)+128)) //|\
              //((int)(128*sin((25*sin(RAD(i)))+j)+128))
                                 //formulas for finding a given pts color, feel
                                 //free to play around with them

#define COLOR_DB ((int)(128*cos(RAD(i+50*cos(RAD(j))+25*sin(RAD(j))))+128)) |\
                 ((int)(128*cos(RAD(j+50*cos(RAD(i))+25*sin(RAD(i))))+128))


int main()
{

	Set_Mode(MCGA);                          //set video mode 13h (MCGA)

   int i,j;                                 //counters

   if(1) { owenmap(); exit(0);}

   //first, I'll draw the pattern directly to the screen...
   for(i=0;i<SCREEN_WIDTH;i++)
   	for(j=0;j<SCREEN_HEIGHT;j++)
      	Plot_Pixel(i,j,COLOR);
   getch();                                //wait for a keypress

   //and now I'll draw to a buffer and then copy that to scrn

   Create_DB();                            //allocate mem for a double buffer
   Clear_Scrn(0);                          //clear screen to 0(black)
   Clear_Scrn_DB(0);                       //clear the DB to 0

   for(i=0;i<SCREEN_WIDTH;i++)
   	for(j=0;j<SCREEN_HEIGHT;j++)
      	Plot_Pixel_DB(i,j,COLOR_DB);
   Show_DB();
   getch();

   Delete_DB();                            //free the Db's Memory
   Set_Mode(TEXT);                         //return to text mode
   return 0;
}

int owenmap()
{

  int i,j,k,color;

  float x,y, u,v, mag, utemp, cx, cy, l;
  float xmin;
  float ymin, yrange;

  /////////////////////////////////////////////////////////
  ///////////some interesting places to look///////////////
  /////////////////////////////////////////////////////////
  /////////// un-comment as necessary... //////////////////
  /////////////////////////////////////////////////////////

  //original coords
  float xrange = 3.5, xcenter = -0.5, ycenter=0.0;
  int movieframes = 60;
  float framechange = 0.10;

  //zoom in on one of the baby mandelbrots near the top
  //float xrange = .03, xcenter = -0.157, ycenter=1.033;
  //int movieframes = 80;
  //float framechange = 0.015;

  //zoom in on a 7-bulb on the side of the main mandelbrot
  //float xrange = .20, xcenter = -0.65, ycenter=0.435;
  //int movieframes = 100;
  //float framechange = 0.0015;

  //right end of mandelbrot set
  //float xrange = .25, xcenter = 0.325, ycenter=0.0;
  //int movieframes = 120;
  //float framechange = 0.002;

  /////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////

  //code that computes yscale, minima, etc.
  xmin = xcenter - 0.5 * xrange;
  yrange = 0.8 * xrange;
  ymin = ycenter - 0.5 * yrange;


  //first, I'll draw the pattern directly to the screen...
   for(i=0;i<SCREEN_WIDTH;i++) {
   	for(j=0;j<SCREEN_HEIGHT;j++) {
         x=(float)i/SCREEN_WIDTH;
         y=1.0 - (float)j/SCREEN_HEIGHT;
         x=xmin+(x*xrange);
         y=ymin+(y*yrange);

         u=0;
         v=0;
         mag=u*u+v*v;

         //////////////////////////
         //begin mandelbrot program

         for(k=0;(k<100)&&(mag<=25);k++) {
         	// z = z^2 + c; c = x + yi
            // so  u = u*u - v*v + x;
            // and v = 2*u*v + y;
            utemp = u;
            u = u*u - v*v + x;
            v = 2*v*utemp + y;
            mag = u*u + v*v;
         	}
         if (k >= 99)
         	color = 0;
         else
         	color = (int) 32 + (k*64/99);

         //end mandelbrot program
         ////////////////////////

      	Plot_Pixel(i,j,color);
    	}
   }

   getch();


  cx = xcenter;
  cy = ycenter;

  xrange = 3.5; xcenter = 0; ycenter=0.0;

  //code that computes yscale, minima, etc.
  xmin = xcenter - 0.5 * xrange;
  yrange = 0.8 * xrange;
  ymin = ycenter - 0.5 * yrange;
  
  Create_DB();

  for(l=-(movieframes/2)*framechange;l<(framechange*movieframes/2);l+=framechange) {
  //now let's do it again
   for(i=0;i<SCREEN_WIDTH;i++) {
   	for(j=0;j<SCREEN_HEIGHT;j++) {
         x=(float)i/SCREEN_WIDTH;
         y=1.0 - (float)j/SCREEN_HEIGHT;
         x=xmin+(x*xrange);
         y=ymin+(y*yrange);

         //note that in julia program,
         //u and v are initialized to x and y
         //rather than 0 and 0.
         u=x;
         v=y;
         mag=u*u+v*v;

         //////////////////////////
         //begin julia program

         for(k=0;(k<100)&&(mag<=25);k++) {
         	// z = z^2 + c; c = cx + cyi
            // so  u = u*u - v*v + cx;
            // and v = 2*u*v + cy;
            utemp = u;
            u = u*u - v*v + cx+l;
            v = 2*v*utemp + cy;
            mag = u*u + v*v;
         	}
         if (k >= 99)
         	color = 0;
         else
         	color = (int) 32 + (k*64/99);

         //end julia program
         ////////////////////////

      	Plot_Pixel_DB(i,j,color);
    	}
   }
   Wait_VTrace();                        //wait until the electron beam moves from
                                         //corner to corner and isn't drawing anything
   Show_DB();                            //then show the buffer
   if(kbhit())
   {
   	while(kbhit()) getch();
      getch();
   }
   }
   getch();

   Delete_DB();
   Set_Mode(TEXT);          //return to text mode
   return 0;
}

