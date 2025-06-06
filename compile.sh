#!/bin/bash

# PCANNON COMPILE.SH v1.0

source ./utils/inc/sh/colors.sh

if ! command -v cmake > /dev/null 2>&1; then
	echo "$BRIGHT_RED Please have 'cmake' installed $RESET"
fi

if ! command -v jq > /dev/null 2>&1; then
	echo "$BRIGHT_RED Please have 'JQ' installed $RESET"
fi

COMPILATION_FILE_PATH=".private/dev/compilation.json"
PROJECT_INFO_PATH=".private/project.json"

cores=$(jq '.cores' $COMPILATION_FILE_PATH)
enableBackup=$(jq -r '.enableBackup' $COMPILATION_FILE_PATH)
projectName=$(jq -r '.exeName' $PROJECT_INFO_PATH)

if [ ! -d "./build" ]; then
	mkdir build
fi

function ninjaComp(){
    if [ "$1" != "m" ]; then
		cmake -S . -B build -G Ninja
	fi

	cmake --build build -j$cores -v
}

function makeComp(){
	cd build

	if [ "$1" != "m" ]; then
		cmake .. -DCMAKE_BUILD_TYPE=Release
	fi

	make
}

if [ "$enableBackup" == "YES" ]; then
    printf "${BOLD}${GREEN}[ INFO ] Backing up executable\n${RESET}"

    exeBakPath="./build/bin/exeBackup" 

    if [ ! -d "$exeBakPath" ]; then
        mkdir -p "$exeBakPath"
    fi
    
    cp "./build/bin/$projectName" "$exeBakPath/$projectName-$(date)"

    echo " [ Done ]"
fi

if [ "$1" == "" ] || [ "$1" == "m" ]; then
	if command -v ninja > /dev/null 2>&1; then
		ninjaComp $@

		exit

	elif command -v make > /dev/null 2>&1; then
		makeComp $@

		exit

	else
		echo "$BRIGHT_RED Please have 'ninja' or 'make' $RESET"
	fi

elif [ "$1" == "--settings" ]; then
    echo "[ * ] Compilation settings"

    echo -e "${BOLD}[ PROMPT ] Enter the number of cores to compile the program ${RESET}"

    read cores
    
    if [ ! -d "tmp" ]; then
        echo -e "${BOLD}[ NOTE ] Creating \`tmp/\` dir"

        mkdir tmp
    fi

    if [[ ! -s "$COMPILATION_FILE_PATH" ]]; then
        echo "{}" > "$COMPILATION_FILE_PATH" 
    fi

    jq ".cores = $cores" "$COMPILATION_FILE_PATH" > tmp/tmp_dev_compilation.json

    echo -e "${BOLD}[ PROMPT ] Enable backup? 'YES' or 'NO' ${RESET}"

    read enableBackup

	# Set JQ 
	jq --arg val "$enableBackup" ".cores = $cores | .enableBackup = \$val" "$COMPILATION_FILE_PATH" > tmp/tmp_dev_compilation.json

    mv tmp/tmp_dev_compilation.json "$COMPILATION_FILE_PATH"
    
    echo "[ DONE ]"

    exit

elif [ "$1" == "--ninja" ]; then
	ninjaComp

elif [ "$1" == "--make" ]; then
	makeComp
fi
