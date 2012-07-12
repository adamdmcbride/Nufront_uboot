#!/bin/sh

find . -name "*.h" -or -name "*.c" -or -name "*.cc" -or -name "*.s" -or -name "*.S" -or -name "*.cc" > cscope.files
cscope -bkq -i cscope.files
ctags -R *
