#!/bin/bash
mogrify -thumbnail "640x480" -radial-blur 5 "$@"
