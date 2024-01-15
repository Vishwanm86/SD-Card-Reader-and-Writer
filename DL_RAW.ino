//Data Logger code, Version 1
//Written by: Satish Mesha
//Micro-controller: ATmega328P U-TH
//Hardware: Datalogger ver 1.0 (supplied by MESHA)
#include "DL_RAW.h"  //library file for global variable declaration
#include <SPI.h>
#include <SD.h>

File myFile;
File root;
File entry;

unsigned long last_ts;
unsigned long curr_ts;
bool trf_status = false;
int fileCount = 0;

void setup() {
  // put your setup code here, to run once:
  // Setup Serial connection
  Serial.begin(115200);

  dataString.reserve(80);

  // SD CARD
  while (!SD.begin(14000000, 10)) {
    Serial.println(F("Waiting for SD card initialization"));
    delay(1000);
  }
  
  Serial.println(F("Storage Enabled: V1.3"));

  //open file and write header
  while (!myFile) {
    myFile = SD.open(file_name, FILE_WRITE);
  }

  myFile.println(header_name_file);
  myFile.close();  

  delay(1000);
  last_ts = millis();
}

void loop() {

  if (trf_status) { // Device is in file transfer mode
    enter_file_transfer();
  } else {
    data_acq();
  }

  // Check if there is any data from the serial interface
  if (Serial.available() > 0) {
    if (Serial.read() == 'S') {
      trf_status = true;
      enter_file_transfer();
    }
  }
}

void data_acq(){
  curr_ts = millis();
  while ((millis() - last_ts) < 1000) {
    delay(2);
  }
  last_ts = millis();
  

          for (int i = 0; i < AVG_WINDOW; i++) {
  for (int j = 0; j < 6; j++) {
    channels[j] += analogRead(A0 + j + (j > 3 ? 2 : 0));
  }
}
          for (int i = 0; i < 6; i++)
          {
            channels[i] /= AVG_WINDOW;
          }

          channels[0] -= channels[1];
          channels[1] -= channels[2];
          channels[2] -= channels[3];
          dataString = String(curr_ts) + "," + String(last_ts);

          for (int i = 0; i < 4; i++)
          {
            dataString += "," + String(channels[i] * scaleFactors[i]);
          }

          dataString += ",";
          dataString += String((channels[4] - offset_channel_4) * scaleFactors[4]);
          dataString += ",";
          dataString += String((channels[5] - offset_channel_5) * scaleFactors[5]);
          write_data_file(dataString);
          for (int i = 0; i < 6; i++)
  {
    channels[i] = 0;
  }
  /*

  for (int i = 0; i < AVG_WINDOW; i++) {
    channel_0 = channel_0 + analogRead(A0);  // From hardware, BAT1
    channel_1 = channel_1 + analogRead(A1);
    channel_2 = channel_2 + analogRead(A2);
    channel_3 = channel_3 + analogRead(A3);
    channel_4 = channel_4 + analogRead(A6);
    channel_5 = channel_5 + analogRead(A7);
  }
  channel_0 = channel_0 / AVG_WINDOW;
  channel_1 = channel_1 / AVG_WINDOW;
  channel_2 = channel_2 / AVG_WINDOW;
  channel_3 = channel_3 / AVG_WINDOW;
  channel_4 = channel_4 / AVG_WINDOW;
  channel_5 = channel_5 / AVG_WINDOW;

  channel_0 = channel_0 - channel_1;
  channel_1 = channel_1 - channel_2;
  channel_2 = channel_2 - channel_3;

  dataString = String(curr_ts);
  dataString += ",";
  dataString += String(last_ts);
  dataString += ",";
  dataString += String(channel_0 * channel_0_SF);
  dataString += ",";
  dataString += String(channel_1 * channel_1_SF);
  dataString += ",";
  dataString += String(channel_2 * channel_2_SF);
  dataString += ",";
  dataString += String(channel_3 * channel_3_SF);
  dataString += ",";
  dataString += String((channel_4 - offset_channel_4) * channel_4_SF);
  dataString += ",";
  dataString += String((channel_5 - offset_channel_5) * channel_5_SF);


  write_data_file(dataString);

  channel_0 = 0;
  channel_1 = 0;
  channel_2 = 0;
  channel_3 = 0;
  channel_4 = 0;
  channel_5 = 0;
  */
}

//write calibrated data into file
void write_data_file(String& dataString) {
  Serial.println(dataString);
  myFile = SD.open(file_name, FILE_WRITE);

  //if file is open write into the file
  if (myFile) {
    myFile.println(dataString);  //write into file
    // myFile.flush();
    myFile.close();              //close file
  }
}

/****** Functions to manage file transfer when BT is connected ******/
void enter_file_transfer() {
  char in1, in2;
  Serial.println("Main Menu:");
  Serial.println("1. Read File");
  Serial.println("2. Delete File");

  do {
    while (!Serial.available()) {
    }

    in1 = Serial.read();
    if (in1 == 'X') {
      trf_status = false;
      return;
    }
  } while (!((in1 == '1') || (in1 == '2')));

  listAndReadFiles();

  do {
    while (!Serial.available()) {
    }

    in2 = Serial.read();
    if (in2 == 'X') {
      trf_status = false;
      return;
    }
    in2 = in2 - '0';
  } while (in2 > fileCount);

  switch (in1) {
    case '1':
      // selectFileToRead();
      readFile(int(in2));
      break;
    case '2':
      deleteFile(int(in2));
      break;
  }
}

void listAndReadFiles() {
  root = SD.open("/");
  fileCount = 0;
  Serial.println("Available Files:");

  while (true) {
    entry = root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    fileCount++;
    Serial.print(fileCount);
    Serial.print(": ");
    Serial.print(entry.name());
    Serial.print("  :");
    Serial.println(entry.size());
    entry.close();
  }

  root.close();
}

void readFile(int fileNumber) {
  // int byteCount=0;
  root = SD.open("/");
  int currentFile = 0;

  while (true) {
    entry = root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    currentFile++;
    if (currentFile == fileNumber) {
      Serial.print("AA " + String(entry.size()) + " Reading file: ");
      Serial.println(entry.name());

      while (entry.available()) {
        char data = entry.read();// Change to int if not works
        Serial.print(data);
        // delayMicroseconds(100);
        // byteCount++;
        // wdt_reset();// Add a 2-microsecond delay after each byte
        // Check for a specific value (e.g., 225)
      }
      // entry.close();
      // break;
    }
    entry.close();
  }

  root.close();
  Serial.println("ZZ End of the file: ");
  delay(2000);
}

void deleteFile(int fileNumber) {

  root = SD.open("/");
  int currentFile = 0;

  while (true) {
    entry = root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    currentFile++;
    if (currentFile == fileNumber) {
      String fileName = entry.name();
      Serial.print("Deleting file: ");
      Serial.println(fileName);
      entry.close();
      root.close();
      SD.remove(fileName);
      break;
    }
    else {
      entry.close();
    }
  }
  if (root) root.close();
  // root = SD.open("/"); // Reopen the root directory after file deletion
}
