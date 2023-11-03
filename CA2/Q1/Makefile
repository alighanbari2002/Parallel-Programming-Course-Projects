# Paths
PATH_BIN     := bin
PATH_OBJ     := $(PATH_BIN)/obj

# Commands
RM           = rm -f
RMDIR        = rm -rf
MKDIR        = @mkdir -p

# Compiler
CC           = g++
CFLAGS       = -c -msse4.1
INCDIR       = -I/opt/intel/ipp/include
LIBDIR       = -L/opt/intel/ipp/lib
LIBFILE      = -lippcore

# Output files
OUT_EXE      = program
OUT_TXT      = output.txt
# ------------------------------------------------------------------
.PHONY: all\
		clean clean-obj clean-exe clean-out delete-bin\
		run out\
		help

all: pre_build build

pre_build:
	$(MKDIR) $(PATH_OBJ)

build: $(PATH_BIN)/$(OUT_EXE)

$(PATH_BIN)/$(OUT_EXE): $(PATH_OBJ)/main.o
	$(CC) $< -o $@

$(PATH_OBJ)/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@

clean: clean-obj clean-exe delete-bin

clean-obj:
	$(RM) $(PATH_OBJ)/*

clean-exe:
	$(RM) $(PATH_BIN)/$(OUT_EXE)

clean-out:
	$(RM) $(OUT_TXT)

delete-bin:
	$(RMDIR) $(PATH_BIN)

run: ; @cd $(PATH_BIN) && ./$(OUT_EXE)

out: ; @cd $(PATH_BIN) && ./$(OUT_EXE) > ../$(OUT_TXT)

help:
	@echo Targets: all clean clean-obj clean-exe clean-out delete-bin run out
# ------------------------------------------------------------------
