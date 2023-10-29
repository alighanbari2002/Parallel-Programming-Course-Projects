# ------------------------------------------------------------------
PATH_BIN     := bin
PATH_OBJ     := $(PATH_BIN)/obj
# ------------------------------------------------------------------
RM           = rm -f
RMDIR        = rm -rf
MKDIR        = @mkdir -p
# ------------------------------------------------------------------
CC           = g++
CFLAGS       = -c -msse4.1
INCDIR       = -I/opt/intel/ipp/include
LIBDIR       = -L/opt/intel/ipp/lib
LIBFILE      = -lippcore
# ------------------------------------------------------------------
OUT_EXE      = program
OUT_TXT      = output.txt
# ------------------------------------------------------------------
all: pre_build build

pre_build:
	$(MKDIR) $(PATH_OBJ)

build: $(PATH_OBJ)/main.o
	$(CC) $(PATH_OBJ)/main.o -o $(PATH_BIN)/$(OUT_EXE)
# ------------------------------------------------------------------
$(PATH_OBJ)/main.o: main.cpp
	$(CC) $(CFLAGS) $(INCDIR) main.cpp -o $@
# ------------------------------------------------------------------
.PHONY: all\
		clean clean-obj clean-exe clean-out delete-bin\
		run out\
		help

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
