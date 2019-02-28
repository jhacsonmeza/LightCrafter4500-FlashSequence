# LightCrafter 4500

DLP LightCrafter 4500 command line utility for creating pattern sequences with images stored in flash memory. By default, the internal trigger is used and projected patterns are 8-bit depth.

## Folder Contents

* Debug
	- LightCrafter4500Flash.exe: executable.
	- hidapi.dll: DLL necessary for USB communication with the LightCrafter for run executable.
* LightCrafter4500Flash:
	- LightCrafter4500Flash.cpp: main code.
	- dlpc350_api.cpp, dlpc350_api.h, dlpc350_common.cpp, dlpc350_common.h, dlpc350_error.h, dlpc350_usb.cpp and dlpc350_usb.h: APIs for commands supported by LightCrafter 4500.
	- hidapi.h and hidapi.lib: module for USB communication.
	- getopt.c and getopt.h: library for parsing command line arguments.
	- LightCrafter4500Flash.vcxproj: Visual Studio 2017 project.

## Usage

There four principal parse options: `PatternExposure`, `PatternPeriod`, `Repeat`, and `Sequence`. `PatternExposure` and `PatternPeriod` define the exposure time and frame period in microseconds (μs), respectively. Minimum value that both can take is 8333 μs for the case of 8-bit depth images (for more information about minimum pattern exposure see [User's Guide](http://www.ti.com/lit/ug/dlpu011f/dlpu011f.pdf) page 45). `Repeat` defines if pattern sequences are repeated indefinitely (set to 1) or not (set to 0). `Sequence` defines the sequence of flash images by providing the index of images without spaces, e.g., 012 represent the sequence of image 0, image 1 and image 2.

 

Run example:

	LightCrafter4500Flash --PatternExposure 600000 --PatternPeriod 600000 --Repeat 0 --Sequence 0102