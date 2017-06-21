#! /usr/bin/ python
import re
import os

common__identifier = "[a-zA-Z_][a-zA-Z0-9_]*"
common__classFuncPrefix = common__identifier + "::"
common__space = "[' '\t]*"
common__prefixes = "(?:" + common__identifier + "(?:[\t' ']*<.+?>)?[' '\t]+)*?"
common__funcType = "(?:" + "(?:" + common__classFuncPrefix + ")*?" + common__identifier + "(?:[\t' ']*<.+?>)?)"
common__refPointer = "[\*&\[\]]*"
common__funcName = "(?:" + common__classFuncPrefix + ")*?" + common__identifier + "[\+\-\*/=><]*"
#common__arguments = "\([a-zA-Z0-9_,\s\*&<>:\[\]\.]*\)"
common__arguments = "\([a-zA-Z0-9_,\(\)\*&\s\.:-><]*?\)" #
common__suffixes = "(?:" + common__identifier + ")?"
common__comments = "(?:(?://[^\n]+)|(?:/\*.+?\*/))*"

common__merged = "(?:\n" + common__prefixes + common__funcType + common__space + common__refPointer \
 + common__space + common__funcName + common__space + common__arguments + common__space + common__suffixes \
 + common__space + common__comments + "\s*?\n"  + "{)"
#common_)merged = "(?:\n" + common__funcType + "+" + common__refPointer + common__funcName \
#+ common__arguments + common__suffixes + "\s*" + common__comments + "{)"

class__className = common__identifier
class__funcName = "~?" + common__identifier
class__arguments = common__arguments
class__suffixes = "(?::[a-zA-Z0-9_,\(\)\*&\s\.:-><]+?)?"
class__comments = common__comments

class__merged = "(?:\n" + "(?:" + class__className + "\s*" + "::" + ")*?" + "\s*" + class__funcName \
+ common__space + class__arguments + "\s*" + class__suffixes + "\s*" + class__comments + "\s*?\n" + "{)" 

all__merged = "(" + common__merged + "|" + class__merged + ")"

#all__regularExpression = re.compile(all__merged, re.DOTALL)
all__regularExpression = re.compile(all__merged)

command__printStatus = r"""
#ifdef SSLAB__DEBUG
	qWarning("[sslab][%s][tid %lu] %s:%d:%s",QDateTime::currentDateTime().toString("dd.MM.yyy hh:mm:ss.zzz").toAscii().data(), QThread::currentThreadId(), __FILE__,__LINE__,__func__);
#endif
"""

include__getTimeInfo = r"""
#ifdef SSLAB__DEBUG
#ifndef SSLAB__QDATETIME
#define SSLAB__QDATETIME
#include <QDateTime>
#endif
#endif
"""

include__getPidInfo = r"""
#ifdef SSLAB__DEBUG
#ifndef SSLAB__QTHREAD
#define SSLAB__QTHREAD
#include <QThread>
#endif
#endif
"""

start_directory = "."

if __name__ == '__main__':
	print "Process will be start when you press the enter key."
	raw_input()

	for(path, dir, files) in os.walk(start_directory):
		dir[:] = [dirs for dirs in dir if dirs != '.moc']	
		dir[:] = [dirs for dirs in dir if dirs != 'moc']	
		for file in files:
			#if file.lower() == "qstring.cpp" or file.lower() == "qdatatime.cpp" or file.lower() == "qbytearray.cpp":
			if file.lower() == "qdatatime.cpp":
				continue
			
			fileName = os.path.join(path, file)
			if(fileName[-4:].lower()  == '.cpp'):
				cppFile = open(fileName, 'r+')
				cppSrc = cppFile.read()
				cppSrc = include__getTimeInfo + include__getPidInfo + cppSrc
				cppSrc = all__regularExpression.sub(r'\1'+command__printStatus, cppSrc)
				cppFile.seek(0, 0)
				cppFile.write(cppSrc)
				cppFile.close()
				print "["+fileName+"] is modified."

			elif(fileName[-2:].lower()  == '.c'):
				cppFile = open(fileName, 'r+')
				cppSrc = cppFile.read()
				cppSrc = include__getTimeInfo + include__getPidInfo + cppSrc
				cppSrc = all__regularExpression.sub(r'\1'+command__printStatus, cppSrc)
				cppFile.seek(0, 0)
				cppFile.write(cppSrc)
				cppFile.close()
				print "["+fileName+"] is modified."

	print "Process is terminated successfully."
