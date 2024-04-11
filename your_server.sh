#!/bin/sh
#
# DON'T EDIT THIS!
set -e
cmake . >/dev/null
make >/dev/null
exec ./server "$@"
