#!/bin/bash -eux

mkdir -p build
(cd build && cmake ..)
(cd build && make)
