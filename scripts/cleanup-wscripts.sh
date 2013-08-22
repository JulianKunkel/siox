#!/bin/bash
./scripts/replace-string.sh "\([^a-zA-Z_\\-]\)\($1\)\([^.a-zA-Z]\)" wscript "\1$2\3" $3
