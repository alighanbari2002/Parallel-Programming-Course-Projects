CXX      = g++
CXXFLAGS += -Wall -pedantic -pthread -O1 -ftree-vectorize -flto
CXX      += $(CXXFLAGS)
CPPFLAGS += -std=c++11

AR      = ar
ARFLAGS += rvs

RM     = rm -f
RMDIR  = rm -rf
MKDIR  = mkdir -p
COPY   = cp
MOVE   = mv -f
RENAME = mv -f

NULL_DEVICE = /dev/null
