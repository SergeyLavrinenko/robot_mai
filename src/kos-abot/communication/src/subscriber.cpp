#include <iostream>
#include <string_view>
#include <string>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "subscriber.h"

using namespace std::literals;

Subscriber::Subscriber(const subscriber_confg &config_, MessageValidator *messageValidator_, NavigationMessageSender *navMessageSender_)
    : mosquittopp("appalphabot")
{   
    config = config_;
    messageValidator = messageValidator_;
    navMessageSender = navMessageSender_;

    username_pw_set(
        config.mqttUser.c_str(), config.mqttPassword.c_str()
    );
    std::cout << "[Subscriber] connectiong to " << config.host << " " << config.port << std::endl;
    connect(config.host.c_str(), config.port, 60);
}

void Subscriber::on_connect(int rc)
{
    std::cout << "[Subscriber] connected with code "
              << rc << std::endl;

    if (rc == 0)
    {
        subscribe(NULL, config.subTopic.c_str());
    }
}

void Subscriber::on_message(const struct mosquitto_message *message)
{
    if (config.subTopic == message->topic)
    {
        const std::string_view payload{reinterpret_cast<char*>(message->payload),
                                       static_cast<size_t>(message->payloadlen)};
        std::cout << "[Subscriber]" << "Got message with topic: " << message->topic
                  << ", payload: " << payload << std::endl;
        auto messageStr = std::string {payload.begin(), payload.end()};

        auto parsedMsg = messageValidator->validateMessage(messageStr);
        if (parsedMsg == nullptr) {
            std::cerr << "[Subscriber] message is not valid" << std::endl;
            return;
        }

        navMessageSender->sendMessage(std::move(parsedMsg));
    }
}

void Subscriber::on_subscribe(__rtl_unused int        mid,
                              __rtl_unused int        qos_count,
                              __rtl_unused const int *granted_qos)
{
    std::cout << "[Subscriber] " << "Subscription succeeded." << std::endl;
    auto message = "alive";
    auto replyTopic = config.subTopic + "/ctl";
    int msg;
    publish(&msg, replyTopic.c_str(), strlen(message), message);
}
