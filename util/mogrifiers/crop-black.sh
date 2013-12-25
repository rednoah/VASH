#!/bin/bash
mogrify "$@" -thumbnail "640x400" -bordercolor black -border 80 -gravity center -crop 640x480+0+0