#!/bin/bash

#clean things up
echo "Cleaning the temporaries and outputs"
echo "========================================"
make clean
#echo ""
rm -rf test_deliverable/working

echo "Building from scratch"
echo "========================================"
make bin/c_compiler
if [[ "$?" -ne 0 ]]; then
	echo "Build failed.";
	exit 1
fi

#begin testbench
if [[ -z "$1" ]]; then 
    COMPILER=bin/c_compiler
else
    COMPILER=$1
fi

mkdir -p test_deliverable/working

PASSED=0
CHECKED=0

for DRIVER in test_deliverable/test_cases/*_driver.c ; do
    echo "****************************"
    NAME=$(basename $DRIVER _driver.c)
    CHECKED=$(( ${CHECKED}+1 ));
    TEST_ID=test_deliverable/test_cases/$NAME.c
    echo "Test case $NAME"

    # Step 1: Compile ${NAME}.c using the compiler-under-test into MIPS assembly.
    $COMPILER -S test_deliverable/test_cases/$NAME.c -o test_deliverable/working/$NAME.s
    if [[ $? -ne 0 ]]; then
        >&2 echo "ERROR : Compiler-under-test generating wrong code"
        continue
    fi
    
    # Step 2: Compile ${NAME}_driver.c using GCC into a MIPS object file.
    mips-linux-gnu-gcc -mfp32 -c $DRIVER -o test_deliverable/working/${NAME}_driver.o
    if [[ $? -ne 0 ]]; then
        >&2 echo "ERROR : Test-case is malformed, GCC couldn't compile program"
        continue
    fi
    
    
    # Step 3: Link driver object and assembly into executable
    mips-linux-gnu-gcc -mfp32 -static test_deliverable/working/$NAME.s test_deliverable/working/${NAME}_driver.o -o test_deliverable/working/${NAME}_output
    if [[ $? -ne 0 ]]; then
        >&2 echo "ERROR : Linker and assembly not working"
        continue
    fi
    
    # Step 4: Run the actual executable
    qemu-mips test_deliverable/working/${NAME}_output
    GOT_RESULT=$?
    if [[ $result -ne 0 ]]; then
        >&2 echo "ERROR : Testcase returned $GOT_RESULT, but expected 0."
    else
        echo -e "$NAME [PASSED]"
        PASSED=$(( ${PASSED}+1 ));
    fi
done

echo ""
echo ""
echo "########################################"
echo "       # Passed ${PASSED} out of ${CHECKED} #"
echo "########################################"
echo ""
echo ""

# RELEASE=$(lsb_release -d)
# if [[ $? -ne 0 ]]; then
#     echo ""
#     echo "Warning: This appears not to be a Linux environment"
#     echo "         Make sure you do a final run on a lab machine or an Ubuntu VM"
# else
#     grep -q "Ubuntu 16.04" <(echo $RELEASE)
#     FOUND=$?

#     if [[ $FOUND -ne 0 ]]; then
#         echo ""
#         echo "Warning: This appears not to be the target environment"
#         echo "         Make sure you do a final run on a lab machine or an Ubuntu VM"
#     fi
# fi