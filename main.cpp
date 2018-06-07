/*
 *
 * TagInfo - main.cpp
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

const char *author = "Markus Reckwerth";
const int copyyear = 2018;

using namespace std;

static void print_help (char* pname)
{
  printf("usage: %s [options] <file> ...\n\noptions:\n\t-a\tshow audio stream info\n\t-b\tshow basic tag info\n\t-c\tshow codec specific tag info (default)\n\t-h\tprint help\n\t-v\tprint version\n", pname);
  exit(EXIT_SUCCESS);
}

static void print_version (char *pname)
{
  printf("%s v%i.%i | (c) %i %s\n", pname, VERSION_MAJOR, VERSION_MINOR, copyyear, author);
  exit(EXIT_SUCCESS);
}

static void print_error (char *errmsg)
{
  fprintf(stderr,"ERROR: %s\n",errmsg);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  int opt;
  int flags=0;
  int a_flag=0;
  int b_flag=0;
  int c_flag=0;

  char *progname = basename(argv[0]);

  char *loc = setlocale(LC_CTYPE,"");
  bool isUTF8 = (strstr(loc,"UTF-8") != NULL);

  // loop through arguments
  while ((opt = getopt(argc, argv, "abchv")) != -1) {
    switch (opt) {
      case 'a':
        a_flag=1;
	flags++;
	break;
      case 'b':
        b_flag=1;
	break;
      case 'c':
        c_flag=1;
	break;
      case 'h':
        print_help(progname);
      case 'v':
	print_version(progname);
        default:
	  exit(EXIT_FAILURE);
    }
  }

  // check arguments
  if (argc == optind) print_error((char *) "no input file given");

  // set default option
  if (a_flag == 0 && b_flag == 0 && c_flag == 0) c_flag=1;

  // loop through files
  for(int i = optind; i < argc; i++) {

    TagLib::FileRef f(argv[i]);

    // check if file exists
    if (f.isNull()) {
      char errstr[0];
      sprintf(errstr, "file \"%s\" is not readable", argv[i]);
      print_error(errstr);
    }

    // print file name
    if (argc > optind + 1) {
      if (i != optind) printf("\n\n");
      printf("## %s ###\n",basename(argv[i]));
    }

    // print basic tags
    if (b_flag) {
      if(f.tag()) {

	TagLib::Tag *tag = f.tag();

	printf("\nbasic tags\n==========\n");
	printf("artist     : \"%s\"\n",tag->artist().toCString(isUTF8));
	printf("album      : \"%s\"\n",tag->album().toCString(isUTF8));
	printf("title      : \"%s\"\n",tag->title().toCString(isUTF8));
	if (tag->year()) {
          printf("year       : \"%04i\"\n",tag->year());
	}
	else {
          printf("year       : \"\"\n");
	}
	printf("genre      : \"%s\"\n",tag->genre().toCString(isUTF8));
	if (tag->track()) {
          printf("track      : \"%03i\"\n",tag->track());
	}
        else {
	  printf("track      : \"\"\n",tag->track());
	}
	printf("comment    : \"%s\"\n",tag->comment().toCString(isUTF8));
      }
      else {
	print_error((char *) "cannot read basic tags");
      }
    }

    // print all tags
    if (c_flag) {
      if (f.file()) {

	TagLib::PropertyMap tags = f.file()->properties();

        unsigned int longest = 0;
        for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
          if (i->first.size() > longest) {
            longest = i->first.size();
          }
        }

	printf("\ncodec specific tags\n===================\n");

        if (tags.isEmpty()) {
	  printf("no tags found in audio file\n");
	}
	else {
          for(TagLib::PropertyMap::ConstIterator i = tags.begin(); i != tags.end(); ++i) {
            for(TagLib::StringList::ConstIterator j = i->second.begin(); j != i->second.end(); ++j) {
              printf("%-*s : \"%s\"\n",longest,i->first.toCString(isUTF8),(*j).toCString(isUTF8));
	    }
          }
        }
      }
    }

    // print audio properties
    if (a_flag) {
      if (f.audioProperties()) {

        TagLib::AudioProperties *properties = f.audioProperties();

        int millisecs = properties->lengthInMilliseconds() % 1000;
        int seconds = (properties->lengthInMilliseconds() - millisecs) / 1000 % 60;
        int minutes = (properties->lengthInMilliseconds() - millisecs - seconds * 1000) / 60000 % 60;
        int hours = (properties->lengthInMilliseconds() - millisecs - seconds * 1000 - minutes * 60000) / 3600000;

        int rseconds = seconds;
        if (millisecs > 500) {
          rseconds++;
        }
        int rminutes = (properties->lengthInMilliseconds() - millisecs - rseconds * 1000) / 60000 % 60;
        int rhours = (properties->lengthInMilliseconds() - millisecs - rseconds * 1000 - rminutes * 60000) / 3600000;

      	printf("\naudio stream info\n=================\n");

        printf("bitrate     : %i kb/s\n", properties->bitrate());
        printf("sample rate : %i Hz\n", properties->sampleRate());
        printf("channels    : %i\n", properties->channels());
        printf("duration    : %02i:%02i:%02i (%02i:%02i:%02i.%03i)\n", rhours, rminutes, rseconds, hours, minutes, seconds, millisecs);
     }
    }
  }

  exit(EXIT_SUCCESS);
}
