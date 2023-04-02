#include <iostream>
#include <memory>
#include "robot_control.h"
#include <opencv2/opencv.hpp>


void Robot_control::processEvents() {

    switch (this->state) {
    case EWait:
        if (ord_control->order_exist()) {
            this->state = EFillingGlass;
        }
        break;
    case EFillingGlass:
        if(flag_filling_glass){
            if(dr_control->drink_ready()){
                flag_filling_glass = false;
                this->state = EMoveToTarget;
            }
        }
        else{
            dr_control->open();
            flag_filling_glass = true;
        }
        break;
    case EMove:
        if(flag_move_to_home)
            this->state = EMoveToHome;
        else
            this->state = EMoveToTarget;
        break;
    case EMoveToHome:
        flag_move_to_home = true;
        if(det->has_delta_angle_to_home(5)){
            if (det->get_angle_to_home() > 0)
                this->state = ERotateLeft;
            else
                this->state = ERotateRight;
            break;
        }

        if(det->has_delta_distance_to_home(160)){
            this->state = EForward;
        }
        else{
            this->state = EWait;
            ord_control->order_end();
            flag_move_to_home = false;
        }
        
        break;
    case EMoveToTarget:
        if(det->has_delta_angle_to_target(5)){
            if (det->get_angle_to_target() > 0)
                this->state = ERotateLeft;
            else
                this->state = ERotateRight;
            break;
        }

        if(det->has_delta_distance_to_target(160))
            this->state = EForward;
        else
            this->state = ESendPush;
        
        break;
    case ERotateLeft:
        robot->left();
        this->state = EMove;
        break;
    case ERotateRight:
        robot->right();
        this->state = EMove;
        break;
    case EForward:
        robot->forward();
        this->state = EMove;
        break;
    case ESendPush:
        ord_control->send_push();
        this->state = EGivingGlass;
        break;
    case EGivingGlass:
        if(ord_control->glass_is_given())
            this->state = EMoveToHome; 
        break;
    }
}
void Robot_control::run() {

    while (1){
        this->det->update_image();
        this->det->draw_image();
        std::cout<<this->state<<"\n";
        ord_control->loop();

        this->processEvents();

        char c=(char)waitKey(25);
            if(c==27)
                break;
    }
}

Robot_control::Robot_control(std::shared_ptr<Detector> d, std::shared_ptr<Robot> r, std::shared_ptr<Order_manager> o, std::shared_ptr<Drink_control> dr): det(d), robot(r), dr_control(dr), ord_control(o){
    this->state = EWait;
}