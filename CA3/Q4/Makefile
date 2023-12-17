PATH_SRC    := src
PATH_BIN    := bin
PATH_OUTPUT := output
PATH_LIB    := $(shell pkg-config --cflags opencv4)

include common_vars.mk

LDFLAGS  += $(shell pkg-config --libs opencv4)
LDLIBS   += # -l
CPPFLAGS += -fopenmp $(PATH_LIB)

OUT_EXE := program.exe
OUT_TXT := report.txt
#------------------------------------------------------------------------------------------
VPATH = $(PATH_SRC)

FILES   = $(patsubst src/%, %, $(shell find $(PATH_SRC) -name "*.cpp" -type f))
FOLDERS = $(patsubst src/%, %, $(shell find $(PATH_SRC) -mindepth 1 -type d))
#------------------------------------------------------------------------------------------
all: directories $(PATH_BIN)/$(OUT_EXE)

$(PATH_BIN)/$(OUT_EXE): $(FILES)
	$(CXX) $(CPPFLAGS) $^ $(LDFLAGS) -o $@
#------------------------------------------------------------------------------------------
directories: $(PATH_BIN) $(PATH_OUTPUT)

$(PATH_BIN):    ; $(MKDIR) $@
$(PATH_OUTPUT): ; $(MKDIR) $@
#------------------------------------------------------------------------------------------
.PHONY: all directories \
		clean delete-output \
		run report help

clean:         ; $(RM) $(PATH_BIN)/$(OUT_EXE)
delete-output: ; (RMDIR) $(PATH_OUTPUT)

ARGS ?=
run:    ; @cd $(PATH_BIN) && ./$(OUT_EXE) $(ARGS)
report: ; @cd $(PATH_BIN) && ./$(OUT_EXE) $(ARGS) > ../$(PATH_OUTPUT)/$(OUT_TXT)

help:
	@echo Targets: all clean delete-output run report
	@echo '(make run ARGS="arg1 arg2...")'
