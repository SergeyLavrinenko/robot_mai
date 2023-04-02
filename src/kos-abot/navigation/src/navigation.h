#ifndef _NAVIGATION_NAVIGATION_CTR_H
#define _NAVIGATION_NAVIGATION_CTR_H

#include <thread>
#include <mutex>

#include "pwm_controller.h"

enum NavigationTasks {
    GO_FORWARD,
    GO_BACKWARD,
    GO_LEFT,
    GO_RIGHT
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


class NavigationController {
    std::thread activeTask;
    std::mutex runMut;
    std::unique_ptr<pwm_controller> pwmController;
    bool isRunning;
    bool stop;

    cur_pos pos;

    void handleManualTask(NavigationTasks task, int durationMs, int speed);
    void handleAutoTask(cur_pos position, point direction, rectangle screnCoords, rectangle realCoords);

public:
    NavigationController(GpioHandle *handle);
    int startManualTask(NavigationTasks task, int durationMs, int speed);
    int startAutoTask(cur_pos position, point direction, rectangle screnCoords, rectangle realCoords);
    int stopTask();
};

#endif