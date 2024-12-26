#!/bin/bash
ps aux | grep 'sphinx-autobuild' | grep '/usr/bin/python3' | awk '{print $2}' | xargs kill -9

make clean
make html
sphinx-autobuild --host 0.0.0.0 source build/html &
