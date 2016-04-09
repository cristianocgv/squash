# Introduction #
This page will explain how to install `squash` on Microsoft Windows.


# Before you begin #
Before attempting to build `squash` on Microsoft Windows you MUST have Trolltech's Qt libraries installed. You can download the open source version of Qt (for free) at http://trolltech.com/developer/downloads/qt/windows. Download the executable and install Qt, installing the Ming libraries if required.


# Building from source #

## Getting the code ##

You can download either the [official source release](http://code.google.com/p/squash/downloads/list?q=label:Type-Source) of squash, or [grab the latest source code](http://code.google.com/p/squash/source/checkout) from subversion.

## Compiling ##

Start a Qt command prompt by navigating to the Start menu entry entitled _Qt 4.4.0 Command Prompt_ (or similar).

Navigate to the `squash` source code and issue the following commands
  * `qmake Makefile.qmake`
  * `make`