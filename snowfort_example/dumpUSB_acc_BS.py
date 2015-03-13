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
	else:
		readOut = None

	
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

fn = 'Test1'

fn_csv = fn+'.csv'
fn_handle = open(fn_csv,'w+')
fn_raw = fn+'_raw.txt'
fn_handle2 = open(fn_raw,'w+')
fn_status = fn+'_status.csv'
fn_handle3 = open(fn_status,'w+')

data_str = ''

while True & (usb_obj is not None):
	readData,readTime = readUSB(usb_obj)

	if readData is None:
		continue
	else:
		raw_data = readTime + ',' + readData
		fn_handle2.write(raw_data+'\n')


	# change string to list
	#ascii_vec = getValueFromASCII(readData)

	ascii_vec = readData.split(",")
	print ascii_vec

	if len(ascii_vec) > 3:

		ascii_vec[-1] = ascii_vec[-1].strip()

		# SN_id
		SN_id = ascii_vec[0]
		pkt_seq = ascii_vec[1]
		payload_len = ascii_vec[2]

		status_vec = ascii_vec[0:3]
		fn_handle3.write(readTime+','+(','.join(status_vec))+'\n')


		total_sample = (len(ascii_vec)-3)/3;
		#sample_vec = []

		for i in range(0,total_sample):
			#sample_vec.append(conv_func(a,16384,0))
			sample_vec = []
			sample_vec.append(readTime)
			sample_vec.append(SN_id)
			sample_vec.append(pkt_seq)
			sample_vec.append(payload_len)

			x = conv_func(int(ascii_vec[3+i*3]),16384,0)
			y = conv_func(int(ascii_vec[3+i*3+1]),16384,0)
			z = conv_func(int(ascii_vec[3+i*3+2]),16384,0)

			sample_vec.append(x)
			sample_vec.append(y)
			sample_vec.append(z)
			
			ss = ",".join(sample_vec)
			print ss
			fn_handle.write(ss+"\n")
		else:
			print readData

		


