CXX      = g++
CXXFLAGS += -Wall -pedantic -msse3 -O2 -march=native -mtune=native -ffast-math -fprefetch-loop-arrays -ftree-vectorize -funroll-loops -flto -fomit-frame-pointer
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
