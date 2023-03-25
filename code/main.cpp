#include <memory>

#include "robot_control.h"
#include "detector.h"
#include "robot.h"
#include "order_manager.h"
#include "drink_control.h"
#include "robot_mqtt.h"
using namespace std;
using namespace cv;


int main () {
    shared_ptr<Detector> det(new Detector(0));
    shared_ptr<Robot> robot(new Robot_mqtt("192.168.1.4", 1883));
    shared_ptr<Drink_control> drink_control(new Drink_control);
    shared_ptr<Order_manager> order_control(new Order_manager);

    shared_ptr<Robot_control> rob_control(new Robot_control(det, robot, order_control, drink_control));

    det->set_color_bot_front(34, 65, 255);
    det->set_color_bot_rear(176, 169, 220);
    det->set_color_target(84, 170, 128);


    rob_control->run();

  return 0;
}