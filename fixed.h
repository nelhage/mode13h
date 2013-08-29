#ifndef __FIXED_H__
#define __FIXED_H__

typedef long fixed;

#define FX(a) (((fixed)a)<<8)
#define INT(a) ((a)>>8)
#define FXMUL(a,b) (((a)*(b))>>8)
#define FXDIV(a,b) (((a)<<8)/(b))

#endif