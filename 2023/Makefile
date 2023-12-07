python ?= python3.12

puzzles = $(shell for d in $$(seq -w 1 25); do [ -d $$d ] && echo $$d; done)

all: $(puzzles)

$(puzzles):
	@$(python) -B $@/solve.py
	@[ $@ = $(lastword $(puzzles)) ] || echo

format = $(addprefix format-,$(puzzles))
$(format):
	@$(python) -B -m black -S $(subst format-,,$@)/solve.py
format:
	@$(python) -B -m black -S .

clean:
	$(RM) -r *__pycache__ */__pycache__
	$(RM) *.swp */*.swp

.PHONY: $(puzzles) $(format) format clean
