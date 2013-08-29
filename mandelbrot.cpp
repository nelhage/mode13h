#include "mode13h.h"
#include "mouse.h"          //my mouse library
#include "F_KEY.h"          //a library for good keyboard input

const int SCREEN_CENTERX = SCREEN_WIDTH/2;       //center of the screen coordinates
const int SCREEN_CENTERY = SCREEN_HEIGHT/2;

void Draw_Mandelbrot(double xrange, double xcenter, double ycenter);

int main()
{
	Mouse_Status Status;                    //to keep track oF the mouse coordinates
   double xrange = 3.5 ,xcenter = -0.6,ycenter = 0.0;//should be familiar
   double Scale;                           //to scale movement back

   if(!Setup_Mouse()) return 0;            //if no mouse detected, exit
   Set_Mouse_Position(SCREEN_CENTERX,SCREEN_CENTERY); //set mouse to the center
   Create_DB();                            //alloc mem for the DB
   Install_keyboard();                     //install the keyboard handler
   Set_Mode(MCGA);                         //set mode 13h

   while(!ESC)                             //every key has a #define for it that
   {                                       //is 1 if pressed, else 0
      Get_Mouse_Status(Status);            //get the mouse position and button states
                                           //(it takes a reference to Status)
      Scale = xrange/7;                  //scale movement based on zoom
      if(Ctrl) Scale /= 2;                 //slow movement if ctrl is pressed

      //move based on how far the mouse is from the center
      xcenter += (((double)(Status.x-SCREEN_CENTERX))/SCREEN_CENTERX) * Scale;
      ycenter += (-((double)(Status.y-SCREEN_CENTERY))/SCREEN_CENTERY) * Scale;

      xcenter -= (LEFT) * Scale;            //move based on the keyboard
      xcenter += (RIGHT) * Scale;

      ycenter += (UP_ARROW) * Scale;
      ycenter -= (DOWN) * Scale;

      if(Status.L_Button || EQUAL) xrange /= 2;   //zoom in or out
      if(Status.R_Button || MINUS) xrange *= 2;
      Set_Mouse_Position(SCREEN_CENTERX,SCREEN_CENTERY); //bring mouse back to the center


      Draw_Mandelbrot(xrange,xcenter,ycenter);           //draw it
      Plot_Pixel_DB(SCREEN_CENTERX,SCREEN_CENTERY,31);   //put a white dot in the center
      Wait_VTrace();                                     //wait for the vertical retrace...
      Show_DB();                                         //and show it!
      //delay(10);
   }


   Delete_DB();                                         //misc. cleanup
   Uninstall_keyboard();
   Set_Mode(TEXT);
	return 0;
}

//draw the mandelbrot set, almost identical to your code
void Draw_Mandelbrot(double xrange, double xcenter, double ycenter)
{
	int i,j,k,color;
	double x,y, u,v, mag, tmp;
   double xmin, ymin, yrange = 0.8 * xrange;

   xmin = xcenter - 0.5*xrange;
   ymin = ycenter - 0.5*yrange;

   for(i=0;i<SCREEN_WIDTH;i++)
   {
   	for(j=0;j<SCREEN_HEIGHT;j++)
      {
      	x = ((double)i)/SCREEN_WIDTH;
         y = 1. - ((double)j)/SCREEN_HEIGHT;

         x = xmin + x*xrange;
         y = ymin + y*yrange;

         u = v = 0;
         mag = 0;

         for(k=0;k<100&&mag <= 25;k++)
         {
         	//z = u + vi; c = x + yi
            //z = z*z + c
            //u = u*u - v*v + x
            //v = 2*u*v + y
            tmp = u;
            u = u*u - v*v + x;
            v = 2*tmp*v + y;
            mag = u*u + v*v;
         }
         if(k >= 99) color = 0;
         else color = 32 + (int)(k*64/99);
         Plot_Pixel_DB(i,j,color);
      }
   }
   //Show_DB();                          //don't show yet so the calling
}                                        //function can draw over it if they want


