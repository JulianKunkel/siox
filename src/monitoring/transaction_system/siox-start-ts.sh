#!/bin/bash

_MYDIR=$(readlink -f "$(dirname "$(which "$0")")")

_SIOXDIR=$(dirname "$_MYDIR")

exec "$_MYDIR/siox-daemon" --configEntry "$_SIOXDIR/etc/siox-ts.conf"

