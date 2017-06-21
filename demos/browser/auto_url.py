#!/usr/bin/env python
import sys
import os
import subprocess
import time

flag = 'nocache'
readfile = open(sys.argv[1], "r")
lines = readfile.readlines()
i = 0

#iozone = subprocess.Popen(['iozone','-a'], stdout=subprocess.PIPE)

for l in lines:
	os.system('rm -rf /root/.cache/Qt/demobrowser/*')
	os.system('rm -rf /mnt/ramdisk/.cache/Qt/demobrowser/*')
#	writefile = open('./wget_result/'+str(i),"w")
	i=i+1
	writefile = open('./wget_result/'+l.split('/')[1]+'_'+flag,"w")
	webkit = subprocess.Popen(['./browser',l], stdout=writefile, stderr=subprocess.STDOUT)
	time.sleep(30)
	webkit.kill()
#	if i%5 == 0 :
#		iozone.kill()
#		iozone = subprocess.Popen(['iozone','-a'], stdout=subprocess.PIPE)
	writefile.flush()
	writefile.close()
	os.system('sync')
	os.system('echo 3 > /proc/sys/vm/drop_caches')
	time.sleep(5)

#iozone.kill()
