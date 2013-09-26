This is a test bootloader that I used to debug the serial, video and video console code. 

Stuff in here was then transfered to the SBOOT bootloader that was used to debug serial KITL.  
The SBOOT loader will download an image but given it's serial, it is very slow.

This directory provides standalone boot code that doesn't depend on anything else in the BSP
aside from a few files in the \inc directory.