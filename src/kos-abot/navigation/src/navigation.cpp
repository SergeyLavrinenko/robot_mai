#include "navigation.h"
#include <iostream>
#include <unistd.h>
#include <gpio/gpio.h>
#include <cmath>


#define MAX_MANUAL_SPEED 30

NavigationController::NavigationController(GpioHandle *handle) {
    pwmController = std::make_unique<pwm_controller>(handle);
}

void NavigationController::handleManualTask(NavigationTasks task, int durationMs, int speed) {
    std::cerr << "[NavigationController] STarting task" << std::endl;
    int speed_ = (speed < 0 || speed > 100) ? MAX_MANUAL_SPEED : speed;

    switch(task){
        case GO_LEFT:
            pwmController->goLeft(speed_);
            break;
        case GO_FORWARD:
            pwmController->goForward(speed_);
            break;
        case GO_BACKWARD:
            pwmController->goBack(speed_);
            break;
        case GO_RIGHT:
            pwmController->goRight(speed_);
            break;
    }

    // sleep 10 ms than check if get stop...
    int sleepTimes = durationMs / 100;
    for (int i=0; i<sleepTimes && !stop; ++i) {
        usleep(100000);
    }

    pwmController->stop();
    runMut.lock();
    isRunning = false;
    runMut.unlock();
    std::cerr << "[NavigationController] Task finished...." << std::endl;
}

int NavigationController::startManualTask(NavigationTasks task, int durationMs, int speed) {
    std::cerr << "[NavigationController] Requested to start manual Task: " << task << " duration " << durationMs << "ms..." << std::endl;
    runMut.lock();
    if (isRunning) {
        runMut.unlock();
        std::cerr << "[NavigationController] another task is ative now..." << std::endl;
        return 0;
    }

    if (activeTask.joinable()) {
        activeTask.join();
    }
    
    isRunning = true;
    stop = false;
    
    activeTask = std::thread(&NavigationController::handleManualTask, this, task, durationMs, speed);
    runMut.unlock();

    return 1;
}


int NavigationController::startAutoTask(cur_pos position, point direction, rectangle screnCoords, rectangle realCoords) {
        std::cerr << "[NavigationController] Requested to start auto Task: ";
    runMut.lock();
    if (isRunning) {
        pos = position;
        runMut.unlock();
        return 1;
    }

    if (activeTask.joinable()) {
        activeTask.join();
    }
    
    isRunning = true;
    stop = false;

    activeTask = std::thread(&NavigationController::handleAutoTask, this, position, direction, screnCoords, realCoords);
    runMut.unlock();
    return 1;
}


void NavigationController::handleAutoTask(cur_pos position, point direction, rectangle screnCoords, rectangle realCoords) {
    cur_pos myPos;
    while(!stop) {
        runMut.lock();
        myPos = pos;
        runMut.unlock();

        point wheelCenter = {
            (myPos.up.x + myPos.down.x) / 2,
            (myPos.down.y + myPos.up.y) / 2,
        };

        std::cerr << wheelCenter.x << " " << wheelCenter.y << std::endl;
        std::cerr << direction.y << " " << wheelCenter.y << std::endl;

        if ((direction.x - wheelCenter.x) * (direction.x - wheelCenter.x) < 10  && 
                (direction.y - wheelCenter.y) * (direction.y - wheelCenter.y) < 10 ) 
                break;

        point myVector = {
                myPos.up.x - myPos.down.x,
                myPos.up.y - myPos.down.y,
        };
        
        point destVector = {
            direction.x - wheelCenter.x,
            direction.y - wheelCenter.y,
        };

        float vecCos = (destVector.x * myVector.y + destVector.y * myVector.x) / (
            sqrt(destVector.x * destVector.x + destVector.y * destVector.y) * sqrt(myVector.x * myVector.x + myVector.y * myVector.y)
        );

        if (vecCos > 0.98) {
            std::cerr << "On the vector... go Forward" << std::endl;
            pwmController->goForward(50);
        } else {
            float dot = myVector.x*-destVector.y + myVector.y*destVector.x;
            if (dot>0) {
                std::cerr << "Not on the vector... go Right" << std::endl;
                pwmController->goRight(35);
            } else {
                std::cerr << "Not on the vector... go Left" << std::endl;
                pwmController->goLeft(35);
            }
        }

        usleep(100000);
    }

    pwmController->stop();
    runMut.lock();
    isRunning = false;
    runMut.unlock();
    std::cerr << "[NavigationController] AutoTask finished...." << std::endl;
}


int NavigationController::stopTask() {
    stop = true;
    
    return 1;
}