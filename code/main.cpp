#include "robot_control.h"
using namespace std;
using namespace cv;


int main () {
    Detector *det = new Detector(0);
    Robot *robot = new Robot;
    Drink_control *drink = new Drink_control;
    Order_manager *order = new Order_manager;

    Robot_control *rob_control = new Robot_control(det, robot, order, drink); 

    det->set_color_bot_front(34, 65, 255);
    det->set_color_bot_rear(176, 169, 220);
    det->set_color_target(84, 170, 128);


    rob_control->run();


    /*while(1){
        //

        det.update_image();
        
        det.draw_rect_bot_front(0, 255, 0);
        det.draw_rect_bot_rear(255, 0 , 0);
        det.draw_rect_target(0, 0, 255);

        Mat res_image = det.get_image();

        // ---- Наносим градусы на картинку ----
        Point text_position(20, 30);
        int font_size = 1;
        Scalar font_Color(90, 100, 0);
        int font_weight = 2;
        putText(res_image, to_string(det.get_angle_to_target()), text_position,FONT_HERSHEY_COMPLEX, font_size,font_Color, font_weight);
        // -------------------------------------
        Point text_position1(20, 60);
        int font_size1 = 1;
        Scalar font_Color1(90, 100, 0);
        int font_weight1 = 2;
        putText(res_image, to_string(det.get_distance_to_target()), text_position1,FONT_HERSHEY_COMPLEX, font_size1,font_Color1, font_weight1);

        cv::imshow("tmp", res_image);
        
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }*/
  delete det;

  return 0;
}