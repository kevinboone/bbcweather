/*==========================================================================
bbcweather
utf32_string.h
Copyright (c)2017 Kevin Boone, GPLv3.0
*==========================================================================*/

#pragma once

#include "convertutf.h"

#define UTF8_MAX_BYTES 5

int utf32_string_length (const UTF32 *s);
char *utf32_string_convert_to_utf8 (const UTF32 *_in);
UTF32 *utf32_string_create_from_utf8 (const char *_in);
void utf32_string_convert_single_char (const UTF32 c, char *utf8); 


