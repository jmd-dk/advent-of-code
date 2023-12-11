python ?= python3.12

MAKEFLAGS += --no-print-directory

puzzles = $(shell for d in $$(seq -w 1 25); do [ -d solution/$$d ] && echo $$d; done)

all: puzzles

puzzles: $(puzzles)
$(puzzles):
	@$(python) -B solution/$@/solve.py
	@echo
$(addprefix puzzle-,$(puzzles)):
	@$(MAKE) $(subst puzzle-,,$@)

format = $(addprefix format-,$(puzzles))
format:
	@$(python) -B -m black -S .
$(format):
	@$(python) -B -m black -S solution/$(subst format-,,$@)/solve.py

check = $(addprefix check-,$(puzzles))
check: $(check)
$(check):
	@for demo in $(shell grep -i '# *demo *' solution/$(subst check-,,$@)/demo-input.txt | awk '{print $$NF}'); do \
	    echo Demo $${demo}; \
	    DEMO=$${demo} $(call perform_check); \
	done
	@echo Full input: && $(call perform_check)
define perform_check
    $(MAKE) $(subst check-,,$@) 2>/dev/null | tee /dev/tty | grep ✘ | wc -l | grep 0 >/dev/null
endef

dist:
	@$(MAKE) clean
	@echo
	@$(MAKE) format
	@echo
	@$(MAKE) check
	@[ -z "$(shell git status -s 2>/dev/null)" ] || git status
	@[ -n "$(shell git status -s 2>/dev/null)" ] || echo All good! ✨

clean:
	$(RM) -r __pycache__ */__pycache__
	$(RM) *.swp */*.swp

.PHONY: $(puzzles) $(format) format $(check) check dist clean
