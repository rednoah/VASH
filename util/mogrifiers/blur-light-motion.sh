#!/bin/bash
mogrify -gravity center -motion-blur 10x10 "$@"
