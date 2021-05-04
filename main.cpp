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

#if TAGLIB_MAJOR_VERSION >= 1 && TAGLIB_MINOR_VERSION >= 12
#define ID3_LEGACY_GENRE_SUPPORT
#endif

#ifdef ID3_LEGACY_GENRE_SUPPORT
#include <mpegfile.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#endif

#define VERSION_MAJOR 1
#define VERSION_MINOR 3

const char *author = "Markus Reckwerth";
const int copyyear = 2021;

using namespace std;
#ifdef ID3_LEGACY_GENRE_SUPPORT
using namespace TagLib;
#endif

static void print_help (char* pname)
{
#ifdef ID3_LEGACY_GENRE_SUPPORT
  printf("usage: %s [options] <file> ...\n\noptions:\n\t-a\tshow audio stream info\n\t-b\tshow basic tag info\n\t-c\tshow codec specific tag info (default)\n\t-h\tprint help\n\t-l\tshow legacy id3v1 values\n\t-v\tprint version\n", pname);
#else
  printf("usage: %s [options] <file> ...\n\noptions:\n\t-a\tshow audio stream info\n\t-b\tshow basic tag info\n\t-c\tshow codec specific tag info (default)\n\t-v\tprint version\n", pname);
#endif
  exit(EXIT_SUCCESS);
}

static void print_version (char *pname)
{
  printf("%s v%i.%i [taglib v%i.%i] | (c) %i %s\n", pname, VERSION_MAJOR, VERSION_MINOR, TAGLIB_MAJOR_VERSION, TAGLIB_MINOR_VERSION, copyyear, author);
  exit(EXIT_SUCCESS);
}

static void print_error (char *errmsg)
{
  fprintf(stderr,"ERROR: %s\n",errmsg);
  exit(EXIT_FAILURE);
}

#ifdef ID3_LEGACY_GENRE_SUPPORT
static char* get_ancient_genre_name (const char* genre_name)
{
  static const struct {
    const int code;
    const char *actual_genre;
    const char *ancient_genre;
  } genreMap[] = {
    { 29, "Jazz-Funk", "Jazz+Funk" },
    { 81, "Folk Rock", "Folk/Rock" },
    { 85, "Bebop", "Bebob" },
    { 90, "Avant-garde", "Avantgarde" },
    { 125, "Dancehall", "Dance Hall" },
    { 129, "Hardcore Techno", "Hardcore" },
    { 132, "Britpop", "BritPop" },
    { 133, "Worldbeat", "Negerpunk" }
  };

  static const int genreMapSize = sizeof(genreMap) / sizeof(genreMap[0]);
  for(int i = 0; i < genreMapSize; ++i) {
    if (strcmp(genre_name,genreMap[i].actual_genre) == 0)
      return (char *) genreMap[i].ancient_genre;
  }

  return (char *) genre_name;
}
#endif

int main(int argc, char *argv[])
{
  int opt;
  int flags=0;
  int a_flag=0;
  int b_flag=0;
  int c_flag=0;
#ifdef ID3_LEGACY_GENRE_SUPPORT
  int l_flag=0;
#endif

  char *progname = basename(argv[0]);

#ifdef ID3_LEGACY_GENRE_SUPPORT
  const char *genre_name;
#endif
  char *loc = setlocale(LC_CTYPE,"");
  bool isUTF8 = (strstr(loc,"UTF-8") != NULL);

  // loop through arguments
#ifdef ID3_LEGACY_GENRE_SUPPORT
  while ((opt = getopt(argc, argv, "abchlv")) != -1) {
#else
  while ((opt = getopt(argc, argv, "abchv")) != -1) {
#endif
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
#ifdef ID3_LEGACY_GENRE_SUPPORT
      case 'l':
        l_flag=1;
        break;
#endif
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
    char *next_file = argv[i];
    TagLib::FileRef f(next_file);

    // check if file exists
    if (f.isNull()) {
      char errstr[0];
      sprintf(errstr, "file \"%s\" is not readable", next_file);
      print_error(errstr);
    }

    // print file name
    if (argc > optind + 1) {
      if (i != optind) printf("\n\n");
      printf("## %s ###\n",basename(next_file));
    }

#ifdef ID3_LEGACY_GENRE_SUPPORT
    // get id3 tags
    MPEG::File g(next_file);
    ID3v1::Tag *id3v1tag = g.ID3v1Tag();
    ID3v2::Tag *id3v2tag = g.ID3v2Tag();
#endif

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
#ifdef ID3_LEGACY_GENRE_SUPPORT
        if (l_flag && ((id3v1tag) || (id3v2tag))) {
          genre_name = get_ancient_genre_name(tag->genre().toCString(isUTF8));
        }
        else {
          genre_name = tag->genre().toCString(isUTF8);
        }
        printf("genre      : \"%s\"\n",genre_name);
#else
        printf("genre      : \"%s\"\n",tag->genre().toCString(isUTF8));
#endif
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
#ifdef ID3_LEGACY_GENRE_SUPPORT
              if (l_flag && ((id3v1tag) || (id3v2tag))) {
                genre_name = get_ancient_genre_name((*j).toCString(isUTF8));
              }
	      else {
                genre_name = (*j).toCString(isUTF8);
	      }
              printf("%-*s : \"%s\"\n",longest,i->first.toCString(isUTF8),genre_name);
#else
              printf("%-*s : \"%s\"\n",longest,i->first.toCString(isUTF8),(*j).toCString(isUTF8));
#endif
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
        int rminutes = minutes;
        int rhours = hours;

        if (millisecs > 500) {
          rseconds++;
        }

        if (rseconds > 59) {
          rseconds = rseconds % 60;
          rminutes++;
        }

        if (rminutes > 59) {
          rminutes = rminutes % 60;
          rhours++;
        }

        if (rhours > 99) {
          char errstr[0];
          sprintf(errstr, "track duration of file \"%s\" is too long", next_file);
          print_error(errstr);
        }

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
