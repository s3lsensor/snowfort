# Tutorial 3: Using _build.sh_ script
By [Verônica Pontes](veronicayamee@hotmail.com) and [Larissa Pereira de Queiroz](Larissa_pqueiroz@hotmail.com)


**1. Using _build.sh_ script to create executable application**

 In order to simplify the compile process, SnowFort has a script to run all the compiling commands automatically. This example will help you to get familiar with this script.

- Go to the directory `/home/user/snowfort/snowfort_example` in the Terminal
- The file _build.sh_ runs as follows:

		$ sudo ./build.sh <action> <usb id> <file name> <sensor id>

- _Action_ is the command you want to use.

- _File name_ is the source file that you want to compile. You should not include its attributes. For example, to compile ABC.c, the file name is "ABC", not "ABC.c".

- The _sensor id_ has to be larger than 0.

- _Usb id_ refers to the USB port number. The first mote connected with the USB is 1 and the second mote connected with the USB is 2, etc...

- To clean all the compiled files, use:

		$ sudo ./build.sh clean

- To compile a file and upload this file to the mote, type:

		$ sudo ./build.sh make <usb id> <file name> <sensor id>

E.g.:

	sudo ./build.sh make 1 helloWorld 1


- For MSP simulation application, use:

		sudo ./build.sh test <usb id> <file name> <sensor id>

- For logging into target, use:

		sudo ./build.sh login <usb id>
