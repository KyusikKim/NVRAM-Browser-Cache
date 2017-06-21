#! /usr/bin/ python
import os

str_header = "#include <stdio.h>"
str_printf = 'printf("[sslab]%s:%d:%s\\n",__FILE__,__LINE__,__func__);'

start_directory = '.'

if __name__ == '__main__':
	print "Process will be start when you press the enter key."
	print str_header
	print str_printf
	raw_input()

	for(path, dir, files) in os.walk(start_directory):
#		dir[:] = [dirs for dirs in dir if dirs != '.moc']	
#		dir[:] = [dirs for dirs in dir if dirs != 'moc']	
		for file in files:
			fileName = os.path.join(path, file)
			if fileName[-4:].lower()  == '.cpp' or fileName[-2:].lower() =='.c':
				cppFile = open(fileName, 'r')
				cppSrc = cppFile.read()
				cppFile.close()
				cppFile = open(fileName, 'w')
				cppSrc = cppSrc.replace(str_printf, '')
				cppSrc = cppSrc.replace(str_header, '')
				cppFile.write(cppSrc)
				cppFile.close()
				print "["+fileName+"] is modified."

	print "Process is terminated successfully."
