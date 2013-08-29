#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <fstream.h>
#include "mode13h.h"

struct Bmp_File_Header
{
	unsigned short Type;
   unsigned long Size;
   unsigned short Reserved1;
   unsigned short Reserved2;
   unsigned long OffBits;
   unsigned long StructSize;
   unsigned long Width;
   unsigned long Height;
   unsigned short Planes;
   unsigned short BitCount;
   unsigned long Compression;
   unsigned long Image_Size;
   long XPelsPerMeter;
   long YPelsPerMeter;
   unsigned long ClrUsed;
   unsigned long ClrImportant;
};

class Bitmap
{
   private:
		Bmp_File_Header Header;
   	BYTE Palette[768];
   	BYTE far* Image;
   public:
   	Bitmap();
      ~Bitmap();
   	bool Load(char * FileName);
      void Draw(BYTE far * Dest, bool Use_Pal = true);
      void Set_Palette() {::Set_Palette(Palette);}
      void Copy(int x, int y, int width, int height, BYTE * dest);
      void Snapshot();
      void Save(char * filename);
};

Bitmap::Bitmap()
{
	Image = (BYTE far *)_fmalloc(64000l);
}

Bitmap::~Bitmap()
{
	if(Image)
   	_ffree(Image);
}

bool Bitmap::Load(char * FileName)
{
	if(!Image)
   {
   	cout << "Err: not enough mem to create buffer for Bitmap!";
      return false;
   }
   //cout << "Enough mem!" << endl;
	ifstream File(FileName,ios::nocreate);
   if(!File)
   {
   	cout << "Err: Unable to open file " << FileName << "!";
      return false;
   }
   //cout << "Opened file!" << endl;
   if(!(File.read((char*)&Header,sizeof(Header))))
   {
   	cout << "Err: Unable to read header in file " << FileName << "!";
      File.close();
      return false;
   }
   //cout << "Read header!" << endl;
   if(Header.BitCount != 8 || Header.Width != SCREEN_WIDTH ||
              Header.Height != SCREEN_HEIGHT)
   {
   	cout << "Err: File " << FileName << "is not a 320x200x8 bitmap";
      File.close();
      return false;
   }
   //cout << "Proper Format!" << endl;
   int i;
   BYTE Trash;
   for(i=0;i<256;i++)
   {
   	if(!(File.get(Palette[3*i+2])) || !(File.get(Palette[3*i+1])) ||
      	!(File.get(Palette[3*i])) || !(File.get(Trash)))
      {
      	cout << "Err: Unable to read palette in file " << FileName << "!";
         File.close();
         return false;
      }
   }
   //cout << "Read palette!" << endl;
   BYTE buffer[320];                          //used to read in a line
   BYTE far * dest;
   BYTE far * src = buffer;
   for(i=0;i<SCREEN_HEIGHT;i++)
   {
   	if(!(File.read(buffer,Header.Width)))
      {
      	cout << "Err: Unable to read image data in file " << FileName << "!";
         File.close();
         return false;
      }
      //cout << "Read line " << i << "!" << endl;
      dest = Image + (199-i)*320;
      _fmemcpy(dest,src,320);
      //cout << "Copied line " << i << "!" << endl;
   }
   //cout << "Read the file!" << endl;
   return true;
}                    

void Bitmap::Draw(BYTE far * Dest, bool Use_Pal)
{  if(Use_Pal)
		Set_Palette();
	Copy_Buffer_(Dest,Image);
}

void Bitmap::Copy(int x, int y, int width, int height, BYTE * dest)
{//copy a width by height area @ (x,y) to dest
	BYTE far * src = Image + (y<<6) + (y<<8) + x;
   int xinc = SCREEN_WIDTH - width;
   int ycnt, xcnt;

   for(ycnt = 0;ycnt < height;ycnt++)
   {
   	for(xcnt = 0;xcnt < width;xcnt++)
      	*(dest++) = *(src++);
      src += xinc;
   }

   /*int xinc = SCREEN_WIDTH - width;
   width--;                                     //width now equals # of cols
   BYTE far * foo = Image;
   asm{
   	push ds

      lds si, foo
      add si, x
      mov ax, y
      shl ax, 8
      add si, ax
      mov ax, y
      shl ax, 6
      add si, ax

      les di, dest

      mov cx, height
      dec cx                                     //height - 1 rows 
      }
rowloop:
	asm{
   	push cx
      mov cx, width
      rep movsb
      pop cx
      add si, xinc
      loop rowloop
      }
   asm pop ds*/
}

void Bitmap::Snapshot()
{
	Copy_Buffer_(Image,Video_Buffer);
   int i;
   outp(0x3C6,0xFF);
   outp(0x3C7,0);
   for(i=0;i<768;i++)
   	Palette[i] = inp(0x3C9);
}

void Bitmap::Save(char * filename)
{
	ofstream outfile(filename,ios::binary);

   outfile.write((char*)&Header,sizeof(Header));
   int i;
   for(i=0;i<256;i++)
     	outfile.put((BYTE)((Palette[3*i+2])<<2))
             .put((BYTE)((Palette[3*i+1])<<2))
             .put((BYTE)((Palette[3*i])<<2))
             .put((BYTE)0);
   BYTE buffer[320];
   BYTE far *dest = buffer;
   BYTE far *src;
   for(i=0;i<SCREEN_HEIGHT;i++)
   {
   	src = Image+(199-i)*320;
      _fmemcpy(dest,src,320);
      outfile.write(buffer,320);
   }
   outfile.close();
}





#endif
