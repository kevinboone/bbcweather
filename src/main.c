/*==========================================================================
bbcweatherj
main.c
Copyright (c)2020 Kevin Boone, GPLv3.0
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
#include <getopt.h>
#include <termios.h>
#include "log.h"
#include "feed.h"
#include "formatter.h"
#include "list.h"

// Maximum length of a BBC location code (it's probably not this long)
#define LOC_CODE_MAX 20


/*==========================================================================
  weather_feed_make_uri 
*==========================================================================*/
char *weather_feed_make_uri (const char *location, BOOL obs)
  {
  char *uri = NULL;
  if (obs)
    asprintf (&uri, 
      "https://weather-broker-cdn.api.bbci.co.uk/en/observation/rss/%s",
      location);
  else
    asprintf (&uri, 
      "https://weather-broker-cdn.api.bbci.co.uk/en/forecast/rss/3day/%s",
      location);
  return uri;
  }


/*==========================================================================
  weather_feed_get_loc_code
  This method reports its own errors, and return TRUE if there are none
*==========================================================================*/
BOOL weather_feed_get_loc_code (const char *location, char *loc_code, 
    BOOL exact)
  {
  IN
  BOOL ret = FALSE;
  BOOL partial = !exact;
  const char *data_file = SHAREDIR "/locations.dat";
  log_debug ("Opening location data file %s", data_file);
  FILE *f = fopen (data_file, "r");
  if (f)
    {
    List *list = list_create (free);
    log_debug ("Location data file opened");
    char line[80];
    while (!feof (f))
      {
      fgets (line, sizeof (line) - 1, f);
      if (line[strlen(line) - 1] == 10)
        line[strlen(line) - 1] = 0;
      if (line[0] == 0) continue;
      if (line[0] == '#') continue;
      char *p = strchr (line, ' ');
      if (p)
        {
        *p = 0;
        const char *code = line;
        const char *name = p+1;
        if (partial && strcasestr (name, location))
          {
          char *s;
          asprintf (&s, "%s %s", name, code);
          list_append (list, s);
          }
        else if (strcasecmp (name, location) == 0)
          {
          char *s;
          asprintf (&s, "%s %s", name, code);
          list_append (list, s);
          }
        }
      else
        {
        log_warning ("Badly-formed line in location data file: %s", line);
        }
      }

    int i, l = list_length (list);
    if (l == 1)
      {
      char *list_item = strdup (list_get (list, 0));
      char *p = strrchr (list_item, ' ');
      strncpy (loc_code, p + 1, LOC_CODE_MAX);
      free (list_item);

      ret = TRUE;
      }
    else if (l == 0)
      {
      log_error ("No location matches '%s'", location); 
      }
    else
      {
      log_error ("Ambiguous location name '%s'. Potential matches:", location);
      for (i = 0; i < l; i++)
        {
        char *list_item = strdup (list_get (list, i));
        char *p = strchr (list_item, ' ');
        *p = 0;
        fprintf (stderr, "%s\n", list_item);
        free (list_item);
        }
      }
    list_destroy (list);
    }
  else
    {
    log_debug ("Location data can't be opened");
    log_error ("Can't open location data file %s", data_file);
    }
  OUT
  return ret;
  }


/*==========================================================================
  weather_feed_list_locations
  This method reports its own errors
*==========================================================================*/
void weather_feed_list_locations (void)
  {
  IN
  const char *data_file = SHAREDIR "/locations.dat";
  log_debug ("Opening location data file %s", data_file);
  FILE *f = fopen (data_file, "r");
  if (f)
    {
    log_debug ("Location data file opened");
    char line[80];
    while (!feof (f))
      {
      fgets (line, sizeof (line) - 1, f);
      if (line[strlen(line) - 1] == 10)
        line[strlen(line) - 1] = 0;
      if (line[0] == 0) continue;
      if (line[0] == '#') continue;
      char *p = strchr (line, ' ');
      if (p)
        {
        *p = 0;
        const char *name = p+1;
        printf ("%s\n", name);
        }
      else
        {
        log_warning ("Badly-formed line in location data file: %s", line);
        }
      }
    }
  else
    {
    log_debug ("Location data can't be opened");
    log_error ("Can't open location data file %s", data_file);
    }
  OUT
  }


/*==========================================================================
main.c
*==========================================================================*/
int main (int argc, char **argv)
  {
  IN

  int ret = 0;
  int width = 80;  //We will try to guess this from an ioctl later
  BOOL istty = FALSE;

  if (isatty (STDOUT_FILENO))
    {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
      {
      width = ws.ws_col;
      istty = TRUE;
      }
    }

  int loglevel = WARNING;
  BOOL show_version = FALSE;
  BOOL show_locations = FALSE;
  BOOL show_usage = FALSE;
  BOOL exact = FALSE;
  BOOL observations = FALSE;

  static struct option long_options[] = 
   {
     {"exact", no_argument, NULL, 'e'},
     {"help", no_argument, NULL, 'h'},
     {"locations", no_argument, NULL, 'l'},
     {"observations", no_argument, NULL, 'o'},
     {"version", no_argument, NULL, 'v'},
     {"width", required_argument, NULL, 'w'},
     {0, 0, 0, 0}
   };

  int opt;
  while (1)
   {
   int option_index = 0;
   opt = getopt_long (argc, argv, "?vlhw:eo",
     long_options, &option_index);

   if (opt == -1) break;

   switch (opt)
     {
     case 0:
       if (strcmp (long_options[option_index].name, "exact") == 0)
         exact = TRUE;
       else if (strcmp (long_options[option_index].name, "help") == 0)
         show_locations = TRUE;
       else if (strcmp (long_options[option_index].name, "locations") == 0)
         show_locations = TRUE;
       else if (strcmp (long_options[option_index].name, "observations") == 0)
         observations = TRUE;
       else if (strcmp (long_options[option_index].name, "version") == 0)
         show_version = TRUE;
       else if (strcmp (long_options[option_index].name, "width") == 0)
         width = atoi (optarg);
       else
         {
         OUT
         exit (-1);
         }
       break;

     case 'e': exact = TRUE; break;
     case 'h': case '?': show_usage = TRUE; break;
     case 'l': show_locations = TRUE; break;
     case 'o': observations = TRUE; break;
     case 'v': show_version = TRUE; break;
     case 'w': width = atoi (optarg); break;
     default:  
       {
       OUT
       exit(-1);
       }
     }
   }

  log_set_level (loglevel);

  if (show_version)
    {
    printf ("%s " VERSION "\n", argv[0]);
    printf ("Copyright (c)2012-2020 Kevin Boone\n");
    printf ("Distributed under the terms of the GPL, v3.0\n");
    OUT
    exit (0);
    }

  if (show_usage)
    {
    printf ("Usage: %s [options] [location or code]\n", argv[0]);
    printf ("  -l, --locations       list locations\n");
    printf ("  -o, --observations    show observations, not forecast\n");
    printf ("  -v, --version         show version information\n");
    printf ("  -w, --width           screen width in columns\n");
    printf ("  -e, --exact           disable partial name searches\n");
    OUT
    exit (0);
    }

  if (show_locations)
    {
    weather_feed_list_locations();
    OUT
    exit (0);
    }

  char *location;

  if (argc > optind)
    location = argv[optind]; 
  else
    location = "London";

  char loc_code[LOC_CODE_MAX + 1];
  loc_code[0] = 0;

  int num = atoi (location);
  if (num > 0)
    strncpy (loc_code, location, LOC_CODE_MAX);
  else
    weather_feed_get_loc_code (location, loc_code, exact);

  if (loc_code[0])
    {
    curl_global_init (CURL_GLOBAL_ALL);

    char *error = NULL;
    char *resp = NULL;

    char *feed_uri = weather_feed_make_uri (loc_code, observations);
    feed_fetch (feed_uri, &resp, &error);
    free (feed_uri);

    if (error)
      {
      log_error (error);
      free (error);
      }
    else
      {
      FormatterContext context;
      context.width = width; 
      context.no_wrap = FALSE; 
      context.no_highlight = FALSE; 
      if (!istty)
        {
        context.no_wrap = TRUE; 
        context.no_highlight = TRUE; 
        }
      formatter_parse_and_display_rss (&context, resp, &error);
      if (error)
	{
	log_error (error);
	free (error);
	}
      free (resp);
      }

    curl_global_cleanup();
    }
  else
    {
    // Error already reported
    }

  OUT
  return ret;
  }

