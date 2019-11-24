#!/bin/sh

echo "Hello $1"

git clone https://github.com/keith-packard/snek-ci snek
cd snek
make check
