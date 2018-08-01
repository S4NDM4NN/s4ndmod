CC = gcc
CXX = g++
COMMON_INCLUDE = Omnibot/Common/
RTCW_INCLUDE = Omnibot/rtcw/
CFLAGS = -O3 -DOMNIBOT -D__linux__ -DGAMEDLL -DGLOBALSTATS -w -fno-strict-aliasing -fPIC -m32
CXXFLAGS = -I$(COMMON_INCLUDE) -I$(RTCW_INCLUDE) -fpermissive $(CFLAGS)
LDFLAGS = -m32

CSOURCES = $(shell ls S4ND_*.c g_*.c  q_*.c bg_animation.c bg_misc.c bg_pmove.c bg_slidemove.c md5.c geoip.c)
CXXSOURCES = $(shell ls *.cpp Omnibot/Common/BotLoadLibrary.cpp)

all: gameclean qagame.mp.i386.so clean

gameclean:
	rm -rf *.o *.oxx Omnibot/Common/*.oxx

qagame.mp.i386.so: $(CXXSOURCES:.cpp=.oxx) $(CSOURCES:.c=.o)
	$(CXX) -lpthread -s -shared -Wl,-soname,$@ -o $@ $?

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

%.oxx: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.oxx Omnibot/Common/*.oxx
