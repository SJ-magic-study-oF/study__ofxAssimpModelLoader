/************************************************************
description
	ofxArtnetにて、common.hが使用されている。
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/************************************************************
************************************************************/
// #define SJ_DEBUG
// #define SJ_REC


/************************************************************
************************************************************/
enum BOOT_MODE{
	MODE_TEST,
	MODE_DEMO,
	
	NUM_MODES,
};

enum{
	BUF_SIZE = 512,
};

enum COLORPATTERNS{
	COLPATTERN_AQUA,
	COLPATTERN_MAGMA,
	COLPATTERN_DIGITAL,
	COLPATTERN_SEXY,
	
	COLPATTERN_TRIP,
	COLPATTERN_REGGAE,
	COLPATTERN_SAMBA,
	COLPATTERN_SWEETS,
	// ↑以上:Autoで使用
	
	/*
	COLPATTERN_RAINBOW,
	COLPATTERN_BLIZZARD,
	
	
	COLPATTERN_AUTO,
	*/
	
	
	NUM_COLORPATTERNS,
};

/************************************************************
************************************************************/
#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);

/************************************************************
************************************************************/
extern BOOT_MODE BootMode;

/************************************************************
************************************************************/
extern void printMessage(const char* message);
extern int Dice_index(int *Weight, int NUM);
