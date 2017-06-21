#!/usr/bin/env python
import csv
import sys

count = 1
switch = 0
one = 0
data = []

with open(sys.argv[1], 'rU') as csv_file:
	reader = csv.reader(csv_file, delimiter=',', dialect=csv.excel_tab)
	for row in reader:
                if len(row[1]) != 0 and row[0] != 'Started':
                        switch = 1
		elif switch == 1 and row[0] != 'Started':
			if (row[7] == 'html' or row[7].startswith('Redirect')):
                                data.append(row[8])
                                switch = 0
		count += 1

writefile = open('url_'+sys.argv[1].split('.')[0]+'.txt', "w")
for l in data:
	writefile.write(l+'\n')
writefile.flush()
writefile.close()
