snore - sleep with feedback
===========================

SYNOPSIS

    snore [-v] [NUMBER[SUFFIX]...]

snore pause for NUMBER seconds. SUFFIX may be 's' for seconds (default), 'm'
for minutes, 'h' for hours or 'd' for days. Given two or more arguments, pause
for the amount of time specified by the sum of their values. A visual feedback
is given by printing the flowing of time in both ascending and descending
order. If no arguments are given, snore pauses for 1d (one day).

SHORTCUTS

    Enter or Ctrl-j  show split time

    Ctrl-s           pause execution

    Ctrl-q           resume execution

    Ctrl-c           stop and exit


Installation
------------
Edit config.mk to match your local setup (snore is installed into the
/usr/local namespace by default).

Afterwards enter the following command to build and install snore (if
necessary as root):

    make clean install

Status
------
snore is considered complete and no further development is expected to happen.
