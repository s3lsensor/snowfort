#!/usr/bin

import serial
import time,re,math
from time import gmtime,strftime
import datetime
from ctypes import *


# function to open usb
def openUSB(port):
	try:
		usb_obj = serial.Serial(port,baudrate=115200,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS,timeout=None)
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

	return readOut_char

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

fn = 'temp1'

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
	ascii_vec = getValueFromASCII(readData)

	while ascii_vec[0].value == 0:
		ascii_vec.pop(0)

	#if ascii_vec[len(ascii_vec)-1].value == 10:
	#	ascii_vec.pop()


	if len(ascii_vec) > 3:

		if (len(ascii_vec)-3) > c_ushort(ascii_vec[2].value).value:
			sample_len = c_ushort(ascii_vec[2].value).value
			if ascii_vec[len(ascii_vec)-1].value == 10:
				ascii_vec.pop()

		elif (len(ascii_vec)-3) <= c_ushort(ascii_vec[2].value).value:
				sample_len = len(ascii_vec)-3


		# SN_id
		SN_id = str(c_ushort(ascii_vec[0].value).value)
		pkt_seq = str(c_ushort(ascii_vec[1].value).value)
		payload_len = str(c_ushort(ascii_vec[2].value).value)

		# avoid incorrect frame message
		if (ascii_vec[0].value == 73) and (ascii_vec[1].value == 110) and (ascii_vec[2].value == 99):
			continue


		print SN_id,pkt_seq,payload_len

		status_vec = [SN_id,pkt_seq,payload_len]
		fn_handle3.write(readTime+','+(','.join(status_vec))+'\n')


		total_sample = (sample_len/3)/2

		#sample_vec = []

		#print ascii_vec
		for i in range(0,total_sample):
			#sample_vec.append(conv_func(a,16384,0))
			#print i,total_sample,payload_len,len(ascii_vec)
			sample_vec = []
			sample_vec.append(readTime)
			sample_vec.append(SN_id)
			sample_vec.append(pkt_seq)
			sample_vec.append(payload_len)

			x_int = byteComb(ascii_vec[3+i*6+1],ascii_vec[3+i*6])
			y_int = byteComb(ascii_vec[3+i*6+3],ascii_vec[3+i*6+2])
			z_int = byteComb(ascii_vec[3+i*6+5],ascii_vec[3+i*6+4])

			x = conv_func(x_int,16384,0)
			y = conv_func(y_int,16384,0)
			z = conv_func(z_int,16384,0)

			sample_vec.append(x)
			sample_vec.append(y)
			sample_vec.append(z)
			
			ss = ",".join(sample_vec)
			print ss
			fn_handle.write(ss+"\n")
	else:
		print readData

		


