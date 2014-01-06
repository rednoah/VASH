#!/usr/bin/python
from subprocess import call

with open('workset.txt') as fo:
	for line in fo:
		call(["./vash", "test", line.rstrip('\n'), "workset/centroids_k1000.db", "workset/dataset_k1000.db"])

fo.close()
