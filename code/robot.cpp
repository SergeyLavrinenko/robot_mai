#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mosquitto.h>
#include <string.h>
#include <json-c/json.h>
#include "robot.h"
#define NUM_MESSAGES 30
#define THREAD 10
#define MQTT_SERVER "192.168.1.4"
#define KEEP_ALIVE 60
#define MQTT_PORT 1883
#define MQTT_PUB_TOPIC "abot/command/alex"
#define MQTT_QOS_LEVEL 2
#define MSG_MAX_SIZE 512

void sendtoserver (const char* data) {
    bool clean_session = true;
    struct mosquitto *mosq = NULL;
    mosq = mosquitto_new( NULL, clean_session, NULL );
    mosquitto_connect(mosq, MQTT_SERVER, MQTT_PORT, KEEP_ALIVE);
    mosquitto_publish( mosq, NULL, MQTT_PUB_TOPIC, strlen(data), data, 0, 0 );
    mosquitto_destroy( mosq );
}

Robot::Robot(){
    
}
void Robot::left(){
    sendtoserver("{ \"cmd\":\"left\", \"val\": 0.1, \"spd\": 0.2}");
}
void Robot::right(){
    sendtoserver("{ \"cmd\":\"right\", \"val\": 0.1, \"spd\": 0.2}");
}
void Robot::forward(){
    sendtoserver("{ \"cmd\":\"forward\", \"val\": 0.3, \"spd\": 0.2}");
}