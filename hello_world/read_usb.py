import serial
#import urllib
#import urllib2
import time;
import datetime;
import json
import requests

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
    #print tmp
    
    sensor_id = tmp[0].replace("\x00","");
    packet_id = tmp[1].replace("\x00","");

    # determine how many samples -- i2c
    sample_num = int((len(tmp) - 2)/7)
    #print sample_num
    data = tmp[2:len(tmp)]
    #print data

    acc_x = []
    acc_y = []
    acc_z = []
    gyro_x = []
    gyro_y = []
    gyro_z = []
    temperature = []

    # temperature TF: 14 bits
    # y = 340*x-12421 -- x = (y+12421)/340

    # accelerometer TF: 16 bits (signed)
    # y = -/+ 2g, a = 16384, b = 0 (default)
    # y = -/+ 4g, a = 8192, b = 0
    # y = -/+ 8g, a = 4096, b = 0
    # y = -/+ 16g, a = 2048, b = 0

    # gyro TF: 16 bits (unsigned)
    # y = -/+ 250 degree/sec, a = 131, b = 32786 (default)
    # y = -/+ 500 degree/sec, a = 65.5, b = 32786
    # y = -/+ 1000 degree/sec, a = 32.8, b = 32786
    # y = -/+ 2000 degree/sec, a = 16.4, b = 32786



    for n in range(0,sample_num):
        sample = data[(n)*7:(n+1)*7]

        print sample

        val = []

        y = float(sample[0].replace("\x00",""))
        acc_x.append(y/16384)
        val.append(y/16384)


        y = float(sample[1].replace("\x00",""))
        acc_y.append(y/16384)
        val.append(y/16384)

        y = float(sample[2].replace("\x00",""))
        acc_z.append(y/16384)
        val.append(y/16384)


        y = float(sample[4].replace("\x00",""))
        gyro_x.append((y)/131)
        val.append((y)/131)

        y = float(sample[5].replace("\x00",""))
        gyro_y.append((y)/131)
        val.append((y)/131)

        y = float(sample[6].replace("\x00",""))
        gyro_z.append((y)/131)
        val.append((y)/131)

        y = float(sample[3].replace("\x00",""))
        temperature.append((y+12421)/340)
        val.append((y+12421)/340)

        print val

        

    # format JSON data
    data_array = [
    {
        'station': 'Stanford',
        'mote_id': sensor_id,
        'timeslot': int(packet_id),
        'timestamp': time.time(),
        'data': {
            'acc_x': acc_x,
            'acc_y': acc_y,
            'acc_z': acc_z,
            'gyro_x': gyro_x,
            'gyro_y': gyro_y,
            'gyro_z': gyro_z,
            'temperature': temperature
        }
    }]

    # print data_array

    JSON_data = json.dumps(data_array[0],sort_keys=False)

    print JSON_data

    payload = {'json_payload': JSON_data}

    headers = {'Content-type': 'application/json', 'Accept': 'application/json'}

    r = requests.post("http://54.213.119.190:5000/data/post",data=JSON_data,headers=headers)

    print r

    
    
    #for i in range(3,len(tmp)-1):
    #data = tmp[i].replace("\x00","");
    #values = {'sensor_id'    : int(sensor_id),
    #     'slot'  : int(slot_num),
    #     'timestamp'    : datetime.datetime.utcnow(),
    #     'temperature'    : hex(data)}
    #data_id = data_collection.insert(values)

    #fn_name = str(datetime.date.today())+'-S'+tmp[0]
    #fn_name = 'test5'
    #f = open(fn_name +'.csv','a+')
    #output_line = ','.join(tmp[3:len(tmp)])
    #output_line_corr = output_line.replace("\x00","");
    #f.write(output_line_corr)
    #f.close()

    #fn_name = str(datetime.date.today())+'-S'+tmp[0]+'format'
    #fn_name = 'test1_format'
    #f = open(fn_name +'_format'+'.csv','a+')
    #output_line = ','.join(tmp[3:len(tmp)])
    #output_line_corr = output_line.replace("\x00","");
    #output_line_corr = str(localtime)+','+output_line_corr
    #f.write(output_line_corr)

    #f.close()

s.close()