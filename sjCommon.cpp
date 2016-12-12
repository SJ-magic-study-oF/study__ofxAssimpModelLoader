/************************************************************
************************************************************/
#include "sjCommon.h"

/************************************************************
************************************************************/
BOOT_MODE BootMode;

/************************************************************
************************************************************/
/******************************
******************************/
void printMessage(const char* message)
{
	printf("\n> %s\n", message);
}

/******************************
******************************/
int Dice_index(int *Weight, int NUM)
{
	/***********************
	cal sum of Weight
	***********************/
	int TotalWeight = 0;
	int i;
	for(i = 0; i < NUM; i++){
		TotalWeight += Weight[i];
	}
	
	if(TotalWeight == 0) { return -1; }
	
	/***********************
	random number
	***********************/
	int RandomNumber = rand() % TotalWeight;
	
	/***********************
	define play mode
	***********************/
	int index;
	for(TotalWeight = 0, index = 0; /* */; index++){
		TotalWeight += Weight[index];
		if(RandomNumber < TotalWeight) break;
	}
	
	return index;
}

