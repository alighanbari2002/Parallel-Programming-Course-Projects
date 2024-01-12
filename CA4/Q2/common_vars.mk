CXX      = g++
CXXFLAGS += -Wall -Wno-multichar -pthread
CXX      += $(CXXFLAGS)
CPPFLAGS += -std=c++11

RM     = rm -f
RMDIR  = rm -rf
MKDIR  = @mkdir -p
COPY   = @cp
MOVE   = @mv -f
RENAME = @mv -f
NULL_DEVICE = /dev/null
