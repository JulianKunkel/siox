#!/bin/bash

export LD_LIBRARY_PATH=/usr/local/siox/lib/
export PATH=/usr/local/siox/bin:$PATH

siox-daemon --configEntry=daemon.conf

