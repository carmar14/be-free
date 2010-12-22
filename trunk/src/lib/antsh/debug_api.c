//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : debug_api.c - debug interface for ANTsh.
// Date    : 03/02/2010 23:06
// Website : http://www.bugtraker.pl 
//
// This is very simple ANTsh interface tester. It emulates FatFs interface for testing
// purposes. It does not emulate file system itself. For example, when 'mkdir' command
// is used, f_mkdir() function is called returning predefined value, and no real file
// system operation is performed.
//
// ANTsh - Copyright (C) 2009 BugTraker (http://www.bugtraker.pl).
//
// ANTsh is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// ANTsh is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeFree; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                          I N C L U D E D   F I L E S
//
//---------------------------------------------------------------------------------------


#include "types.h"
#include "antsh.h"
#include "debug_api.h"


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                        L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// 
// 
//
// Arguments:
// 
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
int main (void)
{
  char_t buffer[100];
  uint8_t i;
  

  while (1)
    {
      for (i = 0; i < sizeof(buffer); i++)
        {
          buffer[i] = 0;
        }
      printf("\nANTSH# ");
      scanf("%s", buffer);
      for (i = 0; i < 100; i++)
        {
          if ('_' == buffer[i])
            {
              buffer[i] = 0x20;
            }
        }
      antsh_cmd(buffer);
    }
  return 0;
}


//---------------------------------------------------------------------------------------
// 
// 
//
// Arguments:
// 
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
FRESULT f_readdir (DIR* DirObject, FILINFO* FileInfo)
{
  static uint8_t idx = 0;
  FRESULT ret        = FR_OK;


  sprintf(FileInfo->fname, "File%d.wav", idx++);
  if (idx > 20)
    {
      ret = FR_DISK_ERR;
    }
  return (ret);
}


//---------------------------------------------------------------------------------------
// 
// 
//
// Arguments:
// 
//
// Return:
// N/A
//---------------------------------------------------------------------------------------

FRESULT f_opendir (DIR* DirObject, const CHAR* DirName)
{
  return (FR_OK);
}


//---------------------------------------------------------------------------------------
// 
// 
//
// Arguments:
// 
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
FRESULT 
f_mount (BYTE Drive, FATFS *FileSystemObject)
{

    return (FR_OK);
}


//---------------------------------------------------------------------------------------
// 
// 
//
// Arguments:
// 
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
FRESULT
f_chdir (const CHAR* Path)
{
  return (FR_OK);
}


//---------------------------------------------------------------------------------------
// 
// 
//
// Arguments:
// 
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
FRESULT
f_chdrive (BYTE Drive)
{
  return (FR_OK);
}


//---------------------------------------------------------------------------------------
// 
// 
//
// Arguments:
// 
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
FRESULT 
f_mkdir (const CHAR* DirName)
{
  return (FR_OK);
}


