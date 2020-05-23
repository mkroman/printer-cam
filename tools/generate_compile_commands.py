#!/usr/bin/env python
# This scripts generates and makes changes to `compile_commands.json` in order
# to make it work with coc-clangd without throwing a fit

import os
import json
import shlex
import subprocess

project_root = os.path.join(os.path.dirname(__file__), '..')
build_root = os.path.join(project_root, 'build')

# Generate the compile_commands.json using CMake with the Ninja generator
initial_dir = os.getcwd()
os.chdir(build_root)
subprocess.run(["cmake", "-G", "Ninja", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", ".."])
os.chdir(initial_dir)

# Load the generated commands
file_path = os.path.join(build_root, 'compile_commands.json')
with open(file_path, 'r') as file:
    lines = json.load(file)

# Enumerate through each command and apply fixes
for line in lines:
    command = shlex.split(line['command'])
    basename = os.path.basename(command[0])

    if basename in ['xtensa-esp32-elf-gcc', 'xtensa-esp32-elf-g++']:
        # Inject the NOOP definition for __GLIBC_USE(...) since clangd seems to
        # to read the later included limits.h from the system headers which
        # expects it to be available
        command.insert(1, '-D__GLIBC_USE(...)=0')

        # Remove `-fstrict-volatile-bitfields` as it's not supported by clangd
        try:
            command.remove('-fstrict-volatile-bitfields')
        except ValueError as e:
            pass

        # Replace `-mlongcalls` with `-mlong-calls` as the naming is different
        # for clangd
        # The definition can be present multiple times so we'll use a do while
        # loop
        while True:
            try:
                idx = command.index('-mlongcalls')
                command[idx] = '-mlong-calls'
            except ValueError as e:
                break

    line['command'] = shlex.join(command)

# Save the fixed `compile_commands.json` in the project root
with open(os.path.join(project_root, 'compile_commands.json'), 'w') as f:
    json.dump(lines, f, indent=2)
