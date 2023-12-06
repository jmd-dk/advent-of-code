"""This module implements common, *magic* functionality.
As this module is meant to be star-imported, each function below
should import what it needs internally.
"""


# Function returning an open context manager
# for the reading of the input file.
def open_input_file():
    """The input file is determined as follows:
    Default: input.txt
    DEMO environment variable set to 1: demo-input.txt
    DEMO environment variable set to <n> > 1: demo<2>-input.txt
    """
    import contextlib, inspect, os, pathlib

    directory = pathlib.Path(inspect.stack()[1][1]).parent
    demo = os.environ.get('DEMO', '').lower()
    is_demo1 = demo in {'1', 'one', 't', 'true', 'y', 'yes'}
    is_demo2 = demo in {'2', 'two'}
    filename = ''
    filename += 'demo-' * is_demo1
    filename += 'demo2-' * is_demo2
    filename += f'demo{demo}-' * (not filename and demo.isdecimal())
    filename += 'input.txt'

    @contextlib.contextmanager
    def open_file(path):
        with open(path) as file:
            yield file

    return open_file(directory / filename)


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
        time: float

        def __str__(self):
            width = 36
            esc = '\x1b'
            esc_normal = f'{esc}[0m'
            esc_gray = f'{esc}[37m'
            s = f'day {self.day}, part {self.part}: {self.value}'
            t = '({})'.format(pretty_time(self.time))
            spacing = ' ' * (width - len(s) - max(len(t), 9))
            t = f'{esc_gray}{t}{esc_normal}'
            return f'{s}{spacing}{t}'

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
        value = 'error'
        try:
            tic = time.perf_counter()
            value = func(*args, **kwargs)
            toc = time.perf_counter()
        except Exception:
            toc = time.perf_counter()
            traceback.print_exc()
        day = int(pathlib.Path(inspect.getfile(func)).parent.name)
        part = func.__name__.split('_')[-1]
        return Solution(day, part, value, toc - tic)

    return wrapper
