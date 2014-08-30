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

	return (readOut,readTime)


# function to convert ASCII to number
def getValueFromASCII(readOut):

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
	#readOut_clean = ",".join(readOut_int)

	#print readOut_clean

	return readOut_int

# function to combine two 1 byte into one 2 bytes
def byteComb(val1,val2):
	a = c_ushort(val1.value << 8)
	a = c_short(a.value | val2.value)

	return str(a.value)

# TF function
def conv_func(sample,a,b):
	value = str(round(float(sample)/a+b,5))
	return value

# script starts here

usb_port1 = '/dev/ttyUSB0'
usb_obj = openUSB(usb_port1)

fn = 'temp.csv'
fn_handle = open(fn,'a+')

data_str = ''

while True & (usb_obj is not None):
	readData,readTime = readUSB(usb_obj)

	if readData is None:
		continue


	# change string to list
	ascii_vec = getValueFromASCII(readData)

	# SN_id
	SN_id = ascii_vec[0].value
	pkt_seq = ascii_vec[1].value
	payload_len = ascii_vec[2].value

	if payload_len % 2 != 0:
		payload_len = payload_len - 1

	total_sample = payload_len/2;
	sample_vec = []

	for i in range(0,total_len):
		a = byteComb(ascii_vec[3+i*2+1],ascii_vec[3+i*2])
		sample_vec.append(conv_func(a,16384,0))

	sample_vec = ",".join(sample_vec)

	print SN_id,pkt_seq,payload_len,total_sample,sample_vec
		


