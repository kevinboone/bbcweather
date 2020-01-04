/*==========================================================================
bbcweatherj
feed.c
Copyright (c)2017 Kevin Boone, GPLv3.0
*==========================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <curl/curl.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "log.h"
#include "feed.h"

#define EASY_INIT_FAIL "Cannot initialize curl"

/*---------------------------------------------------------------------------
Private structs
---------------------------------------------------------------------------*/
struct DBWriteStruct 
  {
  char *memory;
  size_t size;
  };


/*---------------------------------------------------------------------------
feed_write_callback
Callback for storing server response into an expandable memory block
---------------------------------------------------------------------------*/
static size_t feed_write_callback (void *contents, size_t size, 
    size_t nmemb, void *userp)
  {
  IN
  size_t realsize = size * nmemb;
  struct DBWriteStruct *mem = (struct DBWriteStruct *)userp;
  mem->memory = realloc (mem->memory, mem->size + realsize + 1);
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  OUT
  return realsize;
  }



/*==========================================================================
feed_fetch 
*==========================================================================*/
BOOL feed_fetch (const char *url, char **_response, char **error)
  {
  BOOL ret = FALSE;
  IN
  log_info ("Fetching feed %s", url);

  CURL* curl = curl_easy_init();
  if (curl)
   {
   struct DBWriteStruct response;
   response.memory = malloc(1);  
   response.size = 0;    
   
   struct curl_slist *headers = NULL;

    curl_easy_setopt (curl, CURLOPT_URL, url);
	  
    char curl_error [CURL_ERROR_SIZE];
    curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, curl_error);
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, feed_write_callback);
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response);

    CURLcode curl_code = curl_easy_perform (curl);
    if (curl_code == 0)
      {
      long codep = 0;
      curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &codep); 
      if (codep == 200)
        {
        char *resp = response.memory;
        *_response = strdup (resp);
        ret = TRUE;
        }
      else
        {
        if (error)
          {
          asprintf (error, "Server returned error %d", (int)codep);
          }
        }
      }
    else
      {
      if (error)
        *error = strdup (curl_error); 
      }

    free (response.memory);
    curl_slist_free_all (headers); 
    curl_easy_cleanup (curl);
    }
  else
    {
    if (error)
      *error = strdup (EASY_INIT_FAIL); 
    }

  OUT
  return ret;
  }



