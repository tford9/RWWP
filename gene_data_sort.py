import sys
import csv
import operator 
import numpy as np

reader = csv.reader(open("input.csv"))

next(reader, None)  # skip the headers

sortedList =np.array(sorted(reader, key=operator.itemgetter(1), reverse=True));

a = np.empty(shape = (len(sortedList),3), dtype="S15");
a.fill("");

sortedList = np.hstack((sortedList,a));
print(sortedList);
node_lookup = {};
node_edge_sum = {};

node_encounter = 0;


# Look down the From column in the graph data and give a number to each
# new gene name encountered. Then replace the name in the data with the 
# number associated.

for row in sortedList:
	#if it isn't in the dict, add it and assign the next index to it
	if row[1] not in node_lookup:
		node_lookup[row[1]] = node_encounter;
		node_encounter += 1;

	if row[2] not in node_lookup:
		node_lookup[row[2]] = node_encounter;
		node_encounter += 1;
	
	#replace the name with the number. 
	row[5]  = node_lookup[row[1]];
	row[6] = node_lookup[row[2]];

	# While we are looping through, sum up all the correlation 
	# coefficients for each out edge and put them in a dict for later
	if row[1] not in node_edge_sum:
		#print(row[3]);
		node_edge_sum[row[1]] = float(row[3]);
	else : node_edge_sum[row[1]] += float(row[3]);

	if row[2] not in node_edge_sum:
		#print(row[3]);
		node_edge_sum[row[2]] = float(row[3]);
	else : node_edge_sum[row[2]] += float(row[3]);

for row in sortedList:	

	# It's later, we are going to use those summed coefficients to normalize
	# the coefficients into something like probabilities.
	# always piggyback your for loops.
	row[4] = float(row[3])/ node_edge_sum[row[2]];
	row[3] = float(row[3])/ node_edge_sum[row[1]];

with open("pearson_size_match_sorted_indexed.csv", "wb") as file:
    	writer = csv.writer(file, delimiter=' ')
    	writer.writerows(sortedList)

