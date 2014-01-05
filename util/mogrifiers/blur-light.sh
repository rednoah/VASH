#!/bin/bash
mogrify -gaussian-blur 5x2 "$@"
