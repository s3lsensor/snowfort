#!/usr/bin

import serial
import time,re,math
from time import gmtime,strftime
import datetime
from ctypes import *

# global variable
sample_len = 8

# function to open usb
def openUSB(port):
	try:
		usb_obj = serial.Serial(port,115200,timeout=None)
	except serial.SerialException:
		usb_obj = None

	if usb_obj is not None:
		if not usb_obj.isOpen():
			usb_obj.open()
			print "Open port " + port
	else:
		print "Open USB fails"

	return usb_obj

# convert unsigned int into signed int
def uintToint(num):
	if num > 32768:
		return num - 65536
	else:
		return num

# function to read data from usb
def readUSB(usb_obj):
	# byteToRead = usb_obj.inWaiting()
	# if byteToRead <= 0:
	# 	return (None,None)


	# readOut = usb_obj.read(byteToRead)
	if usb_obj.readable():
		readOut = usb_obj.readline()
		print readOut

	
	readTime = strftime("%H,%M,%S",time.localtime())

	#print readTime+","+readOut

	# remove illegal characters
	# readOut = readOut.decode('ascii',errors='ignore')
	readOut_list = list(readOut)
	readOut_char = []


	#print str(len(readOut_list)) + " readout " + readOut

	for item in readOut_list:
		readOut_char.append(c_ubyte(ord(item)))

	#print readOut_char

	total_len = int(len(readOut_char))
	readOut_int = []

	# readOut_int.append(str(c_ushort(readOut_char[0].value).value))
	# readOut_int.append(str(c_ushort(readOut_char[1].value).value))
	#print len(readOut_char),total_len,range(0,total_len)
	for i in range(0,total_len):
		a = c_short(readOut_char[i].value)
		readOut_int.append(str(a.value))
		#print a,i



	#print readOut_int
	readOut_clean = ",".join(readOut_int)

	#print readOut_clean

	return (readOut_clean,readTime)



# script starts here

usb_port1 = '/dev/ttyUSB1'
usb_obj = openUSB(usb_port1)



fn = 'temp.csv'
fn_handle = open(fn,'a+')

data_str = ''

while True & (usb_obj is not None):
	readData,readTime = readUSB(usb_obj)

	if readData is None:
		continue


	# change string to list
	sample_list = readData.split(',')

	print sample_list

	sn_num = sample_list[0]
	pkt_num = sample_list[1]
	pkt_len = sample_list[2]

	sample_val = sample_list[3:len(sample_list)]
	#sample_val = sample_list[1:len(sample_list)]
	sample_val.insert(0,pkt_len)
	sample_val.insert(0,pkt_num)
	sample_val.insert(0,sn_num)
	

	final_output = readTime+","+",".join(sample_val)
	print final_output

	fn_handle.write(final_output+"\n")





# usb_obj.close()
# fn_handle.close()







