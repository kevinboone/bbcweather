/*==========================================================================
bbcweatherj
formatter.c
Copyright (c)2017 Kevin Boone, GPLv3.0
*==========================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include "utf32_string.h"

/*==========================================================================
utf32_string_length
Returns the number of characters in a null-terminated array of UTF32 
*==========================================================================*/
int utf32_string_length (const UTF32 *s)
  {
  int i = 0;
  UTF32 c = 0;
  do
    {
    c = s[i];
    i++;
    } while (c != 0);
  return i - 1;
  }


/*==========================================================================
utf32_string_create_from_utf8
Returns a null-terminated array of UTF8 characters
*==========================================================================*/
char *utf32_string_convert_to_utf8 (const UTF32 *_in) 
  {
  const UTF32* in = (UTF32 *)_in;
  // Ugh -- we don't know how many UTF8 bytes will be needed to store
  //   the UTF32 string. So we have to take a very conservative estimate
  //   of 4 bytes per character.
  int max_out = 4 *  utf32_string_length (_in) + 1;
  UTF8 *out = malloc (max_out * sizeof (UTF8));
  memset (out, 0, max_out * sizeof (UTF8));
  UTF8 *out_temp = out;

  ConvertUTF32toUTF8 (&in, in + utf32_string_length (in), 
      &out_temp, out + max_out, 0);
  int len = out_temp - out;
  out[len] = 0;
  return (char *)out;
  }


/*==========================================================================
utf32_string_create_from_utf8
Returns a null-terminated array of UTF32 characters
*==========================================================================*/
UTF32 *utf32_string_create_from_utf8 (const char *_in)
  {
  const UTF8* in = (UTF8 *)_in;
  int max_out = strlen (_in);
  UTF32 *out = malloc (max_out * sizeof (UTF32) + 1);
  memset (out, 0, max_out * sizeof (UTF32));
  UTF32 *out_temp = out;

  ConvertUTF8toUTF32 (&in, in+strlen((char *)in),
      &out_temp, out + max_out, 0);
  // printf ("len=%d\n", out_temp - out);
  int len = out_temp - out;
  out[len] = 0;
  return out;
  }


/*==========================================================================
utf32_convert_single_char
*==========================================================================*/
void utf32_string_convert_single_char (const UTF32 c, char *utf8)
  {
  UTF32 _in = c;
  const UTF32* in = &_in;
  int max_out = UTF8_MAX_BYTES; 
  UTF8 *out = (UTF8 *)utf8;
  memset (out, 0, max_out * sizeof (UTF8));
  UTF8 *out_temp = out;

  ConvertUTF32toUTF8 (&in, in + 1, 
      &out_temp, out + max_out * 4, 0);
  int len = out_temp - out;
  utf8[len] = 0;
  }



