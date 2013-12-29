#!/bin/bash
mogrify -thumbnail "640x480" -gravity center -motion-blur 10x10 "$@"
