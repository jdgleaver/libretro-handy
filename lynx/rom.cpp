//
// Copyright (c) 2004 K. Wilkins
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//

//////////////////////////////////////////////////////////////////////////////
//                       Handy - An Atari Lynx Emulator                     //
//                          Copyright (c) 1996,1997                         //
//                                 K. Wilkins                               //
//////////////////////////////////////////////////////////////////////////////
// ROM emulation class                                                      //
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This class emulates the system ROM (512B), the interface is pretty       //
// simple: constructor, reset, peek, poke.                                  //
//                                                                          //
//    K. Wilkins                                                            //
// August 1997                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
// Revision History:                                                        //
// -----------------                                                        //
//                                                                          //
// 01Aug1997 KW Document header added & class documented.                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#define ROM_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "rom.h"

CRom::CRom(const char *romfile,bool useEmu)
{
   int loop;
   mWriteEnable = FALSE;
   mValid       = TRUE;
   Reset();

   // Initialise ROM
   for(loop = 0; loop < ROM_SIZE; loop++)
      mRomData[loop] = DEFAULT_ROM_CONTENTS;

   // actually not part of Boot ROM but uninitialized otherwise
   // Reset Vector etc
   mRomData[0x1F8] = 0x00;
   mRomData[0x1F9] = 0x80;
   mRomData[0x1FA] = 0x00;
   mRomData[0x1FB] = 0x30;
   mRomData[0x1FC] = 0x80;
   mRomData[0x1FD] = 0xFF;
   mRomData[0x1FE] = 0x80;
   mRomData[0x1FF] = 0xFF;

   if (useEmu)
      mValid       = FALSE;
   else
   {
      FILE *fp;

      /* Load up the file */
      if((fp=fopen(romfile,"rb"))==NULL)
        mValid     = FALSE;
      else
      {
         /* Read in the 512 bytes */
         if (fread(mRomData,sizeof(char),ROM_SIZE,fp)!=ROM_SIZE) 
            mValid = FALSE;
         if (fp)
            fclose(fp);
      }

      /* Check the code that has been loaded and report an error if its a
       * fake version (from handy distribution) of the bootrom
       * would be more intelligent to make a crc
       */

      if(mRomData[0x1FE] != 0x80 || mRomData[0x1FF] != 0xFF)
         mValid = FALSE;
   }
}

// Nothing to do here
void CRom::Reset(void) { }
bool CRom::ContextSave(FILE *fp)
{
   if(!fprintf(fp,"CRom::ContextSave"))
      return 0;
   if(!fwrite(mRomData,sizeof(UBYTE),ROM_SIZE,fp))
      return 0;
   return 1;
}

bool CRom::ContextLoad(LSS_FILE *fp)
{
   char teststr[100]="XXXXXXXXXXXXXXXXX";
   if(!lss_read(teststr,sizeof(char),17,fp))
      return 0;
   if(strcmp(teststr,"CRom::ContextSave")!=0)
      return 0;
   if(!lss_read(mRomData,sizeof(UBYTE),ROM_SIZE,fp))
      return 0;
   return 1;
}


//END OF FILE
