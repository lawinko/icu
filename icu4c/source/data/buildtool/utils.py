# Copyright (C) 2016 and later: Unicode, Inc. and others.
# License & terms of use: http://www.unicode.org/copyright.html

from . import *

def dir_for(file):
    if isinstance(file, InFile):
        return "{IN_DIR}"
    if isinstance(file, TmpFile):
        return "{TMP_DIR}"
    if isinstance(file, OutFile):
        return "{OUT_DIR}"
    assert False

def repeated_execution_request_looper(request):
    # dictionary of lists to list of dictionaries:
    # https://stackoverflow.com/a/33046935/1407170
    ld = [dict(zip(request.repeat_with, t)) for t in zip(*request.repeat_with.values())]
    if not ld:
        # No special options given in repeat_with
        ld = [{} for _ in range(len(request.input_files))]
    return zip(ld, request.input_files, request.output_files)
