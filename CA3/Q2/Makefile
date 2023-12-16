PATH_SRC    := src
PATH_BUILD  := build
PATH_BIN    := bin
PATH_OUTPUT := output
PATH_OBJ    := $(PATH_BUILD)/obj
PATH_DEP    := $(PATH_BUILD)/dep
PATH_LIB    := lib

include common_vars.mk

LDFLAGS  += -fopenmp
LDLIBS   += # -l
CPPFLAGS += -fopenmp

OUT_EXE := program.exe
OUT_TXT := report.txt
#------------------------------------------------------------------------------------------
VPATH = $(PATH_SRC)

FILES   = $(patsubst src/%, %, $(shell find $(PATH_SRC) -name "*.cpp" -type f))
FOLDERS = $(patsubst src/%, %, $(shell find $(PATH_SRC) -mindepth 1 -type d))

FILES_DEP = $(patsubst %, $(PATH_DEP)/%.d, $(basename $(FILES)))
FILES_OBJ = $(patsubst %, $(PATH_OBJ)/%.o, $(basename $(FILES)))
#------------------------------------------------------------------------------------------
all: $(PATH_BIN)/$(OUT_EXE)

$(PATH_BIN)/$(OUT_EXE): $(FILES_OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

DEPFLAGS    = -MT $@ -MMD -MP -MF $(PATH_DEP)/$*.dTMP
POSTCOMPILE = @$(MOVE) $(PATH_DEP)/$*.dTMP $(PATH_DEP)/$*.d > $(NULL_DEVICE) && touch $@

$(PATH_OBJ)/%.o: %.cpp
$(PATH_OBJ)/%.o: %.cpp $(PATH_DEP)/%.d | directories
	$(CXX) $(CPPFLAGS) -c $(DEPFLAGS) $< -o $@
	$(POSTCOMPILE)

.PRECIOUS: $(FILES_DEP)
$(FILES_DEP): ;
-include $(FILES_DEP)
#------------------------------------------------------------------------------------------
directories: $(PATH_BUILD) $(PATH_BIN) $(PATH_OBJ) $(PATH_DEP) nested-folders
nested-folders: $(addprefix $(PATH_OBJ)/, $(FOLDERS)) $(addprefix $(PATH_DEP)/, $(FOLDERS))

$(PATH_BUILD):  ; $(MKDIR) $@
$(PATH_BIN):    ; $(MKDIR) $@
$(PATH_OUTPUT): ; $(MKDIR) $@
$(PATH_OBJ):    ; $(MKDIR) $@
$(PATH_DEP):    ; $(MKDIR) $@

$(addprefix $(PATH_OBJ)/, $(FOLDERS)): ; @$(MKDIR) $@
$(addprefix $(PATH_DEP)/, $(FOLDERS)): ; @$(MKDIR) $@
#------------------------------------------------------------------------------------------
.PHONY: all directories nested-folders \
		clean clean-obj clean-dep clean-exe delete-build delete-output \
		run report help

clean: clean-obj clean-dep clean-exe delete-build
clean-obj:     ; $(RM) $(PATH_OBJ)/*
clean-dep:     ; $(RM) $(PATH_DEP)/*
clean-exe:     ; $(RM) $(PATH_BIN)/$(OUT_EXE)
delete-build:  ; $(RMDIR) $(PATH_BUILD)
delete-output: ; $(RMDIR) $(PATH_OUTPUT)

ARGS ?=
run: ; @cd $(PATH_BIN) && ./$(OUT_EXE) $(ARGS)

report: $(PATH_OUTPUT)
	@cd $(PATH_BIN) && ./$(OUT_EXE) $(ARGS) > ../$(PATH_OUTPUT)/$(OUT_TXT)

help:
	@echo Targets: all clean clean-obj clean-dep clean-exe delete-build delete-output run report
	@echo '(make run ARGS="arg1 arg2...")'
