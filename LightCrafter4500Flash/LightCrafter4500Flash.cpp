// LightCrafter4500Flash.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"

#include "dlpc350_common.h"
#include "dlpc350_usb.h"
#include "dlpc350_api.h"

#include <iostream>
#include "getopt.h"

#include <stdio.h>
#include <tchar.h>


using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char splashLut[64];
	unsigned int exposurePeriod;
	unsigned int framePeriod;
	int numImFlash;
	int repeat;

	// Connect to device: lcrOpen
	DLPC350_USB_Init();
	if (DLPC350_USB_IsConnected()) {
		DLPC350_USB_Close();
	}

	DLPC350_USB_Open();
	if (!DLPC350_USB_IsConnected()) {
		printf("Failed to open");
	}

	unsigned int NumImgInFlash;
	DLPC350_GetNumImagesInFlash(&NumImgInFlash);

	while (1)
	{
		static struct option long_options[] = {
			{_T("PatternExposure"), ARG_REQ, 0, _T('e')},
			{_T("PatternPeriod"), ARG_REQ, 0, _T('p')},
			{_T("Repeat"), ARG_REQ, 0, _T('r')},
			{_T("Sequence"), ARG_REQ, 0, _T('s')},
			{ARG_NULL, ARG_NULL, ARG_NULL, ARG_NULL}
		};

		int option_index = 0;
		int c;
		c = getopt_long(argc, argv, _T("eprs"), long_options, &option_index);

		// Check error
		if (c == -1)
			break;

		switch (c)
		{
		case _T('e'):
			exposurePeriod = _wtoi(optarg);
			break;

		case _T('p'):
			framePeriod = _wtoi(optarg);
			break;

		case _T('r'):
			repeat = _wtoi(optarg);;
			break;

		case _T('s'):
			const wstring ws(optarg);
			const string str(ws.begin(), ws.end());

			numImFlash = str.length(); // Number of image in the sequence

			for (int i = 0; i < str.length(); i++) {
				if (NumImgInFlash - 1 < str[i] - '0')
				{
					printf("Image index error");
					return -1;
				}

				splashLut[i] = str[i] - '0';
			}

			break;
		}
	}



	// Set display mode: lcrSetMode
	bool mode = true; // true Pattern display mode, false Video display mode

	int result = DLPC350_SetMode(mode);
	if (result == -1) {
		printf("Failed to set mode");
	}

	// Stop the current pattern sequence: lcrPatternDisplay
	int action = 0; // 0 stop, 1 pause, 2 start

	result = DLPC350_PatternDisplay(action);
	if (result < 0) {
		printf("Failed to set pattern display");
	}

	// Clear locally stored pattern LUT: lcrClearPatLut
	result = DLPC350_ClearPatLut();
	if (result == -1) {
		printf("Failed to clear stored pattern LUT");
	}

	// Set up pattern LUT: lcrAddToPatLut
	int trigType = 0; // 0 internal, 1 external +, 2 external -, 3 no input trigger
	int bitPlaneGroups = 3; // 3 groups for 8 bit images
	int patNum[] = { 0, 1, 2 }; // Index of the 3 groups of 8 bit
	int bitDepth = 8; //from 0 to 8
	int ledSelect = 7; // 0 no led, 1 red, 2 green, 3 yellow, 4 blue, 5 magenta, 6 cyan, 7 white
	bool invertPat = false; // true invert pattern, false do not invert pattern
	bool insertBlack = false; // true insert black-fill pattern, false do not do it
	bool bufSwap = false; // true perform a buffer swap, false do not do it
	bool trigOutPrev = false;

	for (int i = 0; i < numImFlash; i++) {
		for (int j = 0; j < bitPlaneGroups; j++) {
			if (j == 0) {
				result = DLPC350_AddToPatLut(trigType, patNum[j], bitDepth, ledSelect, invertPat, insertBlack, !bufSwap, trigOutPrev);
			}
			else {
				result = DLPC350_AddToPatLut(trigType, patNum[j], bitDepth, ledSelect, invertPat, insertBlack, bufSwap, trigOutPrev);
			}
			if (result == -1) {
				printf("Failed to add to pattern LUT");
			}
		}
	}

	// Set pattern display data source: lcrSetPatternDisplayMode
	bool external = false; // true patterns from RGB/FPD-link interface, false pattern from flash memory

	result = DLPC350_SetPatternDisplayMode(external);
	if (result == -1) {
		printf("Failed to set pattern display mode");
	}

	// Set the sequence parameters: lcrSetPatternConfig
	unsigned int numPatsForTrigOut2;

	if (repeat)
		numPatsForTrigOut2 = 1;
	else
		numPatsForTrigOut2 = numImFlash * bitPlaneGroups;

	if (DLPC350_SetPatternConfig(numImFlash*bitPlaneGroups, repeat, numPatsForTrigOut2, numImFlash) < 0)
	{
		printf("Failed to set pattern configuration");
		return -1;
	}

	// Set exposure time: lcrSetExposureFramePeriod
	result = DLPC350_SetExposure_FramePeriod(exposurePeriod, framePeriod);
	if (result < 0) {
		printf("Failed to set exposure/frame period");
	}

	// Set the pattern sequence to trigger: lcrSetPatternTriggerMode
	int intExtOrVsync = 1; // 0 VSYNC, 1 internal or external

	result = DLPC350_SetPatternTriggerMode(intExtOrVsync);
	if (result < 0) {
		printf("Failed to set pattern trigger mode");
	}

	// Send pattern LUT to device: lcrSendPatLut
	result = DLPC350_SendPatLut();
	if (result < 0) {
		printf("Failed to send pattern LUT");
	}

	// Send image LUT to device: lcrSendImageLut_2images
	result = DLPC350_SendImageLut(&splashLut[0], numImFlash);
	if (result < 0) {
		printf("Failed to send image LUT");
	}

	// Validate the pattern LUT: lcrValidatePatLutData
	unsigned int status;

	result = DLPC350_ValidatePatLutData(&status);
	if (result == -1) {
		printf("Failed to validate pattern LUT data");
	}

	// Start the pattern sequence: lcrPatternDisplay
	action = 2; // 0 stop, 1 pause, 2 start
	result = DLPC350_PatternDisplay(action);
	if (result < 0) {
		printf("Failed to set pattern display");
	}

	return 0;
}


