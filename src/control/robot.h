#pragma once

class Robot{
public:
    Robot();
    virtual void left(float time);
    virtual void right(float time);
    virtual void forward(float time);
};