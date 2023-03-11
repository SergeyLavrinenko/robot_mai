
#include "detector.h"

using namespace std;
using namespace cv;


int main () {
    Detector det(0);
    det.set_color_bot_front(171, 160, 130);
    det.set_color_bot_rear(110, 220, 157);
    det.set_color_target(5, 5, 5);


    while(1){
 
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

        cv::imshow("tmp", res_image);
        
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }
  
  return 0;
}