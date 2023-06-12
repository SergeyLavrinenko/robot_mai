#include "robot_mqtt.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mosquitto.h>
#include <string.h>
#include <string>
#include <json-c/json.h>
#include "robot.h"
#define NUM_MESSAGES 30
#define THREAD 10
#define KEEP_ALIVE 60
#define MQTT_PUB_TOPIC "abot/command/alex"
#define MQTT_QOS_LEVEL 2
#define MSG_MAX_SIZE 512

void Robot_mqtt::sendtoserver (const char* data) {
    bool clean_session = true;
    struct mosquitto *mosq = NULL;
    mosq = mosquitto_new( NULL, clean_session, NULL );
    mosquitto_connect(mosq, server, port, KEEP_ALIVE);
    mosquitto_publish( mosq, NULL, MQTT_PUB_TOPIC, strlen(data), data, 0, 0 );
    mosquitto_destroy( mosq );
}

Robot_mqtt::Robot_mqtt(char *s, int p): server(s), port(p){
}


void Robot_mqtt::left(float time){
    sendtoserver(("{ \"cmd\":\"left\", \"val\": " + std::to_string(time)  + ", \"spd\": 0.6}").c_str());
}
void Robot_mqtt::right(float time){
    sendtoserver(("{ \"cmd\":\"right\", \"val\": " + std::to_string(time)  + ", \"spd\": 0.6}").c_str());
}
void Robot_mqtt::forward(float time){
    sendtoserver(("{ \"cmd\":\"forward\", \"val\": " + std::to_string(time)  + ", \"spd\": 0.4}").c_str());
}