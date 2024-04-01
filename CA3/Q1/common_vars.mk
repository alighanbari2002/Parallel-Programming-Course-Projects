CXX      = g++
CXXFLAGS += -Wall -pedantic -fopenmp -O3 -march=native -mtune=native -ffast-math -funroll-loops -fprefetch-loop-arrays -ftree-vectorize -flto -fomit-frame-pointer
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
