#include <iostream>
#include <memory>
#include "robot_control.h"
#include <opencv2/opencv.hpp>


void Robot_control::processEvents() {

    switch (this->state) {
    case EWait:
        if (ord_control->order_exist()) {
            this->state = EMoveToHome;
            this->flag_start = true;
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
        std::cout<<"move_state "<<this->move_state<<"\n";
        if(move_state == toHome){
            this->state = EMoveToHome;
        }
        else if(move_state == toDrink){
            this->state = EMoveToDrink;
        }
        else{
            this->state = EMoveToTarget;
        }
        break;
    case EMoveToHome:
        this->move_state = toHome;
        if(det_main->has_delta_angle_to_home(5)){
            if (det_main->get_angle_to_home() > 0)
                this->state = ERotateLeft;
            else
                this->state = ERotateRight;
            break;
        }

        if(det_main->has_delta_distance_to_home(160)){
            this->time_forward = 0.1;
            this->state = EForward;
        }
        else{
            this->state = EMoveToDrink;
        }
        
        break;
    case EMoveToDrink:
        this->move_state = toDrink;
        if(det_drink->has_delta_angle_to_target(5)){
            if (det_drink->get_angle_to_target() > 0)
                this->state = ERotateLeft;
            else
                this->state = ERotateRight;
            break;
        }

        if(det_drink->has_delta_distance_to_target(20)){
            this->time_forward = 0.05;
            this->state = EForward;
        }
        else{
            if(this->flag_start){
                this->state = EFillingGlass;
                this->flag_start = false;
            }
            else{
                this->state = EWait;
                ord_control->order_end();
            }
        }
        break;
    case EMoveToTarget:
        this->move_state = toTarget;
        if(det_main->has_delta_angle_to_target(5)){
            if (det_main->get_angle_to_target() > 0)
                this->state = ERotateLeft;
            else
                this->state = ERotateRight;
            break;
        }

        if(det_main->has_delta_distance_to_target(160)){
            this->time_forward = 0.1;
            this->state = EForward;
        }
        else{
            this->state = ESendPush;
        }
        break;
    case ERotateLeft:
        robot->left(0.02);
        this->state = EMove;
        break;
    case ERotateRight:
        robot->right(0.02);
        this->state = EMove;
        break;
    case EForward:
        robot->forward(this->time_forward);
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
        this->det_main->update_image();
        this->det_main->draw_image();
        this->det_drink->update_image();
        this->det_drink->draw_image();
        std::cout<<this->state<<"\n";
        ord_control->loop();

        this->processEvents();

        char c=(char)waitKey(25);
            if(c==27)
                break;
    }
}

Robot_control::Robot_control(std::shared_ptr<Detector> d_m, std::shared_ptr<Detector> d_d, std::shared_ptr<Robot> r, std::shared_ptr<Order_manager> o, std::shared_ptr<Drink_control> dr): det_main(d_m), det_drink(d_d), robot(r), dr_control(dr), ord_control(o){
    this->state = EWait;
}