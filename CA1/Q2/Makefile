# ------------------------------------------------------------------
PATH_BIN     := bin
PATH_OBJ     := $(PATH_BIN)/obj
# ------------------------------------------------------------------
RM           = rm -f
RMDIR        = rm -rf
MKDIR        = @mkdir -p
# ------------------------------------------------------------------
CC           = gcc -std=c11
CFLAGS       = -c
INCDIR       = -I.
# ------------------------------------------------------------------
OUT_EXE      = Output
# ------------------------------------------------------------------
all: pre_build build

pre_build:
	$(MKDIR) $(PATH_OBJ)

build: $(PATH_OBJ)/main.o
	$(CC) $(PATH_OBJ)/main.o -o $(PATH_BIN)/$(OUT_EXE)
# ------------------------------------------------------------------
$(PATH_OBJ)/main.o: main.c
	$(CC) $(CFLAGS) $(INCDIR) main.c -o $@
# ------------------------------------------------------------------
.PHONY: all\
		clean clean-obj clean-exe delete-bin\
		run help

clean: clean-obj clean-exe delete-bin

clean-obj:
	$(RM) $(PATH_OBJ)/*

clean-exe:
	$(RM) $(PATH_BIN)/$(OUT_EXE)

delete-bin:
	$(RMDIR) $(PATH_BIN)

run: ; @cd $(PATH_BIN) && ./$(OUT_EXE)

help:
	@echo Targets: all clean clean-obj clean-exe delete-bin run
# ------------------------------------------------------------------
