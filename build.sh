#!/bin/bash -xe

cc perfinfo.c main.c -lraylib -o perfmon -ggdb
