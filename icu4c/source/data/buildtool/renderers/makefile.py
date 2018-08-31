# Copyright (C) 2016 and later: Unicode, Inc. and others.
# License & terms of use: http://www.unicode.org/copyright.html

from .. import *
from .. import utils

def get_gnumake_rules(requests, common_vars, **kwargs):
    makefile_string = """## Makefile for ICU data
## Copyright (C) 2018 and later: Unicode, Inc. and others.

## Source directory information
srcdir = {SRCDIR}
top_srcdir = {TOP_SRCDIR}

# So that you have $(top_builddir)/config.status
top_builddir = {TOP_BUILDDIR}

## All the flags and other definitions are included here.
include $(top_builddir)/icudefs.mk

""".format(
        # TODO
        SRCDIR = ".",
        TOP_SRCDIR = "..",
        TOP_BUILDDIR = ".."
    )

    # Common Variables
    kwargs["common_vars_mak"] = { k: "$(%s)" % k for k in common_vars.keys() }
    for key, value in common_vars.items():
        makefile_string += "{KEY} = {VALUE}\n".format(
            KEY = key,
            VALUE = value
        )
    makefile_string += "\n"

    # Generate Rules
    make_rules = []
    for request in requests:
        make_rules += get_gnumake_rules_helper(request, **kwargs)

    # All output files, for "all" command
    all_output_files = set(file for rule in make_rules for file in rule.output_files if isinstance(file, OutFile))
    makefile_string += "ALL_OUT = %s\n\n" % files_to_makefile(sorted(all_output_files), **kwargs)

    # Variables for print commands
    for request in requests:
        if isinstance(request, PrintFileRequest):
            makefile_string += "define {NAME}_CONTENT\n{CONTENT}\nendef\nexport {NAME}_CONTENT\n\n".format(
                NAME = request.name.upper(),
                CONTENT = request.content
            )

    # Main Commands
    for rule in make_rules:
        if rule.name == "dirs":
            header_line = "dirs:"
        elif len(rule.output_files) == 1:
            header_line = "{OUT_LIST}: {IN_LIST} | dirs".format(
                OUT_LIST = files_to_makefile(rule.output_files, **kwargs),
                IN_LIST = files_to_makefile(rule.input_files, **kwargs)
            )
        else:
            header_line = "{name}_all: {IN_LIST} | dirs".format(
                IN_LIST = files_to_makefile(rule.input_files, **kwargs),
                name = rule.name
            )
            for file in rule.output_files:
                makefile_string += "{MAKEFILENAME}: {name}_all\n".format(
                    MAKEFILENAME = files_to_makefile([file], **kwargs),
                    name = rule.name
                )
            makefile_string += "\n"
        makefile_string += "{HEADER_LINE}\n{RULE_LINES}\n\n".format(
            HEADER_LINE = header_line,
            RULE_LINES = "\n".join("\t%s" % cmd for cmd in rule.cmds)
        )

    # Main Target
    makefile_string += "all: $(ALL_OUT)\n\n"

    return makefile_string

def files_to_makefile(files, is_nmake, common_vars_mak, **kwargs):
    if len(files) == 0:
        return ""
    dirnames = [utils.dir_for(file).format(**common_vars_mak) for file in files]
    if len(files) == 1:
        return "%s/%s" % (dirnames[0], files[0].filename)
    if is_nmake or len(set(dirnames)) > 1:
        return " ".join("%s/%s" % (dirname, file.filename) for dirname, file in zip(dirnames, files))
    else:
        return "$(addprefix %s/,%s)" % (dirnames[0], " ".join(file.filename for file in files))

def get_gnumake_rules_helper(request, is_nmake, common_vars_mak, **kwargs):
    if isinstance(request, PrintFileRequest):
        if is_nmake or True:
            return [
                MakeRule(
                    name = request.name,
                    input_files = [],
                    output_files = [request.output_file],
                    cmds = [
                        "echo \"$${NAME}_CONTENT\" > {MAKEFILENAME}".format(**common_vars_mak,
                            NAME = request.name.upper(),
                            MAKEFILENAME = files_to_makefile([request.output_file], is_nmake, common_vars_mak, **kwargs)
                        )
                    ]
                )
            ]
        else:
            return [
                MakeRule(
                    name = request.name,
                    input_files = [],
                    output_files = [request.output_file],
                    cmds = [
                        "$(file >{MAKEFILENAME},{CONTENT})".format(**common_vars_mak,
                            MAKEFILENAME = files_to_makefile([request.output_file], is_nmake, common_vars_mak, **kwargs),
                            CONTENT = "\\ \n".join(request.content.split("\n"))
                        )
                    ]
                )
            ]

    if request.tool == "mkinstalldirs":
        template = "sh ../mkinstalldirs {ARGS}"
    else:
        template = "$(INVOKE) $(TOOLBINDIR)/{TOOL} {{ARGS}}".format(TOOL = request.tool)

    if isinstance(request, SingleExecutionRequest):
        cmd = template.format(ARGS = request.args.format(**common_vars_mak, **request.format_with,
            INPUT_FILES = [file.filename for file in request.input_files],
            OUTPUT_FILES = [file.filename for file in request.output_files],
        ))
        return [
            MakeRule(
                name = request.name,
                input_files = request.input_files,
                output_files = request.output_files,
                cmds = [cmd]
            )
        ]

    if isinstance(request, RepeatedExecutionRequest):
        rules = []
        for iter_vars, input_file, output_file in repeated_execution_request_looper(request):
            name_suffix = input_file[input_file.filename.rfind("/")+1:input_file.filename.rfind(".")]
            rules.append(MakeRule(
                name = "%s_%s" % (request.name, name_suffix),
                input_files = [input_file] + request.dep_files,
                output_files = [output_file],
                cmds = [template.format(ARGS = request.args.format(**common_vars_mak, **request.format_with, **iter_vars,
                    INPUT_FILE = input_file.filename,
                    OUTPUT_FILE = output_file.filename
                ))]
            ))
        return rules

    return []
