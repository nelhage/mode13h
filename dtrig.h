#if !defined(__dtrig_h__)
#define __dtrig_h__
#include <math.h>

const double adjust = M_PI / 180;
double sintable[360], costable[360];


#define sind(x) (sintable[x])
#define cosd(x) (costable[x])

void Init_Tables()
{
	for(int i = 0; i <= 360; i++)
   {
   	sintable[i] = sin(adjust * i);
      costable[i] = cos(adjust * i);
   }
}
int Adjust(int angle)
{
	if(angle > 360)
   	while(angle > 360)angle -= 360;
   if(angle < 0)
   	while(angle < 0)angle += 360;
   return angle;
}



#endif