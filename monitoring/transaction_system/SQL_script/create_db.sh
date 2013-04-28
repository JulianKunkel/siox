#!/bin/bash

# Use: ./create_sql.sh -h 136.172.14.14 -p 48142 -U postgres

# Portability with non gnu-systems:
# PGOPTIONS='--client-min-messages=warning' 

psql -X -q -a -1 -v ON_ERROR_STOP=1 --set AUTOCOMMIT=off --pset pager=off $* < siox-trs.sql



