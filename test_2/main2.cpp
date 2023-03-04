#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>

using namespace std;
using namespace cv;



class Detected{
    Mat frame;
    Mat frame_res;

    VideoCapture cap;

    static Point find_rect_centre(Rect r){
        int x = r.x + (r.width / 2);
        int y = r.y + (r.height / 2);

        return Point(x, y);
    }
    
    public:

    Detected(int id){
        cap.open(id);

        if(!cap.isOpened())
        cout << "Error opening video stream or file" << endl;

        this->update_image();
    
    }

    Mat get_image(){
        return frame_res;
    }

    void update_image(){
        cap >> frame;
        frame_res = frame;
    }

    int calc_deg(Rect a, Rect b){
        Point p_a = find_rect_centre(a);
        Point p_b = find_rect_centre(b);

        double gipoten = pow((p_b.x - p_a.x)*(p_b.x - p_a.x) + (p_b.y- p_a.y)*(p_b.y - p_a.y), 2);
        asin(gipoten / (p_b.x - p_a.x));

        double deg = atan2((p_b.x - p_a.x) , (p_b.y - p_a.y)) / M_PI * 180;

        if (deg >= 0)
            return deg;
        else
            return 360 + deg;

    }

    Rect detect_rect(int h, int s, int v, int frame_r, int frame_g, int frame_b){
        vector<cv::Mat> stickers;
        cv::Mat image_hsv;
        std::vector<std::vector<cv::Point>> contours;
        cv::cvtColor(frame, image_hsv, cv::COLOR_BGR2HSV ); 
        cv::Mat tmp_img(frame.size(),CV_8U);
        cv::inRange(image_hsv, cv::Scalar(h-6, s-25, v-35), cv::Scalar(h+6, s+25, v+35), tmp_img);
        // "Замазать" огрехи в при выделении по цвету
        cv::dilate(tmp_img,tmp_img,cv::Mat(),cv::Point(-1,-1),3);
        cv::erode(tmp_img,tmp_img,cv::Mat(),cv::Point(-1,-1),1);
        //Выделение непрерывных областей
        cv::findContours(tmp_img,contours, 0, 1);

        cv::Rect max_rect(0, 0, 0, 0);
        for (uint i = 0; i<contours.size(); i++) { cv::Mat sticker;
            //Для каждой области определяем ограничивающий прямоугольник
            cv::Rect rect=cv::boundingRect(contours[i]);

            //Ищем из всех самый большой прямоугольник
            if (rect.height * rect.width > max_rect.height * max_rect.width){
                max_rect = rect;
            }
            
        }
        cv::rectangle(frame_res,max_rect,cv::Scalar(frame_r, frame_g, frame_b),2); // Добавляем прямоугольник на изображение
        return max_rect;
    }

    ~Detected(){
        cap.release();
        destroyAllWindows();
    }
};

int main () {
    Detected det(0);


    while(1){
 
        det.update_image();
        Rect rect_red = det.detect_rect(171, 160, 130, 255, 0, 0);
        Rect rect_blue = det.detect_rect(110, 220, 157, 0, 0, 255);

        Mat res_image = det.get_image();

        // ---- Наносим градусы на картинку ----
        Point text_position(20, 30);
        int font_size = 1;
        Scalar font_Color(90, 100, 0);
        int font_weight = 2;
        putText(res_image, to_string(det.calc_deg(rect_red, rect_blue)), text_position,FONT_HERSHEY_COMPLEX, font_size,font_Color, font_weight);
        // -------------------------------------

        cv::imshow("tmp", res_image);
        
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }
  
  return 0;
}
 
