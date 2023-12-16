# 🎄 Advent of Code 2023 🧩
*My solutions to the [2023 Advent of Code](https://adventofcode.com/2023/)
puzzles, written in **Python 3.12*** 🐍

[🌟](solution/01/solve.py)
[🌟](solution/02/solve.py)
[🌟](solution/03/solve.py)
[🌟](solution/04/solve.py)
[🌟](solution/05/solve.py)
[🌟](solution/06/solve.py)
[🌟](solution/07/solve.py)
[🌟](solution/08/solve.py)
[🌟](solution/09/solve.py)
[🌟](solution/10/solve.py)
[🌟](solution/11/solve.py)
[🌟](solution/12/solve.py)
[🌟](solution/13/solve.py)
[🌟](solution/14/solve.py)
[🌟](solution/15/solve.py)
[🌟](solution/16/solve.py)
[⚫](https://adventofcode.com/2023/day/17)
[⚫](https://adventofcode.com/2023/day/18)
[⚫](https://adventofcode.com/2023/day/19)
[⚫](https://adventofcode.com/2023/day/20)
[⚫](https://adventofcode.com/2023/day/21)
[⚫](https://adventofcode.com/2023/day/22)
[⚫](https://adventofcode.com/2023/day/23)
[⚫](https://adventofcode.com/2023/day/24)
[⚫](https://adventofcode.com/2023/day/25)


</br>

## 📜 Guidelines adopted
* Python **standard library** only (no external packages).
* Parts one and two are to be solved **separately**.
* **Clean code**, with complex optimizations only as far as needed
  to reach **execution times < 1 second**.

Within the [solution](solution) directory, each day gets its own subdirectory
with a single Python script `solve.py`, defining the following functions:
* `read()`: For **reading input** into some appropriate data structure.
  Further data processing should be avoided.
* `solve_one()`: Solution algorithm for **part one**.
* `solve_two()`: Solution algorithm for **part two**.
* `get_initial()` (optional): For setting **initial conditions**.

> [!NOTE]
> Having `solve_one()` and `solve_two()` be separate with no code shared
> naturally leads to code repetition. However, previous year's experience
> has shown that the simplicity of part one is often tainted by allowing
> code share.


</br>

## ✨ Magic
In [`magic.py`](magic.py) we define helper functionality
used by the solutions:
* `open_input()`: For opening input files (full puzzle input by default,
  demo input selectable through the `DEMO` environment variable; `DEMO=1`,
  `DEMO=2`, ...).
* `memoize()`: Caching functionality, always emitting fresh copies. This is
  used with e.g. `read()` so that both part one and two can simply call this
  function, without the input file being read twice. As fresh copies are used,
  part two does not have to worry about the input data
  being mutated in part one.
* `analyze()`: For easily timing the solution algorithms, comparing the
  solutions to the correct answers and pretty printing the results.

> [!TIP]
> The magic can be removed simply by commenting out the `memoize()` and
> `analyze()` decorators and replacing the `open_input()` call with
> `open('input')`, useful for quick copy-pasting of the solution code,
> without having to deal with the additional [`magic.py`](magic.py) file.


</br>

## 🤖 Makefile
With the [`Makefile`](Makefile) you can do the following:
* `make`: Run all solutions.
  * `make <n>`: Run solution for day `n`.
* `make format`: Format all solution files as well as [`magic.py`](magic.py)
  using the [Black](https://github.com/psf/black) formatter.
  * `make format-<n>`: Format solution file for day `n`.
* `make check`: Run all solutions and compare the results to known answers.
  * `make check-<n>`: Run solution for day `n` and compare the result to
    known answer.
* `make clean`: Cleanup common unwanted, auto-generated files.
* `make dist`: Run `make clean`, `make format`, `make check` and `git status`.


</br>

## 🐋 Running with Docker
The solutions may be run through [Docker](https://www.docker.com/).
To e.g. run day 1, do
```bash
cd advent-of-code/2023/
docker run --rm -v "${PWD}:/mnt" -w /mnt python:3.12 make 01
```
To run all puzzles, you can use
```bash
cd advent-of-code/2023/
docker run --rm -v "${PWD}:/mnt" -w /mnt python:3.12 make
```
To run the exhaustive `dist` target on the newest Python 3.12
with upgraded packages:
```bash
cd advent-of-code/
docker run -t --rm --pull always -v "${PWD}:/mnt" -w /mnt/2023 python:3.12 bash -c "pip install -U pip && pip freeze > requirements.txt && sed -i 's/==/>=/' requirements.txt && echo black >> requirements.txt && pip install -U -r requirements.txt && rm requirements.txt && git config --global --add safe.directory /mnt && make dist"
```
