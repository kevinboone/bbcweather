/*==========================================================================
bbcweather
feed.h
Copyright (c)2017 Kevin Boone, GPLv3.0
*==========================================================================*/

#pragma once

#include "bool.h"

BOOL feed_fetch (const char *url, char **response, char **error);

