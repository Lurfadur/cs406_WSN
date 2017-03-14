#include <TimeLib.h>
#include <Arduino.h>
#include <MsTimer2.h>
#include "RBS.h"
#include "QueueArray.h"


#define INPUT_SIZE 30 // max input size for one message
#define NODE_COUNT 3 // total number of nodes

#define NODE_ID 3 // this node's ID. Must be unique to each node

QueueArray <unsigned long>timeTable;

//unsigned long localTime = 1485131185; // default starting time
unsigned long localTime = 1;
bool sendMsgs = false; // is it ok to broadcast messages?
int nodeId = 0;

/*
   Sends time sync message to other nodes
   Format of message is
   nodeId:time
   where nodeId is the next node to broadcast
   where time is this node's local time
*/
void send_message() {

  //blink for 1/2 second
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);

  int nextNode = NODE_ID;

  nextNode++;

  String msg = (String)nextNode + ":" + (String)timeTable.front();
  Serial.println(msg);

}

/*
   Reads a time sync message in the format
   nodeId:time
   where nodeId is the node to broadcast
   where time is the time from the node sending the message
*/
void read_message() {
  
  if (timeTable.count() != NODE_COUNT) {


    char input[INPUT_SIZE + 1];
    byte bSize = Serial.readBytes(input, INPUT_SIZE);
    input[bSize] = 0;

    char *separator = strchr(input, ':');
    if (separator != 0) {
      // Actually split the string in 2: replace ':' with 0
      separator = 0;

      nodeId = atoi(input);
      ++separator;
      unsigned long nodeTime = atol(separator);

      timeTable.push(nodeTime);
    }
  }
}

/*
   Calculates the average difference between all nodes
   Adjusts local time
   Empties the timeTable
*/
void findDiff() {

  unsigned long locTime = timeTable.front();
  int totalDiff = 0;

  while (timeTable.isEmpty() == false) {
    totalDiff += locTime - timeTable.pop();
  }

  totalDiff = totalDiff / NODE_COUNT;
  totalDiff *= -1;
  locTime += totalDiff;
  setTime(locTime);

  String tempMsg = (String)NODE_ID + "=" + (String)locTime;
  Serial.println(tempMsg);
}

void blinkLED(){
  static boolean output = HIGH;
  digitalWrite(13, output);
  output = !output;

  localTime++;
}

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  setTime(localTime);

  MsTimer2::set(1000, blinkLED);
  MsTimer2::start();
}

void loop() {
  

  if (Serial.available() > 1 && sendMsgs == true) { // time message from node

    read_message();

    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);

    if (nodeId == NODE_ID) {
      send_message();
    }
  }

  if (Serial.available() == 1 && sendMsgs == false) { // action message

    char arg = Serial.read();
    if (arg == 'S') {


      MsTimer2::stop();
      delay(1000);
      MsTimer2::start();
      
      sendMsgs = true;
      //time_t local = now();
      timeTable.push((unsigned long)localTime);

      if (NODE_ID == 1) {
        send_message();
      }
    }
  }
  if (timeTable.count() == NODE_COUNT) { // time to stop TX/RX between nodes and adjust the time
    sendMsgs = false;
    findDiff();
    String tempMsg = (String)NODE_ID + "F";
    Serial.print(tempMsg); // print for finished
  }
  
}

