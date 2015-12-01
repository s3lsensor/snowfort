# Tutorial 5: Using analog sensor -- ADXL335

By [Verônica Pontes](veronicayamee@hotmail.com) and [Larissa Pereira de Queiroz](Larissa_pqueiroz@hotmail.com)

The ADXL335 is a low power, complete 3-axis accelerometer with analog outputs. The sensor measures acceleration with a minimum full-scale range of ±3 g. It can measure the static acceleration of gravity in tilt-sensing applications, as well as dynamic acceleration resulting from motion, shock, or vibration. It captures the x, y and z channel at the same time.

**1. How to connect and use the sensor**

* Follow the layout in the figure below and connect the sensor with the mote:

![Test Image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/layout2.png)

* Connect the sensor with the computer as shown below:

![Test Image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/Sensor%20analogico.jpg)

* Go to the directory `/home/user/snowfort/snowfort_example` in the terminal
* Open the file `ADXL335_example.c` and look for the line with `#define SAMPLE_RATE 20` so you can adjust the frequency
* Save the change
* Compile and load the file `ADXL335_example.c` into your mote
* Log into your mote
* You should see the output messages like the figure below:

![test image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/adxl%20output.png)


**2. How to interpret data**

In the output, the first column represents the acceleration on x-axis, the second column is the acceleration on y-axis, the third column is the acceleration on z-axis, and the fourth column is a voltage reference.

The following formula is used in order to get the real acceleration values:

			V_in = reading value/2048 * 1.5V
			Acc = (V_in - 1.5V)/(300mV/g)
