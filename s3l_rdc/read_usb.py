import serial
#import urllib
#import urllib2
import time;
import datetime;

import pymongo;
from pymongo import MongoClient

s = serial.Serial('/dev/ttyUSB0',115200,timeout=None)
#s = 0
client = MongoClient('54.215.3.34',27017)
db = client['snowflake_ui_development']

#data_input = {"sensor_id": 1,
#	      "temperature": -100,
#	      "timestamp": datetime.datetime.utcnow(),
#	      "slot": 0}

data_collection = db.data
#post_id = data_collection.insert(data_input)
#print post_id




if not s.isOpen():
    s.open()

while True:
    l = s.readline()
    tmp = l.split(',')
    if len(tmp) != 4:
        continue
    #url = 'http://128.12.167.67/sensorRead.php'
    
    if tmp[3].endswith("\n"):
        tmp[3] =tmp[3][:-1]
    localtime = time.asctime(time.localtime(time.time()))
    print "Local Time:", localtime
    print tmp
    
    aa = tmp[3];
    
    values = {'sensor_id'    : int(float(tmp[0])),
              'slot'  : int(tmp[1]),
              'timestamp'    : datetime.datetime.utcnow(),
              'temperature'    : int(aa[0:2])}
    data_id = data_collection.insert(values)
    
    f = open(tmp[0]+'.csv','a+')
    f.write(',')
    f.write(','.join(tmp[3].split(' ')))
    f.close()
    #data = urllib.urlencode(values)
    #req = urllib2.Request(url, data)
    #response = urllib2.urlopen(req)
    #if 'OK!' == response.read():
    #    print tmp
    #    continue
    #else:
    #    break 
s.close()