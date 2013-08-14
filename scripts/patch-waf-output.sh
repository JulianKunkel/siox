#!/bin/bash
echo "Must be run at the source root directory"
echo "Echo run waf before!"
patch  -p0  < scripts/waf_unit_test_output.patch
