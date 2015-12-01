#Tutorial 2: How to Test the Hardware (Hello World)

By [Verônica Pontes](veronicayamee@hotmail.com) and [Larissa Pereira de Queiroz](Larissa_pqueiroz@hotmail.com)

**1.** Plug in the mote and check if a USB icon is shown on the top right corner of your VMware and if the mote is connected to the virtual machine. Connect your mote after the VMware starts.

**2.** To test if your mote is connected, run the following command:

		user@instant-contiki :~$ ~/contiki/tools/sky/motelist-linux

You should see at least one reference.

**3.** Go to the directory: /home/user/contiki/examples/hello-world

**4.** Run the following command step by step and wait the last command to be executed and then type the next command:

		user@instant-contiki :~$ make clean TARGET=sky
		user@instant-contiki :~$ make hello-world TARGET=sky
		user@instant-contiki :~$ sudo make hello-world.upload TARGET=sky
**5.** For the password, type "user".

**6.** When the last command is executing, you should see the LEDs on your mote flash. Once the last command is executed, type following command:

		user@instant-contiki :~$ sudo make login TARGET=sky

**7.** Now press the Reset button on the mote, you should see "Hello, World" on your terminal.
