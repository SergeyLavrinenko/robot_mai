#include "order_manager.h"

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mosquitto.h>

#define input_topic "/robot_mai/order_manager/ch1"
#define output_topic "/robot_mai/order_manager/ch2"

bool Order_manager::order_exist_bool = false;
bool Order_manager::glass_is_given_bool = true;

void Order_manager::connect_callback(struct mosquitto *mosq, void *obj, int result) {
    printf("connect callback(order_manager mqtt), rc=%d\n", result);
}

void Order_manager::message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);
    
     if (!strcmp((char*)(message->payload), "new_order")){
        if(order_exist_bool){
            char data[] = "order_alredy_exist";
            mosquitto_publish( mosq, NULL, output_topic, strlen(data), data, 0, 0 );
        }
        else{
            char data[] = "order_ok";
            mosquitto_publish( mosq, NULL, output_topic, strlen(data), data, 0, 0 );
            order_exist_bool = true;
        }
    }

    if (!strcmp((char*)(message->payload), "glass_ok")){
            glass_is_given_bool = true;

            char data[] = "go_to_home";
            mosquitto_publish( mosq, NULL, output_topic, strlen(data), data, 0, 0 );
    }
}

Order_manager::Order_manager(char *s, int p): server(s), port(p){
    mosquitto_lib_init();
    mosq = mosquitto_new(clientid, true, 0);
    if (mosq){
        mosquitto_connect_callback_set(mosq, connect_callback);
        mosquitto_message_callback_set(mosq, message_callback);
        rc = mosquitto_connect_async(mosq, server, port, 60);
        mosquitto_loop_start(mosq);
        mosquitto_subscribe(mosq, NULL, input_topic, 0);
    }
}


bool Order_manager::order_exist(){
    return order_exist_bool;
}

void Order_manager::send_push(){
    char data[] = "order_arrived";
    mosquitto_publish(mosq, NULL, output_topic, strlen(data), data, 0, 0 );
    glass_is_given_bool = false;
}

bool Order_manager::glass_is_given(){
    return glass_is_given_bool;
}

void Order_manager::order_end(){
    order_exist_bool = false;
}


void Order_manager::loop(){
        if(rc){
            printf("connection error! (order_manager mqtt)\n");
            mosquitto_reconnect(mosq);
        }
}

Order_manager::~Order_manager(){
    mosquitto_disconnect(mosq);
    mosquitto_loop_stop(mosq, false);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}