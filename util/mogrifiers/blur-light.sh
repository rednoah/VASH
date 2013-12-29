#!/bin/bash
mogrify -thumbnail "640x480" -gaussian-blur 5x2 "$@"
