"""This module implements common, *magic* functionality.
As this module is meant to be star-imported, each function below
should import what it needs internally.
"""


# Function returning an open context manager
# for the reading of the input file.
def open_input():
    import contextlib, inspect, io, pathlib

    @contextlib.contextmanager
    def open_file(path):
        with open(path) as file:
            yield file

    @contextlib.contextmanager
    def open_text(text):
        with io.StringIO(text) as file:
            yield file

    directory = pathlib.Path(inspect.stack()[1][1]).parent
    filename, demo = _get_input_info()
    if demo:
        return open_text(_get_demo(directory))
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
        correct: bool
        time: float

        def __str__(self):
            width = 44
            s = f'day {self.day}, part {self.part}: '
            v = str(self.value if self.value is not None else 'error')
            t = '({})'.format(pretty_time(self.time))
            spacing = ' ' * (width - len(s) - len(v) - max(len(t), 9))
            spacing += ' ' * (not spacing)
            v = escape(
                (
                    'red'
                    if self.value is None
                    else ('green' if self.correct else 'yellow')
                ),
                v,
            )
            c = {
                None: ' ',
                False: escape('red', '✘'),
                True: escape('green', '✔'),
            }[self.correct]
            t = escape('gray', t)
            return f'{s}{v}{spacing}{c} {t}'

    class DontPrint:
        def __str__(self):
            return escape('up')

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        directory = pathlib.Path(inspect.getfile(func)).parent
        day = int(directory.name)
        part = func.__name__.split('_')[-1]
        value_correct = _get_answers(directory)[['one', 'two'].index(part)]
        if value_correct is None:
            return DontPrint()
        value = None
        try:
            tic = time.perf_counter()
            value = func(*args, **kwargs)
            toc = time.perf_counter()
        except Exception:
            toc = time.perf_counter()
            traceback.print_exc()
        correct = None
        if value_correct != '?':
            correct = str(value) == value_correct
        return Solution(day, part, value, correct, toc - tic)

    def pretty_time(t):
        if t <= 0:
            return 'no time at all'
        if t == float('inf'):
            return '∞'
        units = {
            'ps': 1e-12,
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

    def escape(kind, s=None):
        escapes = {
            'normal': '0m',
            'up': '1A',
            'gray': '37m',
            'red': '91m',
            'green': '92m',
            'yellow': '93m',
        }
        code = f'\x1b[{escapes[kind]}'
        if s is None:
            return code
        code_normal = escape('normal')
        return f'{code}{s}{code_normal}'

    return wrapper


# Function returning the input data file as well as the demo number
# in accordance with to the DEMO environment variable.
def _get_input_info(*, info=[]):
    import os, warnings

    if info:
        return info
    demo = os.environ.get('DEMO', '').strip().lower().removeprefix('demo')
    if demo.isdecimal():
        demo = int(demo)
    elif demo:
        warnings.warn(f'Did not understand DEMO={demo!r}')
        demo = None
    filename = 'input'
    if demo:
        filename = f'demo-{filename}'
    if not os.path.isfile(filename) and os.path.isfile(f'{filename}.txt'):
        filename = f'{filename}.txt'
    info += filename, demo
    return _get_input_info()


# Function for reading in demos
def _get_demo(directory, *, demos={}):
    import re

    filename, demo = _get_input_info()
    texts = demos.get(directory)
    if texts is not None:
        return texts[demo]
    with open(directory / filename) as file:
        texts = {}
        text = None
        for line in file:
            if match := re.search(r'# *demo *(\d+)', line.lower()):
                texts[int(match[1])] = text = []
                continue
            if text is None:
                continue
            text.append(line)
    for demo, text in texts.items():
        while not text[-1].strip():
            text.pop()
        texts[demo] = ''.join(text)
    demos[directory] = texts
    return _get_demo(directory)


# Function for reading in correct answers
def _get_answers(directory, *, answers={}):
    import re

    values = answers.get(directory)
    if values is not None:
        return values
    filename = 'answer'
    values = ('?', '?')
    if not (path := directory / filename).is_file():
        answers[directory] = values
        return _get_answers(directory)
    _, demo = _get_input_info()
    with open(path) as file:
        for line in file:
            if not line.strip():
                continue
            kind, values_line = line.split(':')
            if demo and not re.match(rf'demo *{demo}\s*$', kind.lower()):
                continue
            if not demo and 'demo' in kind.lower():
                continue
            values = (tuple(map(str.strip, values_line.split(','))) * 2)[:2]
            values = tuple(
                {'none': None, '': '?'}.get(value.lower(), value) for value in values
            )
            break
    answers[directory] = values
    return _get_answers(directory)
