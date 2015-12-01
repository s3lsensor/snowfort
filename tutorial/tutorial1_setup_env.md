#Tutorial 1: Setting up the environment

By [Verônica Pontes](veronicayamee@hotmail.com) and [Larissa Pereira de Queiroz](Larissa_pqueiroz@hotmail.com)

**1. Download VMWare Player**

Click <a href="https://my.vmware.com/web/vmware/free#desktop_end_user_computing/vmware_player/7_0">here</a> to download the VMWare Player on your computer. It might need a reboot after that.

**2. Download Instant Contiki**

Instant Contiki is an Ubuntu Linux virtual machine that runs in VMWare player and makes Contiki easy to install and get started with. Click <a href="http://sourceforge.net/projects/contiki/files/Instant%20Contiki/">here</a> to download the 2.6 version. After it is done, unzip the file and place the unzipped directory on the desktop.

**3. Starting Instant Contiki**

- Open VMWare Player
- In the panel shown in Figure 1, choose "InstantContiki2.6" and click on "Play virtual machine". If you do not have this virtual machine, follow these steps:
 * Click on "Open a virtual machine"
 * Go to "C:\desktop\InstantContiki2.6"
 * Choose "Instant\_Contiki\_Ubuntu12.04_32-bit.vmx"
 * Click on "Open"
 * Click on "Play virtual machine"
- Log in using the password "user" once you see Figure 2


![test image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/VMware%20Player.png)

**Figure 1**

![test image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/password.png)
**Figure 2**

**4. Starting Cooja**


Cooja is the Contiki network simulator. Cooja allows large and small networks of Contiki motes to be simulated. Motes can be emulated at the hardware level, which is slower but allows precise inspection of the system behavior, or at a less detailed level, which is faster and allows simulation of larger networks.

- To start Cooja:
 * Open a terminal window
 * As shown in Figure 3, go to the Cooja directory:

			cd ~/contiki/tools/cooja
 * Start Cooja with the command:

			ant run

![test image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/cooja.png)
                                            **Figure 3**

 * After Cooja compiles itself, it will start with a blue empty window, shown in the figure below. Now that Cooja is up and running you can try it out with an example simulation.

![test image](https://github.com/s3lsensor/snowfort/blob/master/tutorial/images/cooja2.png)

**5. Running Contiki on hardware**

- Connect the hardware, open a terminal and go to the code directory:

		cd ~/contiki/examples/hello-world
- Compile Contiki and the application:

		sudo make TARGET=sky hello-world
If you plan to compile more than once for the chosen platform, you can ask Contiki to remember your choice of hardware with the special savetarget maketarget, as follows:

		sudo make TARGET=sky savetarget

-  Upload Contiki to the hardware:

		sudo make hello-world.upload

- To see the serial port output of the connected mote, run the following command:

		sudo make login
- Press the reset button on the mote. Something like the following should show up:

		Rime started with address 225.26
		MAC e1:1a:00:00:00:00:00:00 Contiki-2.6-737-gfe0a042 started. Node id is not set.
		CSMA ContikiMAC, channel check rate 8 Hz, radio channel 26
		Starting 'Hello world process'
		Hello, world
