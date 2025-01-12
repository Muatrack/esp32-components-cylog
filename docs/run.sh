#!/bin/bash
ps aux | grep 'sphinx-autobuild' | grep -v 'color=auto' | awk '{print $2}' | xargs kill -9

if [ -d .env ]; then
   echo "Dir .env exists"
else
   echo "Dir .env does not exist"
   python3 -m venv .env
   source .env/bin/activate
   pip install sphinx-autobuild sphinx_rtd_theme
fi

make clean
make html
sphinx-autobuild --host 0.0.0.0 --port 8080 source build/html &
