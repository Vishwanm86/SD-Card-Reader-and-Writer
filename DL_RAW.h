//Data Logger code, Version 1
//Written by: Vishww NM
//Date      : 25 Dec 2024
//Micro-controller: Arduino Uno
//Hardware: Datalogger ver 1.0 

//This header file declares the global variables

//define header file name (should be same as that of file name saved)
// String header_name = "DL_RAW.h";

//this below definition avoids adding header file twice
#ifndef header_name
#define header_name

#define AVG_WINDOW 1024
/*
// Variables that hold the analog values
float channel_0 = 0;
float channel_1 = 0;
float channel_2 = 0;
float channel_3 = 0;
float channel_4 = 0;
float channel_5 = 0;*/
float channels[6] = {0};

const String file_name = "DLFSF.txt"; //File name to store the data
const String header_name_file = "TS1, TS2, BAT1, BAT2, BAT3, BAT4, I1, I2"; // header
const String err_string = "Error opening file: ";

String dataString;
/*
float channel_0_SF  = 0.103; //Channel 0 SCALE factor
float channel_1_SF  = 0.103; //Channel 1 SCALE factor
float channel_2_SF  = 0.103; //Channel 2 SCALE factor
float channel_3_SF  = 0.103; //Channel 3 SCALE factor
float channel_4_SF  = 0.36; //Channel 4 SCALE factor
float channel_5_SF  = 0.36; //Channel 5 SCALE factor
  */
  float scaleFactors[6] = {0.103, 0.103, 0.103, 0.103, 0.36, 0.36};

float offset_channel_4 = 511; //offset value for current sensor in channel 4
float offset_channel_5 = 511; //offset value for current sensor in channel 4
#endif //close ifndef
