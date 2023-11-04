# Paths
PATH_BIN     := bin
PATH_OBJ     := $(PATH_BIN)/obj
PATH_OUT	 := out

# Commands
RM           := rm -f
RMDIR        := rm -rf
MKDIR        := @mkdir -p

# Compiler
CXX          := g++
CXXFLAGS     := -std=c++11 -Wall -Wextra -Wno-sign-compare -g -msse4.1
INCDIR       := -I/usr/include/opencv4 ## -I/opt/intel/ipp/include
LIBDIR       := -L/opt/intel/ipp/lib
LIBFILE      := $$(pkg-config --libs opencv4) ## -lippcore 

# Output files
OUT_EXE      := program
OUT_TXT      := output.txt
# ------------------------------------------------------------------
.PHONY: all\
		clean clean-obj clean-exe clean-out delete-bin\
		run out\
		help

all: pre_build build

pre_build:
	$(MKDIR) $(PATH_OBJ)
	$(MKDIR) $(PATH_OUT)

build: $(PATH_BIN)/$(OUT_EXE)

$(PATH_BIN)/$(OUT_EXE): $(PATH_OBJ)/main.o
	$(CXX) $< -o $@

$(PATH_OBJ)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(INCDIR) $(LIBFILE)

clean: clean-obj clean-exe delete-bin

clean-obj:
	$(RM) $(PATH_OBJ)/*

clean-exe:
	$(RM) $(PATH_BIN)/$(OUT_EXE)

clean-out:
	$(RMDIR) $(PATH_OUT)

delete-bin:
	$(RMDIR) $(PATH_BIN)

run: ; @cd $(PATH_BIN) && ./$(OUT_EXE)

out: ; @cd $(PATH_BIN) && ./$(OUT_EXE) > ../$(PATH_OUT)/$(OUT_TXT)

help:
	@echo Targets: all clean clean-obj clean-exe clean-out delete-bin run out
# ------------------------------------------------------------------