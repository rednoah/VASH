#!/bin/bash
mogrify "$@" -thumbnail "640x480" -gaussian-blur 0x8
