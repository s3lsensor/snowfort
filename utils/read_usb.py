import serial
#import urllib
#import urllib2
import time;
import datetime;

#import pymongo;
#from pymongo import MongoClient

s = serial.Serial('/dev/ttyUSB0',115200,timeout=None)
#client = MongoClient('54.215.19.254',27017)
#db = client['snowflake_ui_development']


#data_collection = db.data
#post_id = data_collection.insert(data_input)
#print post_id


if not s.isOpen():
    s.open()

while True:
    l = s.readline()
    tmp = l.split(',')
    if len(tmp) < 4:
        continue

    localtime = time.asctime(time.localtime(time.time()))
    print "Local Time:", localtime
    print tmp
    
    sensor_id = tmp[0].replace("\x00","");
    slot_num = tmp[1].replace("\x00","");
    
    
    #for i in range(3,len(tmp)-1):
	#data = tmp[i].replace("\x00","");
	#values = {'sensor_id'    : int(sensor_id),
	#	  'slot'  : int(slot_num),
	#	  'timestamp'    : datetime.datetime.utcnow(),
	#	  'temperature'    : hex(data)}
	#data_id = data_collection.insert(values)
	
    fn_name = str(datetime.date.today())+'-S'+tmp[0]
    f = open(fn_name +'.csv','a+')
    output_line = ','.join(tmp[3:len(tmp)])
    output_line_corr = output_line.replace("\x00","");
    f.write(output_line_corr)
    f.close()

    fn_name = str(datetime.date.today())+'-S'+tmp[0]+'format'
    f = open(fn_name +'.csv','a+')
    output_line = ','.join(tmp[3:len(tmp)])
    output_line_corr = output_line.replace("\x00","");
    output_line_corr = str(localtime)+','+output_line_corr
    f.write(output_line_corr)

    f.close()

s.close()
