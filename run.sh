#!/bin/bash

# PCANNON RUN.SH v1.0

PROJECT_INFO_PATH=".private/project.json"

projectName=$(jq -r '.exeName' $PROJECT_INFO_PATH)

if [[ "$1" == "GDB" ]]; then
	gdb ./build/bin/$projectName

else
	./build/bin/$projectName "$@"

fi

