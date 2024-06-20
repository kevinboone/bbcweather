/*==========================================================================
bbcweatherj
formatter.c
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
#include "sxmlc.h"
#include "utf32_string.h"
#include "formatter.h"
#include "config.h"

/*==========================================================================
formatter_parse_and_display_rss
*==========================================================================*/
void formatter_output (const FormatterContext *context, int flags,
    const UTF32 *s)
  {
  int max_line_len = context->width;
  int line_len = 0;

  if (!context->no_highlight)
    {
    if (flags & FMTFLAG_H1)
      {
      fputs (ANSI_H1, stdout);
      }

    if (flags & FMTFLAG_H2)
      {
      fputs (ANSI_H2, stdout);
      }
    }

  if (context->no_wrap)
    {
    char *utf8 = utf32_string_convert_to_utf8 (s);
    fputs (utf8, stdout);
    free (utf8);
    }
  else
    {
    const int MAX_WORD = 1000; // FRIG
    int word_buff_len = 0;
    UTF32 *word_buff = malloc (MAX_WORD * sizeof (UTF32));
    memset (word_buff, 0, MAX_WORD * sizeof (UTF32));
    int i, len = utf32_string_length (s);
    typedef enum {STATE_START=0} State;
    typedef enum {TYPE_ANY=0, TYPE_WHITE=1} Type;
    State state = STATE_START;
    for (i = 0; i < len; i++)
      {
      UTF32 c = s[i];
      Type type = TYPE_ANY;
      switch (c)
	{
	case ' ': type = TYPE_WHITE; break; 
	default: type = TYPE_ANY;
	}
      
      switch (state * 100 + type)
	{
	case STATE_START * 1000 + TYPE_ANY:
	  word_buff [word_buff_len] = c;
	  word_buff_len++;
	  break;
     
	case STATE_START * 1000 + TYPE_WHITE:
	  word_buff [word_buff_len] = 0;
	  char *utf8 = utf32_string_convert_to_utf8 (word_buff);
   
	  int word_len = utf32_string_length (word_buff);
	  if (word_len + 1 + line_len >= max_line_len - 1)
	    {
	    fputs ("\n", stdout);
	    line_len = 0;
	    }

	  fputs (utf8, stdout);
	  fputs (" ", stdout);
	  line_len += word_len + 1; // +1 for the space
	  free (utf8);
	  word_buff_len = 0;
	  memset (word_buff, 0, MAX_WORD * sizeof (UTF32));
	  state = STATE_START;
	  break;
     
	default:
	  log_error ("Internal error: char type %d in state %d", type, state);
	}

      }

    if (word_buff_len > 0)
      {
      word_buff [word_buff_len] = 0;
      if (word_buff_len + line_len >= max_line_len) fputs ("\n", stdout);
      char *utf8 = utf32_string_convert_to_utf8 (word_buff);
      fputs (utf8, stdout);
      free (utf8);
      word_buff_len = 0;
      memset (word_buff, 0, MAX_WORD * sizeof (UTF32));
      }

    free (word_buff);
    }

  if (!context->no_highlight)
    {
    if (flags && FMTFLAG_H1)
      {
      fputs ("\x1B[0m", stdout);
      }
    }    
  }


/*==========================================================================
formatter_parse_and_display_rss
*==========================================================================*/
BOOL formatter_parse_and_display_rss (const FormatterContext *context, 
    const char *rss, char **error)
  {
  IN
  BOOL ret = FALSE;

  XMLDoc doc;
  XMLDoc_init (&doc);
  if (XMLDoc_parse_buffer_DOM (rss, "bbcweather", &doc))
    {
    XMLNode *root = XMLDoc_root (&doc);
    XMLNode *r1 = root->children[0];
    int i, l = r1->n_children;
    for (i = 0; i < l; i++)
      {
      XMLNode *r2 = r1->children[i]; // r2 = item (and others)
      if (strcmp (r2->tag, "title") == 0)
        {
        char *ed_title = strdup (r2->text);
        UTF32 *l_title = utf32_string_create_from_utf8 (ed_title);
        formatter_output (context, FMTFLAG_H1, l_title);
        free (l_title);
        free (ed_title);
        printf ("\n\n");
        }
      else if (strcmp (r2->tag, "item") == 0)
        {
        char *title = NULL;
        char *desc = NULL;

        int i, l = r2->n_children;
        for (i = 0; i < l; i++)
          {
          XMLNode *r3 = r2->children[i];
          if (strcmp (r3->tag, "title") == 0)
            title = strdup (r3->text);
          if (strcmp (r3->tag, "description") == 0)
            desc = strdup (r3->text);
           } 
        if (title && desc)
          {
          char *ed_title = strdup (title);
     
          // Remove the max/min temp info, as it appears in the description as
          //   well
          char *p = strcasestr (ed_title, ", Maximum");
          if (p) *p = 0;

          UTF32 *l_title = utf32_string_create_from_utf8 (ed_title);
          UTF32 *l_desc = utf32_string_create_from_utf8 (desc);

          formatter_output (context, FMTFLAG_H2, l_title);
          printf ("\n");
          formatter_output (context, 0, l_desc);
          printf ("\n\n");

          free (ed_title);
          free (l_title);
          free (l_desc);

          }
        if (title) free (title);
        if (desc) free (desc);
        }
      }
    XMLDoc_free (&doc);
    }
  else
    {
    asprintf (error, "Can't parse RSS");
    }

  OUT
  return ret;
  }



