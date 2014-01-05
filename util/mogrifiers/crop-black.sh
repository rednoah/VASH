#!/bin/bash
mogrify -bordercolor black -border 80 -gravity center -crop 640x480+0+0 "$@"
