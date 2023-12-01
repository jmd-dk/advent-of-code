def get_input_file():
    import os, pathlib
    demo = os.environ.get('DEMO', '').lower()
    is_demo1 = demo in {'1', 'one', 't', 'true', 'y', 'yes'}
    is_demo2 = demo in {'2', 'two'}
    filename = ''
    filename += 'demo-' * is_demo1
    filename += 'demo2-' * is_demo2
    filename += 'input.txt'
    return pathlib.Path(filename)

