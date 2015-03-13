#!/usr/bin

import serial
import time,re,math
from time import gmtime,strftime
import datetime
from ctypes import *


# function to open usb
def openUSB(port):
	try:
		usb_obj = serial.Serial(port,115200,timeout=None)
	except serial.SerialException:
		usb_obj = None
		print serial.SerialException

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

	return (readOut,readTime)



# TF function
def conv_func(sample,a,b):
	value = str(round(float(sample)/a+b,5))
	return value

def only_numerics(seq):
    return filter(type(seq).isdigit, seq)

# script starts here

usb_port1 = '/dev/ttyUSB0'
usb_obj = openUSB(usb_port1)

fn = 'temp.csv'
fn_handle = open(fn,'w+')

data_str = ''

while True & (usb_obj is not None):
	readData,readTime = readUSB(usb_obj)

	if readData is None:
		continue

	data_list = readData.split(",")
	print data_list

	if len(data_list) == 3:

		data_list[-1] = data_list[-1].strip()

		#print int(data_list[0]),int(data_list[1]),int(data_list[2])
	
		sample_vec = []
		sample_vec.append(conv_func(int(data_list[0]),16384,0))
		sample_vec.append(conv_func(int(data_list[1]),16384,0))
		sample_vec.append(conv_func(int(data_list[2]),16384,0))
		ss =  ",".join(sample_vec)
		print ss
		fn_handle.write(ss+"\n")
	else:
		print readData


		


