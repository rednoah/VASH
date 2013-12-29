#!/bin/bash
mogrify -thumbnail "640x480" -box transparent -pointsize 26 -gravity center -fill white -stroke orange -draw "text 0,150 'These are subtitles for this particular frame!'" "$@"
