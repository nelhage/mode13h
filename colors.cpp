#include "mode13h.h"                        //my 13h library
#include <iostream.h>
#include <conio.h>                          //getch()
#include <math.h>                           //M_PI, sin(), cos()

long Isqrt(double val);
void CoolPal();

#define RAD(x) ((x)*M_PI/180)               //convert x to radians

#define COLOR ((int)(128*sin((25*sin(RAD(j)))+i)+128)) |\
              ((int)(128*sin((25*sin(RAD(i)))+j)+128))
//#define COLOR Isqrt((i-160-sin(i))*(i-160-sin(i))+(j-100-cos(j))*(j-100-cos(j)))// &\
              //Isqrt((i-160)*(i-160)+(j-100)*(j-100))
                                 //formulas for finding a given pts color, feel
                                 //free to play around with them

#define COLOR_DB ((int)(128*cos(RAD(i+50*cos(RAD(j))+25*sin(RAD(j))))+128))&\
                 ((int)(128*cos(RAD(j+50*cos(RAD(i))+25*sin(RAD(i))))+128))


int main()
{

	/*for(int i=0;i<1000;i++)
   {
   	cout << "Isqrt(" << i << ") = " << Isqrt(i) << endl;
      if(!(i%25))
      	getch();
   }*/
	Set_Mode(MCGA);                          //set video mode 13h (MCGA)
   
   int i,j;                                 //counters
   //first, I'll draw the pattern directly to the screen...
   for(i=0;i<SCREEN_WIDTH;i++)
   	for(j=0;j<SCREEN_HEIGHT;j++)
      {
      	Plot_Pixel(i,j,COLOR);
         //cout << "(x,y): (" << i << "," << j << ")." << endl;
         //getch();
      }
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

long Isqrt(double val)
{
	if(!val)return 0;
   if(val==1)return 1;
   if(val < 0) return -1;
	double ans = val/2;
   double tmp = val/ans;
   while(tmp < ans-.001 || tmp > ans+.001)
   {
   	ans = (tmp+ans)/2;
      tmp = val/ans;
   }
   return (long)ans;
}



