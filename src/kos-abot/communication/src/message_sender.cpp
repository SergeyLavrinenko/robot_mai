#include "message_sender.h"
#include <iostream>


void NavigationMessageSender::sendManualMessage(std::unique_ptr<Message> message) {
       auto manual_payload = message->getManualMessagePayload();

    echo_NavigationCommand_SetNavigationCommand_req req;
    echo_NavigationCommand_SetNavigationCommand_res res;     
    req.value.command = (uint8_t) manual_payload.cmd;
    req.value.durationMs = manual_payload.valMs;
    req.value.speed = manual_payload.speed;

    if (echo_NavigationCommand_SetNavigationCommand(&proxy->base, &req, NULL, &res, NULL) == rcOk) {
        if (res.result == 1) {
            std::cout << "[NavMessageSender] Command sucsessfull applied by controller" << std::endl;
        } else {
            std::cerr << "[NavMessageSender] Command sent but not applied by controller. COntroller is busy..." << std::endl;
        }

    } else {
          std::cerr << "[NavMessageSender] Error sending command to naviation controller" << std::endl;
    }
    nk_req_reset(&req);
}

#define SET_ENCODED_INT(v) v = (int)(auto_payload.v * 1000);

void NavigationMessageSender::sendAutoMessage(std::unique_ptr<Message> message) {
       auto auto_payload = message->getAutoMessagePayload();

    echo_NavigationCommand_SetAutoNavigationCommand_req req;
    echo_NavigationCommand_SetAutoNavigationCommand_res res;  

    req.value.SET_ENCODED_INT(target.x)
    req.value.SET_ENCODED_INT(target.y)

    req.value.SET_ENCODED_INT(curPos.up.x)
    req.value.SET_ENCODED_INT(curPos.up.y)
    req.value.SET_ENCODED_INT(curPos.down.x)
    req.value.SET_ENCODED_INT(curPos.down.y)

    req.value.SET_ENCODED_INT(realCoords.tl.x)
    req.value.SET_ENCODED_INT(realCoords.tl.y)

    req.value.SET_ENCODED_INT(realCoords.tr.x)
    req.value.SET_ENCODED_INT(realCoords.tr.y)

    req.value.SET_ENCODED_INT(realCoords.br.x)
    req.value.SET_ENCODED_INT(realCoords.br.y)

    req.value.SET_ENCODED_INT(realCoords.bl.x)
    req.value.SET_ENCODED_INT(realCoords.bl.y)

    req.value.SET_ENCODED_INT(screenCoords.tl.x)
    req.value.SET_ENCODED_INT(screenCoords.tl.y)

    req.value.SET_ENCODED_INT(screenCoords.tr.x)
    req.value.SET_ENCODED_INT(screenCoords.tr.y)

    req.value.SET_ENCODED_INT(screenCoords.br.x)
    req.value.SET_ENCODED_INT(screenCoords.br.y)

    req.value.SET_ENCODED_INT(screenCoords.bl.x)
    req.value.SET_ENCODED_INT(screenCoords.bl.y)

    if (echo_NavigationCommand_SetAutoNavigationCommand(&proxy->base, &req, NULL, &res, NULL) == rcOk) {
        if (res.result == 1) {
            std::cout << "[NavMessageSender] Command sucsessfull applied by controller" << std::endl;
        } else {
            std::cerr << "[NavMessageSender] Command sent but not applied by controller. COntroller is busy..." << std::endl;
        }

    } else {
          std::cerr << "[NavMessageSender] Error sending command to naviation controller" << std::endl;
    }
    nk_req_reset(&req);
}


void NavigationMessageSender::sendMessage(std::unique_ptr<Message> message) {
    if (!message->isManualMessage()) {
        std::cerr << "[NavMessageSender] Sending auto message...." << std::endl;
        sendAutoMessage(std::move(message));
        return;
    }

    std::cerr << "[NavMessageSender] Sending manual message...." << std::endl;
    sendManualMessage(std::move(message));
}