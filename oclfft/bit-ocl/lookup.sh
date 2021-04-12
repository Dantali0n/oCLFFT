#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $DIR/src
objcopy --input binary --output elf64-x86-64 --binary-architecture i386 lookup.cl lookup.o