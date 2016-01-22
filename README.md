snore - sleep with feedback
===========================

SYNOPSIS

    snore [-v] [NUMBER[SUFFIX]...]

snore pause for NUMBER seconds. SUFFIX may be 's' for seconds, 'm' for minutes,
'h' four hours or 'd' for days. If the suffix is wrong or not given 's' is
default. Given two or more arguments, pause for the amount of time specified by
the sum of their values. A visual feedback is given by printing the flowing of
time in both ascending and descending order.

Installation
------------
Edit config.mk to match your local setup (snore is installed into the
/usr/local namespace by default).

Afterwards enter the following command to build and install snore (if
necessary as root):

    make clean install

If you are going to use the default bluegray color scheme it is highly
recommended to also install the bluegray files shipped in the dextra package.
