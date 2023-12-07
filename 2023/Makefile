python ?= python3.12

MAKEFLAGS += --no-print-directory

puzzles = $(shell for d in $$(seq -w 1 25); do [ -d $$d ] && echo $$d; done)

all: puzzles

puzzles: $(puzzles)
$(puzzles):
	@$(python) -B $@/solve.py
	@echo
$(addprefix puzzle-,$(puzzles)):
	@$(MAKE) $(subst puzzle-,,$@)

format = $(addprefix format-,$(puzzles))
format:
	@$(python) -B -m black -S .
$(format):
	@$(python) -B -m black -S $(subst format-,,$@)/solve.py

check = $(addprefix check-,$(puzzles))
check: $(check)
$(check):
	@echo Demo: && DEMO=1 $(call perform_check)
	@test ! -f $(subst check-,,$@)/demo2-input.txt || (echo Demo2: && DEMO=2 $(call perform_check))
	@echo Full input: && $(call perform_check)
define perform_check
    $(MAKE) $(subst check-,,$@) 2>/dev/null | tee /dev/tty | grep ✔ | wc -l | grep 2 >/dev/null
endef

dist:
	@$(MAKE) clean
	@echo
	@$(MAKE) format
	@echo
	@$(MAKE) check
	@[ -z "$(shell git status -s 2>/dev/null)" ] || git status
	@[ -n $(git status -s) ] || echo All good! ✨

clean:
	$(RM) -r *__pycache__ */__pycache__
	$(RM) *.swp */*.swp

.PHONY: $(puzzles) $(format) format $(check) check dist clean
