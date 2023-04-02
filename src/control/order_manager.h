#pragma once


#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mosquitto.h>

class Order_manager{
    char* server = nullptr;
    int port;

    uint8_t reconnect = true;
    char clientid[14] = "order_manager";
    struct mosquitto *mosq;
    int rc = 0;

    static void connect_callback(struct mosquitto *mosq, void *obj, int result);
    static void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

public:

    static bool order_exist_bool;
    static bool glass_is_given_bool;

    Order_manager(char *s, int p);
    Order_manager(Order_manager &d);
    Order_manager& operator=(Order_manager &d);

    bool order_exist();
    void send_push();
    bool glass_is_given();
    void order_end();
    void loop();

    ~Order_manager();
};