padJoy - 0.8.2
--------------

This is an extension to play with play station emulators like ePSXe and PCSX.
Credits: I used linuzappz' padXwin as a reference; thanks to lu_zero, Skye, Gismo and others for their support.

Copyright 2002-2003 by Erich Kitzmüller <ammoq@ammoq.com>.
padJoy is released under the GPL license, see license.txt.

New Features:
V0.8.2  - avoid double initialisation (happened in PCSX when padJoy was configured to handle both pads)

V0.8.1	- Bugfix for analog mode (again thanks to Gismo); improved handling of axis events (makes macros shorter, less CPU-cycles spent)

Important: Configuration is saved to cfg/padJoy.cfg, so you must create a cfg directory (relative to the directory where you start the emulation). The file name of the config file has been changed from padAmmoq.cfg to padJoy.cfg, so you might want to copy or rename padAmmoq.cfg, to keep your configuration.

It has been developed an tested with Mandrake Linux 9.1 (previous versions were made with SuSE Linux 7.3 and SuSE Linux 8.1).

Please note: With some distros, some actions are required to get other plugins to work (Pete's MesaGL). These action cause padJoy to hang or crash in "multithreaded mode". If you encouter this problem, disable "multithreaded" in the config screen.

Installation: Copy or move bin/libpadJoy-0.8.so into the plugin directory of your emulation. Copy or move bin/cfgPadJoy into the cfg directory of your emulation.

To compile, just type
	make

if this doesn't work, try
	makedep
	make

You need gtk-devel to compile.

Device file name; try /dev/input/js0 or /dev/js0

(To test if a device file is working, try
	od -xw8 /dev/input/js0
you should see output like that when pressing buttons or moving the pad:
0000000 d900 0026 0000 0081
0000010 d900 0026 0000 0181
0000020 d900 0026 0000 0281
0000030 d900 0026 0000 0381
0000040 d900 0026 0000 0481
0000050 d900 0026 0000 0581
0000060 d900 0026 0000 0681
0000070 d900 0026 0000 0781
0000100 d900 0026 0000 0881
0000110 d900 0026 0000 0981
0000120 d900 0026 0000 0a81
0000130 d900 0026 0000 0b81
0000140 d900 0026 0000 0082
0000150 d900 0026 0000 0182
0000160 d900 0026 0000 0282

with one new line every time you press or release a button.

If you get no response, try
	modprobe joydev
as root. On some distros (like my Mandrake 9.1), you need to execute
	chmod a+r /dev/input/js0
as root.


Macros: in the configuration screen, there are new buttons labeled "M1", "M2"
and "M3". Click on this button to define the event that launches a button.
Then click on "Def" to define the sequence of events that should be executed
when launching the macro. Note that also the timing of events is recorded.
Recording stops after 2 seconds of inactivity.
To playback a macro, press the correspondig button and keep it pressed.
Once you release the button, playback is stopped.
You can program the launch event as part of the macro sequence; this makes
it a looping makro. So if "A" is mapped to the X button of the pad and "C"
is the lauch event for the macro, recording "Apress" "Arelease" "Cpress"
as a macro gives you continuous fire on X.

Questions:
Once you get it working, I would be happy to get feedback about the following
questions:

- which Linux version, which joydevice are you using?

- what was the device file name?

- which emulation?

- everything works?

- if you have problems, which settings?

Please send answers, comments, requests to ammoq@ammoq.com .

Have fun, ammoQ


Version history (features of previous versions, look above for new features):
-----------------------------------------------------------------------------
V0.8.0	- analog support; configuration is now a seperate program, to fix PCSX 1.5 issue; renamed config file from padAmmoq.cfg to padJoy.cfg (for consistency)

V0.7.3	- increased number of buttons to 32 and axes to 20, some improvements that should help to configure analog sticks (thanks to Gismo)

V0.7.2	- minor improvements: more debug messages, better support for configuration for PSX2USB converters, minzero and maxzero now in the config file (thanks to Danny Milosavljevic)

V0.7.1	- fixed a bug when using two pads (thanks to Oyvind Saether)

V0.7	- symbolic event names instead of numbers in the config file, some fixes for macros including keyboard events

V0.6.2	- increased number of axes from 6 to 10

V0.6.1	- increased zero range for analog joy devices

V0.6	- support for macros

V0.5	- support for mulithreaded reading of joy device input (hope it helps those who have timing problems)
	- disabled keyboard support for ePSXe (doesnt't work anyway) - use the internal keyboard support of ePSXe instead, this works concurrently with this Plugin

V0.4	- redesign of the code
	- support for mixed input from joy devices and keyboard (PCSX needs that)
	- axes configurable

V0.3	- support for PCSX byte order

V0.2	- config dialogue, about dialogue

V0.1	- first alpha version, support for joy devices everything hardcoded, easy to compile (doesn't need GTK or X header files)

