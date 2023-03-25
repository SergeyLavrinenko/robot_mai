#include "message_validator.h"
#include <iostream>
 
using namespace rapidjson;

std::unique_ptr<Message> MessageValidator::validateMessage(std::string message) {
    Document document;
    document.Parse(message.c_str());
    if (!document.IsObject() || document.HasParseError() || !document.HasMember("cmd") || !document["cmd"].IsString()) {
        return nullptr;
    }

    Message *msg = new Message();
    auto uPrt = std::unique_ptr<Message>(msg);

    auto cmd = document["cmd"].GetString();
    msg->mp.cmd = -1;
    msg->mp.speed = -1;
    msg->isManual = true;
    if (strcmp(cmd,"forward") == 0) {
        msg->mp.cmd = 0;
    } 
    if (strcmp(cmd, "back") == 0) {
        msg->mp.cmd = 1;
    }
    if (strcmp(cmd, "left") == 0) {
        msg->mp.cmd = 2;
    }
    if (strcmp(cmd, "right") == 0) {
        msg->mp.cmd = 3;
    }
    if (strcmp(cmd, "stop") == 0) {
        msg->mp.cmd = 4;
        return uPrt;
    }

    if (strcmp(cmd, "auto") == 0) {
        return validateAuto(message);
    }

    if (msg->mp.cmd < 0) {
        return nullptr;
    }

    if (!document.HasMember("val") || !document["val"].IsDouble()) {
        return nullptr;
    }
    
    msg->mp.valMs = (int)(document["val"].GetDouble() * 1000);
    if (msg->mp.valMs < 0) {
        return nullptr;
    }

    if (document.HasMember("spd") && document["spd"].IsDouble()) {
        int speed = (int)(document["spd"].GetDouble() * 100);
        if (speed<0 || speed>100) {
            return nullptr;
        } 
        msg->mp.speed = speed;
    } 
    return uPrt;
}


std::unique_ptr<Message> MessageValidator::validateAuto(std::string message) {
    Document document;
    document.Parse(message.c_str());
    Message *msg = new Message();
    auto uPrt = std::unique_ptr<Message>(msg);
    msg->isManual = false;

    if (!document.HasMember("markers_screen") || !document["markers_screen"].IsArray()) {
        return nullptr;
    }

    if (!document.HasMember("floor_coords") || !document["floor_coords"].IsArray()) {
        return nullptr;
    }

    if (!document.HasMember("robot_screen_up") || !document["robot_screen_up"].IsArray()) {
        return nullptr;
    }

    if (!document.HasMember("robot_screen_down") || !document["robot_screen_down"].IsArray()) {
        return nullptr;
    }

    if (!document.HasMember("target") || !document["target"].IsArray()) {
        return nullptr;
    }
    
    auto markers = document["markers_screen"].GetArray();
    if (markers.Size() != 4) {
        std::cerr << "markers_screen size != 4" << std::endl;
        return nullptr;
    }

    auto realCoords = document["floor_coords"].GetArray();
    if (realCoords.Size() != 4) {
        std::cerr << "floor_coords size != 4" << std::endl;
        return nullptr;
    }

    auto robotUp = document["robot_screen_up"].GetArray();
    if (robotUp.Size() != 2) {
        std::cerr << "robot_screen_up size != 2" << std::endl;
        return nullptr;
    }

    auto robotDown = document["robot_screen_down"].GetArray();
    if (robotDown.Size() != 2) {
        std::cerr << "robot_screen_up size != 2" << std::endl;
        return nullptr;
    }

    auto target = document["target"].GetArray();
    if (target.Size() != 2) {
        std::cerr << "target size != 2" << std::endl;
        return nullptr;
    }

    try {
        msg->ap.target.x = target[0].GetDouble();       
        msg->ap.target.y = target[1].GetDouble();

        msg->ap.curPos.up.x = robotUp[0].GetDouble();       
        msg->ap.curPos.up.y = robotUp[1].GetDouble();

        msg->ap.curPos.down.x = robotDown[0].GetDouble();       
        msg->ap.curPos.down.y = robotDown[1].GetDouble();

        msg->ap.realCoords.tl.x = realCoords[0].GetArray()[0].GetDouble();
        msg->ap.realCoords.tl.y = realCoords[0].GetArray()[1].GetDouble();
        msg->ap.realCoords.tr.x = realCoords[1].GetArray()[0].GetDouble();
        msg->ap.realCoords.tr.y = realCoords[1].GetArray()[1].GetDouble();
        msg->ap.realCoords.br.x = realCoords[2].GetArray()[0].GetDouble();
        msg->ap.realCoords.br.y = realCoords[2].GetArray()[1].GetDouble();
        msg->ap.realCoords.bl.x = realCoords[3].GetArray()[0].GetDouble();
        msg->ap.realCoords.bl.y = realCoords[3].GetArray()[1].GetDouble();

        msg->ap.screenCoords.tl.x = markers[0].GetArray()[0].GetDouble();
        msg->ap.screenCoords.tl.y = markers[0].GetArray()[1].GetDouble();
        msg->ap.screenCoords.tr.x = markers[1].GetArray()[0].GetDouble();
        msg->ap.screenCoords.tr.y = markers[1].GetArray()[1].GetDouble();
        msg->ap.screenCoords.br.x = markers[2].GetArray()[0].GetDouble();
        msg->ap.screenCoords.br.y = markers[2].GetArray()[1].GetDouble();
        msg->ap.screenCoords.bl.x = markers[3].GetArray()[0].GetDouble();
        msg->ap.screenCoords.bl.y = markers[3].GetArray()[1].GetDouble();

        return uPrt;

    } catch (std::exception &e) {
        std::cerr << "Got exception while extracting coordinates: " << e.what() << std::endl;
        return nullptr;
    }
}


bool Message::isManualMessage() {
    return isManual;
}

manual_message_payload Message::getManualMessagePayload() {
    return mp;
}

auto_message_payload Message::getAutoMessagePayload() {
    return ap;
}