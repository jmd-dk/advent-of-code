# ❄️ Advent of Code 2024 🧩
*My solutions to the [2024 Advent of Code](https://adventofcode.com/2024/)
puzzles, written in **C++** (≥ 23)*

[🌟](solution/01/solve.cpp)
[🌟](solution/02/solve.cpp)
[🌟](solution/03/solve.cpp)
[🌟](solution/04/solve.cpp)
[⚫](solution/05/solve.cpp)
[⚫](solution/06/solve.cpp)
[⚫](solution/07/solve.cpp)
[⚫](solution/08/solve.cpp)
[⚫](solution/09/solve.cpp)
[⚫](solution/10/solve.cpp)
[⚫](solution/11/solve.cpp)
[⚫](solution/12/solve.cpp)
[⚫](solution/13/solve.cpp)
[⚫](solution/14/solve.cpp)
[⚫](solution/15/solve.cpp)
[⚫](solution/16/solve.cpp)
[⚫](solution/17/solve.cpp)
[⚫](solution/18/solve.cpp)
[⚫](solution/19/solve.cpp)
[⚫](solution/20/solve.cpp)
[⚫](solution/21/solve.cpp)
[⚫](solution/22/solve.cpp)
[⚫](solution/23/solve.cpp)
[⚫](solution/24/solve.cpp)
[⚫](solution/25/solve.cpp)


</br>

## 📜 Guidelines
The following self-imposed guidelines are adopted:
* C++ **standard library** only (no external libraries).
* Parts one and two are to be solved **separately**.


</br>

## 📁 Structure
Each day gets its own subdirectory within the [solution](solution) directory,
with a single file `solve.cpp`, defining the following functions:
* `read()`: For **reading input** into some appropriate data structure.
  Further data processing should be avoided.
* `solve_one()`: Solution algorithm for **part one**.
* `solve_two()`: Solution algorithm for **part two**.

The full (personalized) inputs are
[not included](https://adventofcode.com/about#legal) in this repository.
You should place your own as files named `input` within the different
subdirectories (and modify the `answer` files accordingly). The `input` files
can also be [retrieved automatically](#-makefile).


</br>

## 🤖 Makefile
With the [`Makefile`](Makefile) you can do the following:
* `make`: Run all solutions.
  * `make <n>`: Run solution for day `n`.
* `make input`: Download all `input` files from
  [adventofcode.com](https://adventofcode.com/2024/). For this to work, you
  need to have logged into [Advent of Code](https://adventofcode.com)
  using Chrome or Firefox.
  * `make input-<n>`: Same as above for day `n`.
* `make format`: Format all solution files
  using [clang-format](https://clang.llvm.org/docs/ClangFormat.html).
  * `make format-<n>`: Same as above for day `n`.
* `make clean`: Cleanup common unwanted, auto-generated files.

> [!NOTE]
> In the above commands, valid values of `<n>` always use two digits
> (so `01` instead of just `1`).


</br>

## 🐋 Docker
The solutions may be run through [Docker](https://www.docker.com/). Before
doing so, make sure to [download](#-makefile) the `input` files.

To e.g. run day 1:
```bash
cd advent-of-code/2024/
docker run --rm -v "${PWD}:/mnt" -w /mnt gcc make 01
```
To run all puzzles:
```bash
cd advent-of-code/2024/
docker run --rm -v "${PWD}:/mnt" -w /mnt gcc make
```

> [!NOTE]
> Remember to [clean](#-makefile) when switching between
> building with and without Docker.

