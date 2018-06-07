#
# TagInfo - Makefile
#

INCLUDEDIR=/usr/include
TAGLIB_INCLUDEDIR=$(INCLUDEDIR)/taglib
CC=g++
CFLAGS=-std=c++98
INCLUDES=-I$(TAGLIB_INCLUDEDIR)
LIBS=-ltag

taginfo: main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -o $@ $^
