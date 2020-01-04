/*==========================================================================
bbcweather
formatter.h
Copyright (c)2017 Kevin Boone, GPLv3.0
*==========================================================================*/

#pragma once

#include "bool.h"

#define FMTFLAG_H1      0x00000001
#define FMTFLAG_H2      0x00000002

typedef struct _FormatterContext 
  {
  int width;
  BOOL no_wrap;
  BOOL no_highlight;
  } FormatterContext;

BOOL formatter_parse_and_display_rss (const FormatterContext *context, 
  const char *rss, char **error);



