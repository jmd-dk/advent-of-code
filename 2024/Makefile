MAKEFLAGS += --no-print-directory

ifeq ($(origin CXX),default)
    ifneq ($(shell which clang++),)
        CXX = clang++
    else
        CXX = g++
    endif
endif

CXXFLAGS += -std=c++23 -O2 -Wall -Wextra -Wpedantic

python ?= python3

year = $(notdir $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST))))))

puzzles = $(shell for d in $$(seq -w 1 25); do [ -d solution/$$d ] && echo $$d; done)

all: puzzles

puzzles: $(addprefix puzzle-,$(puzzles))
puzzle-%: solution/%/solve
	@cd $(dir $<) && ./$(notdir $<)
.PRECIOUS: solution/%/solve
solution/%/solve: solution/%/solve.cpp magic.h $(MAKEFILE_LIST)
	$(CXX) $(CXXFLAGS) $< -o $@
$(puzzles):
	@$(MAKE) puzzle-$@

format_cmd := clang-format -i -style="{BasedOnStyle: Google, IndentWidth: 4, AccessModifierOffset: -2, ColumnLimit: 92}"
format = $(addprefix format-,$(puzzles))
format: $(format)
	$(format_cmd) magic.h
$(format):
	$(format_cmd) solution/$(subst format-,,$@)/solve.cpp

input = $(addprefix input-,$(puzzles))
input: $(input)
$(input):
	$(if $(filter "$(AOC_SESSION)",""),$(call set_token),)
	@AOC_SESSION=$(AOC_SESSION) $(python) -c "pass; \
	    import aocd, pathlib; \
	    path = pathlib.Path('solution/$(subst input-,,$@)/input'); \
	    text_prev = path.read_text() if path.is_file() else None; \
	    text = aocd.get_data(year=$(year), day=int('$(subst input-,,$@)')); \
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
	$(RM) $(foreach puzzle,$(puzzles),solution/$(puzzle)/solve)

.PHONY: $(puzzles) clean
