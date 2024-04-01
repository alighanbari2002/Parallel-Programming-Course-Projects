PATH_SRC    := src
PATH_LIB    := lib
PATH_OUTPUT := output
PATH_BUILD  := build
PATH_BIN    := bin
PATH_OBJ    := $(PATH_BUILD)/obj
PATH_DEP    := $(PATH_BUILD)/dep

include common_vars.mk

LDFLAGS  += -flto
LDLIBS   += # -l
CPPFLAGS += -I $(PATH_LIB)

OUT_EXE := program.exe
OUT_TXT := report.txt
LOG_TXT := .log.txt
#------------------------------------------------------------------------------------------
VPATH = $(PATH_SRC):$(PATH_LIB)

FILES   = $(patsubst src/%, %, $(shell find $(PATH_SRC) -name "*.cpp" -type f)) \
		  $(patsubst lib/%, %, $(shell find $(PATH_LIB) -name "*.cpp" -type f))
FOLDERS = $(patsubst src/%, %, $(shell find $(PATH_SRC) -mindepth 1 -type d))

FILES_DEP = $(patsubst %, $(PATH_DEP)/%.d, $(basename $(FILES)))
FILES_OBJ = $(patsubst %, $(PATH_OBJ)/%.o, $(basename $(FILES)))
#------------------------------------------------------------------------------------------
all: $(PATH_BIN)/$(OUT_EXE)

$(PATH_BIN)/$(OUT_EXE): $(FILES_OBJ)
	$(CXX) $^ $(LDFLAGS) $(LDLIBS) -o $@

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

$(PATH_OUTPUT): ; @$(MKDIR) $@
$(PATH_BUILD):  ; @$(MKDIR) $@
$(PATH_BIN):    ; @$(MKDIR) $@
$(PATH_OBJ):    ; @$(MKDIR) $@
$(PATH_DEP):    ; @$(MKDIR) $@

$(addprefix $(PATH_OBJ)/, $(FOLDERS)): ; @$(MKDIR) $@
$(addprefix $(PATH_DEP)/, $(FOLDERS)): ; @$(MKDIR) $@
#------------------------------------------------------------------------------------------
.PHONY: all directories nested-folders \
		clean clean-obj clean-dep clean-exe clean-report clean-log \
		delete-build delete-bin delete-output \
		run report runlog applylog help

clean: clean-obj clean-dep clean-exe
clean-obj:     ; $(RM) $(PATH_OBJ)/*
clean-dep:     ; $(RM) $(PATH_DEP)/*
clean-exe:     ; $(RM) $(PATH_BIN)/$(OUT_EXE)
clean-report:  ; $(RM) $(PATH_OUTPUT)/$(OUT_TXT)
clean-log:     ; $(RM) $(PATH_OUTPUT)/$(LOG_TXT)
delete-build:  ; $(RMDIR) $(PATH_BUILD)
delete-bin:    ; $(RMDIR) $(PATH_BIN)
delete-output: ; $(RMDIR) $(PATH_OUTPUT)

ARGS ?=
define EXECUTE_BINARY
	@cd $(PATH_BIN) && ./$(OUT_EXE) $(ARGS)
endef

run:    ; $(EXECUTE_BINARY)
report: ; $(EXECUTE_BINARY) > ../$(PATH_OUTPUT)/$(OUT_TXT)
runlog: ; @$(EXECUTE_BINARY) | tee ../$(PATH_OUTPUT)/$(LOG_TXT)

applylog: ; @cd $(PATH_OUTPUT) &&                       \
	if [ -f $(LOG_TXT) ] && [ -s $(LOG_TXT) ]; then     \
		$(MOVE) $(LOG_TXT) $(OUT_TXT);                  \
	else                                                \
		echo "Log file does not exist or is empty.";    \
	fi

report runlog applylog: | $(PATH_OUTPUT)

help:
	@echo Targets: all clean clean-obj clean-dep clean-exe clean-report clean-log \
				   delete-build delete-bin delete-output run report runlog applylog
	@echo '(make run ARGS="arg1 arg2...")'
