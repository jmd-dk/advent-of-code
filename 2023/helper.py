def get_input_file():
    import inspect, os, pathlib
    directory = pathlib.Path(inspect.stack()[1][1]).parent
    demo = os.environ.get('DEMO', '').lower()
    is_demo1 = demo in {'1', 'one', 't', 'true', 'y', 'yes'}
    is_demo2 = demo in {'2', 'two'}
    filename = ''
    filename += 'demo-' * is_demo1
    filename += 'demo2-' * is_demo2
    filename += 'input.txt'
    return directory / filename

