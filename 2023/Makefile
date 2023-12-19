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
	@for demo in $(shell grep -i '# *demo *' solution/$(subst check-,,$@)/demo-input | awk '{print $$NF}'); do \
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

input = $(addprefix input-,$(puzzles))
input: $(input)
$(input):
	$(if $(filter "$(AOC_SESSION)",""),$(call set_token),)
	@AOC_SESSION=$(AOC_SESSION) $(python) -c "pass; \
	    import aocd, pathlib; \
	    path = pathlib.Path('solution/$(subst input-,,$@)/input'); \
	    text_prev = path.read_text() if path.is_file() else None; \
	    text = aocd.get_data(year=2023, day=int('$(subst input-,,$@)')); \
	    new = text != text_prev; \
	    path.write_text(text) if new else None; \
	    print('Retrieved input for day $(subst input-,,$@)') if new else None; \
	"
define set_token
    @$(call install_pypackage,advent-of-code-data,aocd)
    @$(call install_pypackage,browser-cookie3,browser_cookie3)
    @$(eval AOC_SESSION = $(shell $(python) -c "pass; \
        import aocd; \
        tokens = aocd.cookies.get_working_tokens(); \
        token = next(iter(tokens)); \
        print(token); \
    "))
endef
define install_pypackage
    $(python) -c "import $2" 2>/dev/null \
        || $(python) -m pip install $1 \
        || (echo "Please install the '$1' Python package:" \
            && echo "    $(python) -m pip install $1" >&2 \
            && return 1 \
        )
endef

clean:
	$(RM) -r __pycache__ */__pycache__ */*/__pycache__
	$(RM) *.swp */*.swp */*/*.swp

.PHONY: $(puzzles) $(format) format $(check) check dist clean
