#!/usr/bin/env python
import sys
import os
import subprocess
import time

readfile = open(sys.argv[1], "r")
lines = readfile.readlines()
i = 0

iozone = subprocess.Popen(['iozone','-a'], stdout=subprocess.PIPE)

os.system('rm -rf /root/.cache/Qt/demobrowser/*')
os.system('rm -rf /mnt/ramdisk/.cache/Qt/demobrowser/*')

for l in lines[:100]:
	writefile = open('./result/busy_inf/m_nth_html/nth_'+str(i),"w")
	i=i+1	
	webkit = subprocess.Popen(['./browser',l], stdout=writefile, stderr=subprocess.STDOUT)
	time.sleep(30)
	webkit.kill()
	if i%5 == 0 :
		iozone.kill()
		iozone = subprocess.Popen(['iozone','-a'], stdout=subprocess.PIPE)
	writefile.flush()
	writefile.close()
	os.system('sync')
	os.system('echo 3 > /proc/sys/vm/drop_caches')
	time.sleep(5)

iozone.kill()
