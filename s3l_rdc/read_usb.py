import serial
#import urllib
#import urllib2

s = serial.Serial('/dev/ttyUSB0',115200,timeout=None)

if not s.isOpen():
    s.open()

while True:
    l = s.readline()
    tmp = l.split(',')
    if len(tmp) != 4:
        continue
    #url = 'http://128.12.167.67/sensorRead.php'
    #values = {'Node'    : tmp[0],
    #          'Packet'  : tmp[1],
    #          'Time'    : tmp[2],
    #          'Data'    : tmp[3]}
    if tmp[3].endswith("\n"):
        tmp[3] =tmp[3][:-1]
    print tmp
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