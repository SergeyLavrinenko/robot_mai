#pragma once
#include "robot.h"

class Robot_mqtt:public Robot{

    char* server = nullptr;
    int port;

    void sendtoserver(const char* data);

public:
    Robot_mqtt(char *s, int p);
    void left(float time);
    void right(float time);
    void forward(float time);
};