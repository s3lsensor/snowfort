#!/usr/bin

import serial
import time,re,math
from time import gmtime,strftime
import datetime
from ctypes import *
import sys

# function to open usb
def openUSB(port):
	try:
		usb_obj = serial.Serial(port,115200,timeout=None)
	except serial.SerialException:
		usb_obj = None

	if usb_obj is not None:
		if (not usb_obj.isOpen()):
			usb_obj.open()
			print "Open port " + port
	else:
		#print "Open USB fails"
		raise ValueError('Open USB fails')

	return usb_obj

# function to read data from usb
def readUSB(usb_obj):
	# byteToRead = usb_obj.inWaiting()
	# if byteToRead <= 0:
	# 	return (None,None)


	# readOut = usb_obj.read(byteToRead)
	if usb_obj.readable():
		readOut = usb_obj.readline()
		#print readOut
		# readOut_xx = [c_ushort(ord(v)) for v in readOut]
		# print readOut_xx
	else:
		readOut = None

	
	#readTime = strftime("%H,%M,%S",time.localtime())
	readTime = datetime.datetime.now()

	return (readOut,readTime)


# script starts here

usb_port1 = '/dev/ttyUSB0'
usb_obj = openUSB(usb_port1)

if len(sys.argv) > 1:
	fn = sys.argv[1]
else:
	fn = "temp"

print fn
fn_csv = fn+'.csv'
fn_handle = open(fn_csv,'w+')

fn_raw = fn+'_raw.txt'
fn_raw_handle = open(fn_raw,'w+')

start_time = datetime.datetime.now()

current_line = ""
line_is_break = False
START_BYTE = 126
START_STR = chr(START_BYTE) + chr(START_BYTE)
last_readTime = start_time

buffer_string = ''

buffer_list_raw = []
buffer_list = []

while True & (usb_obj is not None):
	try:
		#readData,readTime = readUSB(usb_obj)
		byteToRead = usb_obj.inWaiting()
		buffer_string = buffer_string + usb_obj.read(byteToRead)
		readTime = datetime.datetime.now()

		if '\n\n' in buffer_string:
			lines = buffer_string.split('\n\n')
			#print lines
			readData = lines[-2]
			buffer_string = lines[-1]
		else:
			continue
		
		#print readData,buffer_string

		if readData is None:
			continue
		else:
			delta_time = readTime - start_time
			delta2_time = readTime - last_readTime
			total_microsecond = float(delta_time.seconds)*1e6 + float(delta_time.microseconds)

			total_esp_time = float(delta2_time.seconds)*1e6 + float(delta2_time.microseconds)
			last_readTime = readTime

			buffer_list_raw.append('[SNOWFORT]'+str(int(total_microsecond))+','+readData+'\n\n')
			#fn_raw_handle.write('[SNOWFORT]'+str(int(total_microsecond))+','+readData+'\n\n')
		
		# check if the first two chars at the start delimiter bytes
		idx = readData.find(START_STR)

		if idx >= 0:
			current_line = readData[idx:]
		else:
			if line_is_break is False: # current line is not samples
				continue
			else: # second part of the line
				current_line = current_line + readData


		# convert char in readData to uint8_t
		readOut_char = [c_ushort(ord(v)) for v in current_line]
		readOut_payload = readOut_char[6:]

		#print readOut_char


		if len(readOut_char) < 6:
			print len(readOut_char),line_is_break
			print readOut_char


		if line_is_break == False and len(readOut_char) < 6:
			print "continue here"
			continue

		# get packet information
		packet_counter = readOut_char[3].value
		sn_id = readOut_char[2].value
		packet_seq = readOut_char[4].value
		payload_len = readOut_char[5].value

		#print sn_id,packet_counter,packet_seq,payload_len

		# check if the current line length is current
		if len(readOut_payload) != payload_len:
			# line break
			line_is_break = True
			#print "line break", len(readOut_payload)
			#print len(readData),type(readData)
			continue
		else: 
			line_is_break = False
			current_line = ""
			#print sn_id,packet_seq,payload_len,len(readOut_payload)


		# output header
		output_header = ",".join([str(sn_id),str(packet_counter),str(packet_seq),str(payload_len)])

		print readTime,delta_time.seconds,total_microsecond, total_esp_time,output_header

		if payload_len == 0:
			output_str = ",".join(["0"]*6)
			fn_handle.write(str(int(total_microsecond))+','+output_header+','+output_str+'\n')
			continue

		# break samples
		sample_high = readOut_payload[1::2]
		sample_low = readOut_payload[0::2]
		counter = 0
		output_list = []
		for s1,s2 in zip(sample_high,sample_low):
			s = c_ushort(s1.value << 8)
			s = c_short(s.value | s2.value)

			output_list.append(str(s.value))
			counter = counter + 1

			if counter == 6:
				output_str = ",".join(output_list)
				#fn_handle.write(str(readTime)+','+str(int(total_microsecond))+','+output_header+','+output_str+'\n')
				#fn_handle.write(str(int(total_microsecond))+','+output_header+','+output_str+'\n')
				output_msg = str(int(total_microsecond))+','+output_header+','+output_str+'\n'
				fn_handle.write(output_msg)
				#buffer_list.append(output_msg)
				output_list = []
				counter = 0




	except KeyboardInterrupt:
		break


usb_obj.close()

# for msg in buffer_list:
# 	fn_handle.write(msg)

for msg in buffer_list_raw:
	fn_raw_handle.write(msg)

fn_handle.close()
fn_raw_handle.close()


	

		
