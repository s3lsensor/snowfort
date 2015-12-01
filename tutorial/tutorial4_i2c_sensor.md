#Tutorial 4: Using I2C sensor -- MPU 6050
By [Verônica Pontes](veronicayamee@hotmail.com) and [Larissa Pereira de Queiroz](Larissa_pqueiroz@hotmail.com)

The MPU-6050 is a sensor which contains an accelerometer and a gyroscope in a single chip and uses a standard I2C-bus for data transmission. It is very accurate as it contains 16-bits analog to digital conversion hardware for each channel. It captures the x, y and z channel at the same time.

**1. How to connect and use the sensor**

* Follow the layout in the figure below to connect the sensor with the mote

![Test Image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/layout.jpg)

* Connect the sensor with the computer (see section 2)
* Go to the directory `/home/user/snowfort/snowfort_example` in your terminal
* Open the file `MPU6050_example.c` and look for the line with `#define SAMPLE_RATE 20` so you can choose the frequency you need and save the change

The timer on the mote uses 1024 ticks to represent 1 second. Here, instead of defining the sampling rate in seconds, we use ticks to represent it. For example, if we want to have a sampling frequency of 50Hz, the associated sampling rate (as an integer) in ticks is computed as follows:

		Sample_rate = 1024/50 = 20


* Compile and load `MPU6050_example.c` into your mote (see section 3)
* Log into your mote (see section 4)
* You should see the output messages like the figure that follows:

![Test Image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/example_output.png)


**1.1. How to change the Full Scale Range**

As you can see on the sensor <a href="http://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf">Register Map</a>, on pages 14 and 15, you can select the full scale range of the gyroscope or accelerometer outputs by controlling the bits 3 and 4 of that register according to each table.

**Gyroscope**

_AFS\_SEL_ selects the full scale range of the gyroscope outputs according to the following table:

![Test Image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/gyrosensitivity.png)

According to the table above,

- If you need the range to be ±250°/sec, the `FS_SEL` should be equal to 0. Since, in the binary numeral system, 0 is represented by 00, the  bits 4 and 3 should be 0 and 0, respectively.
- If you need the range to be ±500°/sec, the `FS_SEL` should be equal to 1. Since, in the binary numeral system, 1 is represented by 01, the  bits 4 and 3 should be 0 and 1, respectively.
- If you need the range to be ±1000°/sec, the `FS_SEL` should be equal to 2. Since, in the binary numeral system, 2 is represented by 10, the  bits 4 and 3 should be 1 and 0, respectively.
- If you need the range to be ±2000°/sec, the `FS_SEL` should be equal to 3. Since, in the binary numeral system, 3 is represented by 11, the  bits 4 and 3 should be 1 and 1, respectively.

To change the range, go to the file `MPU6050_example.c` and look for the line responsible by the gyroscope range. You should find a line like `read_mpu_reg(MPU_GYRO_CONFIG,&MPU_config)`. Right below it, you should see something like `MPU_config = MPU_config & ~BV(3);`, where the number between () is the bit you want to be zero. To change a bit to 1, you will add a line like `MPU_config = MPU_config | BV(4);`, where the number between () is the bit you want to set to be one.

For example, if you have the code as:

			read_mpu_reg(MPU_GYRO_CONFIG,&MPU_config);
			MPU_config = MPU_config & ~BV(3);
			MPU_config = MPU_config | BV(4);
			write_mpu_reg(MPU_GYRO_CONFIG,MPU_config);


The second line sets bit 3 to zero and the third line sets bit 4 to one. Thus, you have the configuration as 10 in the register, which corresponds to `FS_SEL = 2` and, according to the table, sets your full scale range to ±1000°/sec.


**Accelerometer**

_AFS\_SEL_ selects the full scale range of the accelerometer outputs according to the following table:

![Test Image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/acc_sensitivity.png)

Following the same logic as the gyroscope,

- If you need the range to be  ±2g, the `AFS_SEL` should be equal to 0. Since, in the binary numeral system, 0 is represented by 00, the  bits 4 and 3 should be 0 and 0, respectively.
- If you need the range to be  ±4g, the `AFS_SEL` should be equal to 1. Since, in the binary numeral system, 1 is represented by 01, the  bits 4 and 3 should be 0 and 1, respectively.
- If you need the range to be ±8g, the `AFS_SEL` should be equal to 2. Since, in the binary numeral system, 2 is represented by 10, the  bits 4 and 3 should be 1 and 0, respectively.
- If you need the range to be ±16g, the `AFS_SEL` should be equal to 3. Since, in the binary numeral system, 3 is represented by 11, the  bits 4 and 3 should be 1 and 1, respectively.

To change the range, go to the file `MPU6050_example.c` and look for the line responsible by the accelerometer range. This time, you should find a line like `read_mpu_reg(MPU_ACCEL_CONFIG,&MPU_config);` followed by `MPU_config = MPU_config & ~BV(3);`, where the number between () is the bit you want to be zero. To change a bit to 1, you will add a line like `MPU_config = MPU_config | BV(4);`, where the number between () is the bit you want to set to be one.

So if you have the code as follows:

			read_mpu_reg(MPU_ACCEL_CONFIG,&MPU_config);
			MPU_config = MPU_config & ~BV(3);
			MPU_config = MPU_config | BV(4);
			write_mpu_reg(MPU_ACCEL_CONFIG,MPU_config);


The second line sets bit 3 to zero and the third line sets bit 4 to one. Thus, you have the configuration as 10 in the register, which correspond to `AFS_SEL = 2` and, according to the table, sets your full scale range to ±8g.


**2. Connect with the computer**

* To connect the sensor with the computer, plug the USB cable to the mote and the computer like the figure below:

![Test Image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/mpu.png)


**3. Compile**

* To compile the file and upload this file to the mote, type:

		sudo ./build.sh make 1 MPU6050_example 1

**4. Login**

* To log into the mote, type:

		sudo ./build.sh login

**5. How to interpret the data**

The output messages were shown previously in Figure 6. The first three columns represent the acceleration on x-axis, on the y-axis and on the z-axis, respectively. The fourth column represents the temperature, and the last three columns represent the angular velocity on x, y and z axes, respectively. The line on the code responsible for this output is `PRINTF("%d,%d,%d,%d,%d,%d,%d\n",sample_data.data.accel_x,sample_data.data.accel_y,sample_data.data.accel_z,sample_data.data.tp,sample_data.data.gyro_x,sample_data.data.gyro_y,sample_data.data.gyro_z);`.


* Gyroscope

The datasheet indicates four different sensitivities. Each sensitivity has a correspondent scale factor as shown below:

|    Sensitivity    | Scale Factor |
|:-----------------:|:------------:|
|     ±250°/sec     |     131      |
|     ±500°/sec     |     65.5     |
|     ±1000°/sec    |     32.8     |
|     ±2000°/sec    |     16.4     |

So, for example, if you use a sensitivity of 250°/sec, you have to divide the output by 131 in order to get the real angular velocity reading.


* Accelerometer

Again, the datasheet shows four different sensitivities with their respective scale factors as follows:

|  Sensitivity  | Scale Factor |
|:-------------:|:------------:|
|      ±2g      |     16,384   |
|      ±4g      |     8,192    |
|      ±8g      |     4,096    |
|      ±16g     |     2,048    |

If you use a sensitivity of 2g, you have to divide the output by 16,384 to get the real acceleration reading.
