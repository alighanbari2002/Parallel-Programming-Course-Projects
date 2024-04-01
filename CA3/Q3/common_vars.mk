CXX      = g++
CXXFLAGS += -Wall -pedantic -fopenmp -fprefetch-loop-arrays -ftree-vectorize -flto -fomit-frame-pointer
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
