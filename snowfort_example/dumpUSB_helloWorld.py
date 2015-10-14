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
		#print readOut

	
	readTime = strftime("%H,%M,%S",time.localtime())

	#print readTime+","+readOut

	# remove illegal characters
	# readOut = readOut.decode('ascii',errors='ignore')
	readOut_list = list(readOut)
	readOut_char = []


	#print str(len(readOut_list)) + " readout " + readOut

	for item in readOut_list:
		a = c_ubyte(ord(item))
		print item,a.value

	


# script starts here

usb_port1 = '/dev/ttyUSB0'
usb_obj = openUSB(usb_port1)



fn = 'temp.csv'
fn_handle = open(fn,'a+')

data_str = ''

while True & (usb_obj is not None):
	readUSB(usb_obj)



	#data_str = data_str + readData

	#print data_str
	#print readData + '|'
	# if '\n' in readData:
	# 	data_buffer = data_str.split('\n')
	# 	data_str = data_buffer.pop() # remove last element
		
	# 	#print data_buffer
	# 	for sample in data_buffer:
	# 		# change string to list
	# 		sample_list = sample.split(',')

	# 		if len(sample_list) != sample_len:
	# 			print "skip|" + sample
	# 			continue

	# 		sn_num = sample_list[0]

	# 		sample_val = tf_mpu6050(sample_list[1:len(sample_list)])
	# 		sample_val.insert(0,sn_num)

	# 		final_output = readTime+","+",".join(sample_val)

	# 		print final_output

	# 		fn_handle.write(final_output+"\n")


	







