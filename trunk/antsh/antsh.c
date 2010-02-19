//---------------------------------------------------------------------------------------
// Author  : BugTraker
// File    : antsh.c - source file of ANT shell.
// Date    : 27/01/2010 01:16
// Website : http://www.bugtraker.pl 
//
// ANTsh - Copyright (C) 2009-2010 BugTraker (http://www.bugtraker.pl).
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
#include "antsh_conf.h"
#include "antsh.h"
#include "antsh_io.h"

#if ANTSH_DEBUG_API == 1                                                                  // Use debug API.
#include "debug_api.h"
#else                                                                                     // Use file system.
#include "./FatFs/ff.h"
#include "./display/display.h"
#include "./system.h"
#include "./stream/stream.h"
#endif


//---------------------------------------------------------------------------------------
//
//                             D E F I N I T I O N S
//
//---------------------------------------------------------------------------------------


// Shell commands.
#define SHELL_NUM_CMDS                  4                                                 // Number supported commands.
#define SHELL_NO_CMD                    0                                                 // Command 0 reserved for errors.


// Shell verbose string indexes.
#define SHELL_INV_OPT                   0U                                                // Verbose display invalid options.
#define SHELL_INV_SWI                   1U                                                // Invalid switch.
#define SHELL_INV_CMD                   2U                                                // Invalid command.
#define SHELL_INV_PATH                  3U                                                // Invalid path.
#define SHELL_BUFF_FULL                 4U                                                // Buffer full.
#define SHELL_PATH_ACC_DEN              5U                                                // Path access denied.
#define SHELL_INV_DRV                   6U                                                // Invalid drive.
#define SHELL_DRV_MNT_OK                7U                                                // Drive mounted.


// Shell format strings.
#define SHELL_PDNG_FOUR_SP              0U                                                // Shell padding string index.
#define FOUR_SP_LEN                     4U                                                // Four padding spaces length.

#define SHELL_VBSE_MAX_LEN              20U                                               // Max len of shell verbose space.
#define SHELL_CMDS_IDX1                 1U                                                // First shell supported command.


typedef struct
  {
    char_t *str;
    uint8_t len;
    void (*hdlr) (void);
  } shell_cmd_t;


typedef struct
  {
    uint8_t cmd;
    uint32_t args;
  } parser_t;


typedef struct 
  {
    char_t buff[ANTSH_CONF_BUFF_IN_SIZE];
    uint16_t count;
    uint16_t size;
  } shell_in_t;


typedef struct 
  {
    char_t buff[ANTSH_CONF_BUFF_OUT_SIZE + SHELL_VBSE_MAX_LEN];                           // Shell outbuff has extra len for shell vbse info.
    uint16_t count;
  } shell_out_t;


typedef struct
  {
    FATFS fs;
    FILINFO fi;
    DIR dir;
  } drive_t;


typedef struct
  {
    shell_in_t in;
    shell_out_t out;
    parser_t parser;
    drive_t drv;
  } shell_t;


// Return values for local shell api.
typedef enum
  {
    SH_MRC_ABT  = 0,                                                                      // Abort command execution.
    SH_MRC_CONT = 1                                                                       // Continue command execution.
  } sh_mrc_t;


//---------------------------------------------------------------------------------------
//
//                        L O C A L   P R O T O T Y P E S
//
//---------------------------------------------------------------------------------------


// Shell parsers.
static sh_mrc_t parse_cmd (void);
static sh_mrc_t parse_args (const char_t *p_args);
static uint16_t shell_buff_prnt (const char_t *src, bool_t vbse);


// Shell string functions.
static void mem_set (char_t *dst, char_t val, uint16_t size);
static char_t *str_xstr (const char_t *haystack, const char_t *needle, char_t xchr);
static uint16_t str_len (const char_t *src);


// Shell command handlers.
static void error (void);
static void ls (void);
static void cd (void);
static void mount (void);


//---------------------------------------------------------------------------------------
//
//                               L O C A L   D A T A
//
//---------------------------------------------------------------------------------------


static shell_t shell;


static const shell_cmd_t shell_cmds[SHELL_NUM_CMDS] =
  {
    { "",        0,      &error },
    { "ls",      2,      &ls    },
    { "cd",      2,      &cd    },
    { "mount",   5,      &mount }
  };


static const char_t *shell_vbse[] =
  {
    "\nInvalid option",
    "\nInvalid switch",
    "\nInvalid command",
    "\nInvalid PATH",
    "\nBuffer full",
    "\nPath access denied",
    "\nIvalid drive",
    "\nDrive mounted"
  };


static const char_t *shell_pdng[] =
  {
    "    "
  };


//---------------------------------------------------------------------------------------
//
//                         G L O B A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


#if ANTSH_CONF_USE_RTOS == 1


//---------------------------------------------------------------------------------------
// ANTsh main engine. In this case it runs as a task. This is the most effective usage,
// due to the nature of inserting commands to the shell. after antsh_cmd() is called, the
// engine receives the message, processes it and asynchronously writes to the output. 
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void 
antsh (void)
{
  while (1)
    {

#if ANTSH_CONF_USE_RTOS_DLY == 1
    ANTSH_RTOS_DLY_API;
#endif
    }
}


//---------------------------------------------------------------------------------------
// ANTsh incomming command. Command is copied to ANTsh buff_in and then available for the
// engine to process.
//
// Arguments:
// const char_t *cmd - command to insert into shell in buffer.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
antsh_cmd (const char_t *cmd)
{
  uint16_t i;
  char_t *p_buff_in = shell.buff_in;


  for (i = 0; i < ANTSH_CONF_BUFF_IN_SIZE && (0x0D != *cmd); i++)
    {
      *p_buff_in++ = *cmd++;
    }
} 


#else


//---------------------------------------------------------------------------------------
// ANTsh incomming command. Command is copied to ANTsh buff_in and then available for the
// engine to process.
//
// Arguments:
// const char_t *cmd - command to insert into shell in buffer.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
void
antsh_cmd (const char_t *cmd_str)
{
  uint16_t i;
  shell_t *p_shell   = &shell;


  mem_set(p_shell->in.buff, 0, sizeof(p_shell->in.buff));
  mem_set(p_shell->out.buff, 0, sizeof(p_shell->out.buff));
  p_shell->parser.cmd      = SHELL_NO_CMD;
  p_shell->out.count       = 0;
  p_shell->in.count        = 0;
  p_shell->in.size         = 0;
  

  for (i = 0; i < ANTSH_CONF_BUFF_IN_SIZE && (0x0D != *cmd_str) && (0 != *cmd_str); i++)
    {
      p_shell->in.buff[i] = *cmd_str++;
      ++p_shell->in.size;                                                                 // Count chars in input buffer.
    }
  if (SH_MRC_CONT == parse_cmd())                                                         // Continue if parser did not issue errors to the
    {                                                                                     // shell output buffer.
      shell_cmds[p_shell->parser.cmd].hdlr();                                             // Handle command.
    }
  antsh_out(p_shell->out.buff, p_shell->out.count);                                       // Output to user's interface.
} 


#endif


//---------------------------------------------------------------------------------------
//
//                          L O C A L   F U N C T I O N S
//
//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
// Parse received command.
// If command is found in buffer of supported commands then format is checked:
// - spaces are allowed before and after command,
// - if after command is extra character then cmd is invalid.
//
// Arguments:
// N/A
//
// Return:
// sh_mrc_t ret - shell return code.
//---------------------------------------------------------------------------------------
static sh_mrc_t
parse_cmd (void)
{

  const shell_cmd_t *p_shell_cmds = &shell_cmds[SHELL_CMDS_IDX1];                         // Pointer to first valid command.
  shell_t *p_shell                = &shell;
  char_t *p_str                   = NULL;
  sh_mrc_t ret                    = SH_MRC_CONT;
  bool_t cmd_valid                = _TRUE_;
  uint8_t i;


  for (i = SHELL_CMDS_IDX1; i < SHELL_NUM_CMDS; i++, p_shell_cmds++)                      // Scan all shell commands (ship unknown).
    {
      p_str = str_xstr(p_shell->in.buff, p_shell_cmds->str, ' ');                         // Search for cmd, allow for preceeding spaces.
      if (NULL != p_str)
        {
          p_shell->in.count = (p_str - p_shell->in.buff) + p_shell_cmds->len;             // Count all processed bytes.
          p_str += p_shell_cmds->len;
          if (p_shell->in.size - p_shell->in.count > 0)                                   // Check if this cmd is longer than expected.
            { 
              cmd_valid = _FALSE_;                                                        // Cmd not valid yet.
              if (*p_str == ' ')                                                          // Extra character found - it must be a space.
                {
                  ++p_str;
                  ++p_shell->in.count;                                                    // Count space.
                  cmd_valid = _TRUE_;                                                     // Command still valid.
                }
            }
          if (_TRUE_ == cmd_valid)
            { 
              p_shell->parser.cmd = i;                                                    // Command valid.
              ret = parse_args(p_str);                                                    // Parse arguments - if any.
            }
          break;
        }
    }

  return (ret);
}


//---------------------------------------------------------------------------------------
// Parse argument of a command (if any).
//
// Arguments:
// 
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static sh_mrc_t
parse_args (const char_t *p_args)
{
  shell_t *p_shell = &shell;
  sh_mrc_t ret     = SH_MRC_CONT;
  uint16_t i;
  uint16_t j;

  
  for (i = p_shell->in.count; i < p_shell->in.size; i++)
    {
      if (0x20 == *p_args)                                                                // Process space.
        {
          ++p_args;
        }
      else
        {
          switch (*p_args)
            {
              case '-':
              ++p_args;
              for (j = i + 1; j < ANTSH_CONF_BUFF_IN_SIZE && (0 != *p_args); j++);
                {
                  if (*p_args >= 'a' && *p_args <= 'z')
                    {
                      p_shell->parser.args |= (uint32_t)0x1 << (*p_args - 'a');
                      ++p_args;
                    }
                  else
                    {
                      shell_buff_prnt(shell_vbse[SHELL_INV_OPT], _TRUE_);
                      ret = SH_MRC_ABT;
                      break;
                    }
                }
              break;

              default:
                break;
            }
          break;            
        }
    }
  p_shell->in.count = i;                                                                  // Update num processed bytes.

  return (ret);
}


//---------------------------------------------------------------------------------------
// Print string into shell output buffer. If vbse argument is 'true' then this is shell
// verbose msg, and extra out buff space is available.
//
// Arguments:
// const char_t *src - string to add to shell out buffer.
// bool_t vbse       - switch to specify if this is shell verbose msg.
//
// Return:
// uint16_t i        - num added bytes.
//---------------------------------------------------------------------------------------
static uint16_t
shell_buff_prnt (const char_t *src, bool_t vbse)
{
  shell_t *p_shell = &shell;
  char_t *dst      = &p_shell->out.buff[p_shell->out.count];
  uint16_t add_len = 0;
  uint16_t i;
  

  if (_TRUE_ == vbse)
    {
      add_len = SHELL_VBSE_MAX_LEN;                                                       // For shell vbse msg extra space is available.
    }
  for (i = 0; i < ((ANTSH_CONF_BUFF_OUT_SIZE + add_len) - p_shell->out.count) && (0 != *src); i++)
    {
      *dst++ = *src++;
    }
  p_shell->out.count += i;                                                                // Update out buff count.
  
  return (i);                                                                             // Return num added bytes.
}


//---------------------------------------------------------------------------------------
// Sets num bytes of destination with given value.
//
// Arguments:
// char_t *dst   - buffer to format.
// char_t val    - value to fill in dst with.
// uint32_t size - size of memory to fill in.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void 
mem_set (char_t *dst, char_t val, uint16_t size)
{
  while (size-- > 0)
    {
      *dst++ = val;
    }
}


//---------------------------------------------------------------------------------------
// Searches a needle in the haystack. Additionnaly extra character may be passed to
// specify only character(s) that can be accepted before string.
//
// Arguments:
// const char_t *haystack - pointer to haystack string.
// const char_t *needle   - pointer to needle substring.
// char_t xchr            - if !0 then only allowed char before needle.
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static char_t *
str_xstr (const char_t *haystack, const char_t *needle, char_t xchr)
{
  uint16_t needle_len   = 0;
  uint16_t haystack_len = 0;
  char_t *p_ret         = NULL;
  bool_t stop           = _FALSE_;
  uint16_t i;
  uint16_t j;

  
  needle_len   = str_len(needle);
  haystack_len = str_len(haystack);
  
  if (haystack_len >= needle_len)                                                         // Check if haystack is big enough to have needle.
    {
      for (i = 0; i <= (haystack_len - needle_len) && (_FALSE_ == stop); i++, haystack++)
        {
          for (j = 0; j < needle_len && *(haystack + j) == *(needle + j); j++)            // Check new haystack position.
            {}
          if (j == needle_len)
            {
              p_ret = (char_t *)haystack;                                                 // Needle found.
              stop  = _TRUE_;
            }
          else
            {
              if ((0 != xchr) && (xchr != *(haystack + j)))                               // If xchr specified and different than currently
                {                                                                         // searched piece if haystack then abort search.
                  stop = _TRUE_;
                }
            }
        }
    }

  return (p_ret);
}


//---------------------------------------------------------------------------------------
// Calculate string length.
//
// Arguments:
// const char_t *src - string to calculate.
//
// Return:
// uint16_t i        - num bytes in string.
//---------------------------------------------------------------------------------------
static uint16_t
str_len (const char_t *src)
{
  uint16_t i = 0;


  while (0 != *src++)
    {
      ++i;
    }
  
  return (i);
}


//---------------------------------------------------------------------------------------
// Handle errors. If inserted command is not recognised by the shell.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
error (void)
{
  shell_buff_prnt(shell_vbse[SHELL_INV_CMD], _TRUE_);
}


//---------------------------------------------------------------------------------------
// List current folder.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void 
ls (void)
{
  shell_t *p_shell = &shell;
  uint16_t len = 0;
  uint16_t diff = 0;


  if (FR_OK == f_opendir(&p_shell->drv.dir, "."))                                         // Open current directory. 
    {
      while (FR_OK == f_readdir(&p_shell->drv.dir, &p_shell->drv.fi))                     // Read directory.
        {
          if (0 != p_shell->drv.fi.fname[0])                                              // Check if file exist.
            {
              len  = str_len(p_shell->drv.fi.fname) + FOUR_SP_LEN;                        // Calculate str len + four padding spaces.
              diff = ANTSH_CONF_BUFF_IN_SIZE - p_shell->out.count;
              if (diff >= len)
                {
                  shell_buff_prnt(p_shell->drv.fi.fname, _FALSE_);                        // Print file name.
                  shell_buff_prnt(shell_pdng[SHELL_PDNG_FOUR_SP], _FALSE_);               // Padding spaces.
                  continue;
                }
              else
                {
                  shell_buff_prnt(shell_vbse[SHELL_BUFF_FULL], _TRUE_);                   // End of processing, no more space in buffer.
                }
            }
          else
            {
              shell_buff_prnt(shell_vbse[SHELL_PATH_ACC_DEN], _TRUE_);                    // Nothing found.
            }
          break;
        }
    }
  else
    {
      shell_buff_prnt(shell_vbse[SHELL_INV_PATH], _TRUE_);                                // Nothing found.
    }
}


//---------------------------------------------------------------------------------------
// Executes commands.
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void 
cd (void)
{
  shell_t *p_shell = &shell;


  if (FR_OK !=  f_chdir(&p_shell->in.buff[p_shell->in.count]))
    {
      shell_buff_prnt(shell_vbse[SHELL_INV_PATH], _TRUE_);
    }
}


//---------------------------------------------------------------------------------------
// Mount drive.
// At the moment drive as per FatFs (0-9) must be specified and it is mounted at '/'.
// [mount (0-9)]
//
// Arguments:
// N/A
//
// Return:
// N/A
//---------------------------------------------------------------------------------------
static void
mount (void)
{
  shell_t *p_shell = &shell;
  uint8_t drv      = 0;
  uint16_t i       = p_shell->in.count;


  drv = p_shell->in.buff[i++] - 0x30;                                                     // Extract drive number (0-9).
  for (; i < p_shell->in.size; i++)
    {
      if (0x20 != p_shell->in.buff[i])                                                    // Check for spaces.
        {
          break;
        }
    }
  if (i == p_shell->in.size)                                                              // Nothing or only spaces after drive number.
    {
      if (FR_OK == f_mount(drv, &p_shell->drv.fs))                                        // Mount drive.
        {
          shell_buff_prnt(shell_vbse[SHELL_DRV_MNT_OK], _TRUE_);                          // Drive mounted.
          f_chdrive(drv);                                                                 // Change drive to mounted drive.
        }
      else
        {
          shell_buff_prnt(shell_vbse[SHELL_INV_DRV], _TRUE_);                             // Invalid drive.
        }
    }
}



