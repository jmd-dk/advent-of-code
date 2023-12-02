## Advent of code 2023
My solutions to the [2023 advent of code](https://adventofcode.com/2023/),
written in Python 3.12.


### Guidelines adopted
* Python standard library only (no additional packages).
* Parts one and two are to be solved separately.

Each day gets its own subdirectory with a single Python script
called `solve.py`, defining the following functions:
* `read()`: For reading input into some appropriate data structure.
* `solve_one()`: Solution algorithm for part one.
* `solve_two()`: Solution algorithm for part two.
* `get_initial()` (optional): For setting initial conditions.


### Magic
In [`magic.py`](magic.py) we define helper functionality used by
the solutions:
* `open_input_file()`: For opening input files (full puzzle input by default,
  demo input selectable through the `DEMO` environment variable).
* `memoize()`: Caching functionality, always emitting fresh copies. This is
  used with e.g. `read()` so that both part one and two can simply call this
  function, without the input file being read twice. As fresh copies are used,
  part two does not have to worry about the input data
  being mutated in part one.
* `analyze()`: For easily timing the solution algorithms
  and pretty printing the results.

