#####
CXX 	:= g++
CXXFLAG := -std=c++11 -Wall -Wextra -Wno-sign-compare -g -msse4.1
INC 	:= -I/usr/include/opencv4
INCFLAG := $$(pkg-config --libs opencv4) #-lippcore
#####
OUT := main.out
#####
all: $(OUT)
	
$(OUT): main.cpp
	$(CXX) $(CXXFLAG) main.cpp -o $@ $(INC) $(INCFLAG)
#####
clean:
	rm -rf $(OUT)