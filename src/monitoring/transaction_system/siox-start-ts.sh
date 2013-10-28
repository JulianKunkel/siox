#!/bin/bash

#
# This is ugly and should be rewritten.
#

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/3rdparty/boost/1.53.0/lib/

siox-daemon --configEntry /usr/local/etc/siox-ts.conf

