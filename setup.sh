#!/bin/sh

if [[ "`uname`" == "Linux" ]]; then
    command -v platformio -h >/dev/null 2>&1 || { echo "no platformio installation found, please provide password for sudo or press CTRL-C and execute \"sudo pip install -U platformio\" manually."; echo; sudo pip install -U platformio; }
fi

if [[ "`uname`" == "Darwin" ]]; then
    command -v platformio -h >/dev/null 2>&1 || { brew install platformio; }
fi

platformio init --ide vscode
platformio init --ide eclipse

echo; echo "setup done, run \"platformio run\" to compile or start your IDE."; echo

