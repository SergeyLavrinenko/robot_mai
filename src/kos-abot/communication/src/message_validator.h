#ifndef _MESSAGE_VALIDATOR_H
#define _MESSAGE_VALIDATOR_H

#include <string>
#include <memory>
#include "rapidjson/document.h"

struct manual_message_payload {
    int cmd;
    int valMs;
    int speed;
};


typedef struct {
    float x;
    float y;
} point;

typedef struct {
    point up;
    point down;
} cur_pos;

typedef struct {
    point tl;
    point tr;
    point br;
    point bl;
} rectangle;

typedef struct {
    point target;
    cur_pos curPos;
    rectangle realCoords;
    rectangle screenCoords;
} auto_message_payload;

class MessageValidator;

class Message {
    bool isManual;
    manual_message_payload mp;
    auto_message_payload ap;
public:
    bool isManualMessage();
    manual_message_payload getManualMessagePayload();
    auto_message_payload getAutoMessagePayload();

    friend class MessageValidator;
};


class MessageValidator {
    static std::unique_ptr<Message> validateAuto(std::string message);
public:
    static std::unique_ptr<Message> validateMessage(std::string message);
};

#endif