#!/bin/bash

code="$PWD"
opts=-g
cd ../build > /dev/null
g++ $opts $code/win32 -o ../buil
cd $code > /dev/null
