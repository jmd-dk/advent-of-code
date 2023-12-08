"""This module implements common, *magic* functionality.
As this module is meant to be star-imported, each function below
should import what it needs internally.
"""


# Function returning an open context manager
# for the reading of the input file.
def open_input_file():
    import contextlib, inspect, pathlib

    @contextlib.contextmanager
    def open_file(path):
        with open(path) as file:
            yield file

    directory = pathlib.Path(inspect.stack()[1][1]).parent
    return open_file(directory / get_input_filename())


# Function returning the input data file in accordance
# with to the DEMO environment variable.
def get_input_filename():
    """The input file name is determined as follows:
    Default: input.txt
    DEMO environment variable set to 1: demo-input.txt
    DEMO environment variable set to <n> > 1: demo<2>-input.txt
    """
    import os

    demo = os.environ.get('DEMO', '').lower()
    is_demo1 = demo in {'1', 'one', 't', 'true', 'y', 'yes'}
    is_demo2 = demo in {'2', 'two'}
    filename = ''
    filename += 'demo-' * is_demo1
    filename += 'demo2-' * is_demo2
    filename += f'demo{demo}-' * (not filename and demo.isdecimal())
    filename += 'input.txt'
    return filename


# Decorator cache which always returns a fresh copy
def memoize(func):
    import copy, functools

    cached_func = functools.cache(func)

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        return copy.deepcopy(cached_func(*args, **kwargs))

    return wrapper


# Decorator for automatically timing solutions and handling exceptions.
# The return value will be wrapped in a nice Solution container.
def analyze(func):
    import datetime, functools, inspect, pathlib, time, traceback, typing

    class Solution(typing.NamedTuple):
        day: int
        part: str
        value: object
        correct: bool
        time: float

        def __str__(self):
            width = 42
            esc = '\x1b'
            esc_normal = f'{esc}[0m'
            esc_gray = f'{esc}[37m'
            esc_red = f'{esc}[91m'
            esc_green = f'{esc}[92m'
            esc_yellow = f'{esc}[93m'
            s = f'day {self.day}, part {self.part}: '
            v = str(self.value if self.value is not None else 'error')
            t = '({})'.format(pretty_time(self.time))
            spacing = ' ' * (width - len(s) - len(v) - max(len(t), 9))
            spacing += ' ' * (not spacing)
            v = (
                esc_red
                if self.value is None
                else (esc_green if self.correct else esc_yellow)
            ) + f'{v}{esc_normal}'
            c = {
                None: ' ',
                False: f'{esc_red}✘{esc_normal}',
                True: f'{esc_green}✔{esc_normal}',
            }[self.correct]
            t = f'{esc_gray}{t}{esc_normal}'
            return f'{s}{v}{spacing}{c} {t}'

    def pretty_time(t):
        if t <= 0:
            return 'no time at all'
        if t == float('inf'):
            return '∞'
        units = {
            'ns': 1e-9,
            'μs': 1e-6,
            'ms': 1e-3,
            's': 1,
        }
        for unit, ratio in units.items():
            factor = 59.95 if unit == 's' else 999.5
            if t < factor * ratio:
                num = f'{t/ratio:#.3g}'.rstrip('.')
                return f'{num} {unit}'
        return str(datetime.timedelta(seconds=int(round(t)))).removeprefix('0:')

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        filename_answer = 'answer.txt'
        directory = pathlib.Path(inspect.getfile(func)).parent
        day = int(directory.name)
        part = func.__name__.split('_')[-1]
        value = None
        try:
            tic = time.perf_counter()
            value = func(*args, **kwargs)
            toc = time.perf_counter()
        except Exception:
            toc = time.perf_counter()
            traceback.print_exc()
        correct = None
        if (path := directory / filename_answer).is_file():
            filename_input = get_input_filename()
            with open(path) as file:
                for line in file:
                    if not line.strip():
                        continue
                    filename, values_correct = map(str.strip, line.split(':'))
                    if filename != filename_input or not values_correct.strip():
                        continue
                    value_correct = (values_correct.split(',') * 2)[
                        ['one', 'two'].index(part)
                    ].strip()
                    correct = str(value) == value_correct
                    break
        return Solution(day, part, value, correct, toc - tic)

    return wrapper
