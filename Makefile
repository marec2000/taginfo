#
# TagInfo - Makefile
#

INCLUDEDIR=/usr/include
TAGLIB_INCLUDEDIR=$(INCLUDEDIR)/taglib
CC=g++
CXXFLAGS+=
INCLUDES=-I$(TAGLIB_INCLUDEDIR)
LIBS=-ltag

taginfo: main.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)
