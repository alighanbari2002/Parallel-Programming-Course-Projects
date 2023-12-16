CLEAR_DIR_FLAG = --no-print-directory
SUBDIRS := $(wildcard */.)

define submake
	@counter=1;                                             \
	for d in $(SUBDIRS); do                                 \
		if [ "$(MAKECMDGOALS)" = "run" ]; then              \
			printf "===========\n";                         \
			printf "Question #$$counter\n";                 \
			printf "===========\n";                         \
			$(MAKE) $(1) --directory=$$d $(CLEAR_DIR_FLAG); \
		else                                                \
			$(MAKE) $(1) --directory=$$d;                   \
		fi;                                                 \
		counter=$$((counter + 1));                          \
	done
endef
#----------------------------------------------------------------
all: ; $(call submake, $@)

run:    ; $(call submake, $@)
run-Q1: ; @$(MAKE) run --directory="Q1/" $(CLEAR_DIR_FLAG)
run-Q2: ; @$(MAKE) run --directory="Q2/" $(CLEAR_DIR_FLAG)
run-Q3: ; @$(MAKE) run --directory="Q3/" $(CLEAR_DIR_FLAG)
run-Q4: ; @$(MAKE) run --directory="Q4/" $(CLEAR_DIR_FLAG)

clean:    ; $(call submake, $@)
clean-Q1: ; @$(MAKE) clean --directory="Q1/" $(CLEAR_DIR_FLAG)
clean-Q2: ; @$(MAKE) clean --directory="Q2/" $(CLEAR_DIR_FLAG)
clean-Q3: ; @$(MAKE) clean --directory="Q3/" $(CLEAR_DIR_FLAG)
clean-Q4: ; @$(MAKE) clean --directory="Q4/" $(CLEAR_DIR_FLAG)

report:    ; $(call submake,$@ $(CLEAR_DIR_FLAG))
report-Q1: ; @$(MAKE) report --directory="Q1/" $(CLEAR_DIR_FLAG)
report-Q2: ; @$(MAKE) report --directory="Q2/" $(CLEAR_DIR_FLAG)
report-Q3: ; @$(MAKE) report --directory="Q3/" $(CLEAR_DIR_FLAG)
report-Q4: ; @$(MAKE) report --directory="Q4/" $(CLEAR_DIR_FLAG)

help:
	@echo "Available targets:"
	@echo "  all       - Build all executables"
	@echo "  run       - Run all executables"
	@echo "  run-Q1    - Run Q1 executable"
	@echo "  run-Q2    - Run Q2 executable"
	@echo "  run-Q3    - Run Q3 executable"
	@echo "  run-Q4    - Run Q4 executable"
	@echo "  report    - Generate reports for all executables"
	@echo "  report-Q1 - Generate report for Q1 executable"
	@echo "  report-Q2 - Generate report for Q2 executable"
	@echo "  report-Q3 - Generate report for Q3 executable"
	@echo "  report-Q4 - Generate report for Q4 executable"
	@echo "  clean     - Clean all build artifacts"
	@echo "  clean-Q1  - Clean Q1 build artifacts"
	@echo "  clean-Q2  - Clean Q2 build artifacts"
	@echo "  clean-Q3  - Clean Q3 build artifacts"
	@echo "  clean-Q4  - Clean Q4 build artifacts"
	@echo "  help      - Show this help message"
#------------------------------------------------------------------------------------------
.PHONY: all $(SUBDIRS) \
		run run-Q1 run-Q2 run-Q3 run-Q4 \
		report report-Q1 report-Q2 report-Q3 report-Q4 \
		clean clean-Q1 clean-Q2 clean-Q3 clean-Q4 \
		help
