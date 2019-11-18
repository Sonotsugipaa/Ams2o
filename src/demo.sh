#!/bin/bash

SCRIPT=$1
if [ -z "$1" ]; then
	SCRIPT="demo_script.ams"
fi
chmod 755 ./ams2o

echo
echo " --------------------------------------------"
echo " ------/                              \------"
echo " -----/        AMSCRIPT2 SOURCE        \-----"
echo $'\033[1;94m'
cat "$SCRIPT"
echo $'\033[m'
echo " -----\        AMSCRIPT2 SOURCE        /-----"
echo " ------\                              /------"
echo " --------------------------------------------"
echo

echo
echo " --------------------------------------------"
echo " ------/                              \------"
echo " -----/        AMSCRIPT2 OUTPUT        \-----"
echo $'\033[1;92m'
./ams2o "$SCRIPT"
echo $'\033[m'
echo " -----\        AMSCRIPT2 OUTPUT        /-----"
echo " ------\                              /------"
echo " --------------------------------------------"
echo
