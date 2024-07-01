#!/bin/bash

set -xe

g++ pong.cpp -lSDL2 -lSDL2_ttf -o pong
