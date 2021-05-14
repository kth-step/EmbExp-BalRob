#!/usr/bin/env python3

import os
import json

results_dir = "experiment_results"

experiment_results = []
for filename in os.listdir(results_dir):
	filename = os.path.join(results_dir, filename)
	if not filename.endswith(".json"):
		continue
	print(f"loading {filename}")

	with open(filename) as f:
		data = json.load(f)
		assert(type(data) == list)
	experiment_results += data

print(f"found {len(experiment_results)} experiment results")

experiment_results_filtered = [x for x in experiment_results if x[1] != None]

maxval = max(list(map(lambda x: x[1], experiment_results_filtered)))

print(maxval)

