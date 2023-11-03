#####
CXX 	:= g++
CXXFLAG := -std=c++11 -Wall -Wextra -Wno-sign-compare -g
INC 	:= -I/usr/include/opencv4 \
		   -I/opt/intel/ipp/include \
		   -L/opt/intel/ipp/lib
INCFLAG := $(pkg-config --libs opencv4) -lippcore
#####
OUT := main.out
#####
all: $(OUT)
	
$(OUT): main.cpp
	$(CXX) $(CXXFLAG) $(INC) $(INCFLAG) main.cpp -o $@
#####
clean:
	rm -rf $(OUT)
