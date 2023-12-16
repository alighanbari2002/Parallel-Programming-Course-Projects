SUBDIRS := $(wildcard */.)
CLEAR_DIR_FLAG = --no-print-directory

define submake
	@counter=1;                                             \
	for d in $(SUBDIRS); do                                 \
		if [ "$(MAKECMDGOALS)" = "run" ]; then              \
			printf "===========\n";                         \
			printf "Question #$$counter\n";                 \
			printf "===========\n";                         \
		fi;                                                 \
		$(MAKE) $(1) --directory=$$d;                       \
		counter=$$((counter + 1));                          \
	done
endef
#----------------------------------------------------------------
all: ; $(call submake, $@)

run:    ; $(call submake, $@ $(CLEAR_DIR_FLAG))
run-Q1: ; @$(MAKE) run --directory="Q1/" $(CLEAR_DIR_FLAG)
run-Q2: ; @$(MAKE) run --directory="Q2/" $(CLEAR_DIR_FLAG)

clean:    ; $(call submake, $@)
clean-Q1: ; @$(MAKE) clean --directory="Q1/" $(CLEAR_DIR_FLAG)
clean-Q2: ; @$(MAKE) clean --directory="Q2/" $(CLEAR_DIR_FLAG)

report:    ; $(call submake, $@ $(CLEAR_DIR_FLAG))
report-Q1: ; @$(MAKE) report --directory="Q1/" $(CLEAR_DIR_FLAG)
report-Q2: ; @$(MAKE) report --directory="Q2/" $(CLEAR_DIR_FLAG)

help:
	@echo "Available targets:"
	@echo "  all       - Build all executables"
	@echo "  run       - Run all executables"
	@echo "  run-Q1    - Run Q1 executable"
	@echo "  run-Q2    - Run Q2 executable"
	@echo "  report    - Generate reports for all executables"
	@echo "  report-Q1 - Generate report for Q1 executable"
	@echo "  report-Q2 - Generate report for Q2 executable"
	@echo "  clean     - Clean all build artifacts"
	@echo "  clean-Q1  - Clean Q1 build artifacts"
	@echo "  clean-Q2  - Clean Q2 build artifacts"
	@echo "  help      - Show this help message"
#------------------------------------------------------------------------------------------
.PHONY: all $(SUBDIRS) \
		run run-Q1 run-Q2 \
		report report-Q1 report-Q2 \
		clean clean-Q1 clean-Q2 \
		help
