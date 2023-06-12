#include <memory>

#include "robot_control.h"
#include "detector.h"
#include "robot.h"
#include "order_manager.h"
#include "drink_control.h"
#include "robot_mqtt.h"
using namespace std;
using namespace cv;

#define mqtt_host "127.0.0.1"
#define mqtt_port 1883


int main () {
    shared_ptr<Detector> det_main(new Detector(1, false));
    shared_ptr<Detector> det_drink(new Detector(3, true));
    shared_ptr<Robot> robot(new Robot_mqtt(mqtt_host, mqtt_port));
    shared_ptr<Drink_control> drink_control(new Drink_control);
    shared_ptr<Order_manager> order_control(new Order_manager(mqtt_host, mqtt_port));

    shared_ptr<Robot_control> rob_control(new Robot_control(det_main, det_drink, robot, order_control, drink_control));

    det_main->set_color_bot_front(164, 100, 215);
    det_main->set_color_bot_rear(102, 224, 220);
    det_main->set_color_target(83, 92, 178);
    det_main->set_color_home(2, 73, 228);

    det_drink->set_color_bot_front(166, 132, 204);
    det_drink->set_color_bot_rear(99, 236, 140);
    det_drink->set_point_target(310, 170);


    rob_control->run();

  return 0;
}