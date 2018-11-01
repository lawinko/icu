# Copyright (C) 2018 and later: Unicode, Inc. and others.
# License & terms of use: http://www.unicode.org/copyright.html

from . import *
from .. import *
from .. import utils

import os
import subprocess

def run(build_dirs, requests, common_vars, **kwargs):
    for bd in build_dirs:
        os.makedirs(bd.format(**common_vars), exist_ok=True)
    for request in requests:
        status = run_helper(request, common_vars, **kwargs)
        if status != 0:
            print("!!! ERROR executing above command line: exit code %d" % status)
            return 1
    print("All done!")
    return 0

def run_helper(request, common_vars, bin_dir, **kwargs):
    if isinstance(request, PrintFileRequest):
        output_path = "{DIRNAME}/{FILENAME}".format(
            DIRNAME = utils.dir_for(request.output_file).format(**common_vars),
            FILENAME = request.output_file.filename,
        )
        print("Printing to file: %s" % output_path)
        with open(output_path, "w") as f:
            f.write(request.content)
        return 0

    assert isinstance(request.tool, IcuTool)
    cmd_template = "{BIN_DIR}/{TOOL}.exe {{ARGS}}".format(
        BIN_DIR = bin_dir,
        TOOL = request.tool.name,
        **common_vars
    )
    if isinstance(request, RepeatedExecutionRequest):
        for iter_vars, input_file, output_file in utils.repeated_execution_request_looper(request):
            command_line = utils.format_repeated_request_command(request, cmd_template, iter_vars, input_file, output_file, common_vars)
            # TODO: Is this / to \ substitution too aggressive?
            command_line = command_line.replace("/", "\\")
            print("Running: %s" % command_line)
            res = subprocess.run(command_line)
            if res.returncode != 0:
                return res.returncode
        return 0
    if isinstance(request, SingleExecutionRequest):
        command_line = utils.format_single_request_command(request, cmd_template, common_vars)
        # TODO: Is this / to \ substitution too aggressive?
        command_line = command_line.replace("/", "\\")
        print("Running: %s" % command_line)
        res = subprocess.run(command_line)
        return res.returncode
    assert False