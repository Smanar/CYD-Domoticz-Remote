"""

This script changes WebSockets buffer size for ESP8266 and ESP32
    from 15K to 25K in order to allow receiving large buffers from Domoticz

It's run by (pre) extra_script command in platformio.ini

"""

import os
import pathlib
import sys
import inspect
Import("env")

def trace_log(message):
    """
        Write a message on screen
    """
    print(F"{thisScriptFileName}: {message}")

# Dump global construction environment (for debug purpose)
# print(env.Dump())

# Extract full/path/name from this script file
thisScriptFullFileName = inspect.getouterframes(inspect.currentframe())[0].filename
thisScriptPath = pathlib.Path(thisScriptFullFileName).parent.resolve()
thisScriptFileName = pathlib.Path(thisScriptFullFileName).name

# Name of LIB_DEPS folder
PROJECT_LIBDEPS_DIR = env['PROJECT_LIBDEPS_DIR']

# Name of .PIO folder
PIOENV = env['PIOENV']

# Elements specific to this script (path/file name, line to change)
LIBRARY_NAME = 'WebSockets'
LIBRARY_FOLDER = 'src'
FILE_TO_CHANGE = 'WebSockets.h'
CHANGE_FROM = "#if defined(ESP8266) || defined(ESP32)\r\n" \
            +  "\r\n" \
            +  "#define WEBSOCKETS_MAX_DATA_SIZE (15 * 1024)\r\n"
CHANGE_TO   = "#if defined(ESP8266) || defined(ESP32)\r\n" \
            +  "\r\n" \
            +  "#define WEBSOCKETS_MAX_DATA_SIZE (25 * 1024)\r\n"

# Compose the full file name
fullFileName = os.path.join(PROJECT_LIBDEPS_DIR, PIOENV, \
    LIBRARY_NAME, LIBRARY_FOLDER, FILE_TO_CHANGE)

# Make path relative to reduce length
relativeFileName = os.path.relpath(fullFileName)

# Check for file existence
if os.path.isfile(relativeFileName):
    # Read full file content in binary and convert it to text
    with open(relativeFileName, 'rb') as inputStream:
        binaryContent = inputStream.read()
        textContent = binaryContent.decode(encoding='utf-8')
    # Check for original sequence
    if textContent.find(CHANGE_FROM) >= 0:
        # Replace sequence, convert it to bianry and overwrite file
        textContent = textContent.replace(CHANGE_FROM, CHANGE_TO)
        binaryContent = textContent.encode('utf-8')
        with open(relativeFileName, 'wb') as outputStream:
            outputStream.write(binaryContent)
        trace_log(F'File {relativeFileName} changed')
    # Check for target sequence
    elif textContent.find(CHANGE_TO) >= 0:
        trace_log(F'File {relativeFileName} already changed')
    # Don't find both sequences
    else:
        trace_log(F'*** Can\'t find change sequences in {relativeFileName} ***')
        sys.exit(1)
else:
    trace_log(F'*** File {relativeFileName} *NOT* found ***')
    sys.exit(1)
