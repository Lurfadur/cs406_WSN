#ifndef RBS_h
#define RBS_h

#include <TimeLib.h>
#include <Arduino.h>
#include <MsTimer2.h>
#include "QueueArray.h"


#define INPUT_SIZE 30 // max input size for one message
#define NODE_COUNT 3 // total number of nodes

#define NODE_ID 3 // this node's ID. Must be unique to each node


class RBS{
  public:
    RBS();
    void send_message();
    void read_message();
    void findDiff();
    void blinkLED();
    void setID(int);
    void setNodeCount(int);
    unsigned long localTime = 1;
    bool sendMsgs = false;
    int nodeId = 0;
    QueueArray <unsigned long>timeTable;
  
};


#endif
