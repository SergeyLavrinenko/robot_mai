#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>
#include "detector.h"

using namespace std;
using namespace cv;



Point Detector::find_rect_centre(Rect r){
    int x = r.x + (r.width / 2);
    int y = r.y + (r.height / 2);

    return Point(x, y);
}
Point Detector::find_centre_beetwen_point(Point p1, Point p2){
    int x = (p1.x + p2.x) / 2;
    int y = (p1.y + p2.y) / 2;

    return Point(x, y);
}
    
int Detector::calc_angle_beetwen_point(Point p_a, Point p_b){
    double gipoten = pow((p_b.x - p_a.x)*(p_b.x - p_a.x) + (p_b.y- p_a.y)*(p_b.y - p_a.y), 2);
    asin(gipoten / (p_b.x - p_a.x));

    double deg = atan2((p_b.x - p_a.x) , (p_b.y - p_a.y)) / M_PI * 180;

    if (deg >= 0)
        return deg;
    else
        return 360 + deg;
}
int Detector::calc_distance_beetwen_point(Point p_a, Point p_b){
    return sqrt((p_b.x - p_a.x)*(p_b.x - p_a.x) + (p_b.y - p_a.y)*(p_b.y - p_a.y));
}

void Detector::update_angle_bot(){
    Point p_a = find_rect_centre(rect_bot_front);
    Point p_b = find_rect_centre(rect_bot_rear);

    this->point_centre_bot = find_centre_beetwen_point(p_a, p_b);
    this->angle_bot = calc_angle_beetwen_point(p_b, p_a);
}

void Detector::update_angle_to_target(){
    Point point_target = find_rect_centre(rect_target);
    int angle_bot_centre_to_target = calc_angle_beetwen_point(this->point_centre_bot, point_target);
    this->angle_target = angle_bot_centre_to_target - this->angle_bot;
}

void Detector::update_distance_to_target(){
    Point point_target = find_rect_centre(rect_target);
    this->distance_target = calc_distance_beetwen_point(this->point_centre_bot, point_target);
}

void Detector::update_angle_to_home(){
    Point point_home = find_rect_centre(rect_home);
    int angle_bot_centre_to_home = calc_angle_beetwen_point(this->point_centre_bot, point_home);
    this->angle_home = angle_bot_centre_to_home - this->angle_bot;
}
void Detector::update_distance_to_home(){
    Point point_home = find_rect_centre(rect_home);
    this->distance_home = calc_distance_beetwen_point(this->point_centre_bot, point_home);
}


Detector::Detector(int id){
    cap.open(id);
    if(!cap.isOpened())
        cout << "Error opening video stream or file" << endl;
    this->update_image();
}

Mat Detector::get_image(){
    return frame_res;
}

void Detector::draw_image(){
    this->draw_rect_bot_front(0, 255, 0);
    this->draw_rect_bot_rear(255, 0 , 0);
    this->draw_rect_target(0, 0, 255);
    this->draw_rect_home(100, 200, 30);

    Mat res_image = this->get_image();

    // До цели
    // ---- Наносим градусы на картинку ----
    Point text_position(20, 30);
    int font_size = 1;
    Scalar font_Color(90, 100, 0);
    int font_weight = 2;
    putText(res_image, std::to_string(this->get_angle_to_target()), text_position,FONT_HERSHEY_COMPLEX, font_size,font_Color, font_weight);
    // ------Наносим расстояние до цели-----
    text_position.x = 20;
    text_position.y = 60;
    putText(res_image, std::to_string(this->get_distance_to_target()), text_position,FONT_HERSHEY_COMPLEX, font_size,font_Color, font_weight);

    // До дома
    // ---- Наносим градусы на картинку ----
    text_position.x = 500;
    text_position.y = 30;
    putText(res_image, std::to_string(this->get_angle_to_home()), text_position,FONT_HERSHEY_COMPLEX, font_size,font_Color, font_weight);
    // ------Наносим расстояние до цели-----
    text_position.x = 500;
    text_position.y = 60;
    putText(res_image, std::to_string(this->get_distance_to_home()), text_position,FONT_HERSHEY_COMPLEX, font_size,font_Color, font_weight);

    cv::imshow("tmp", res_image);
}

void Detector::update_image(){
    cap >> frame;
    frame_res = frame;

    this->rect_bot_front = this->detect_rect(this->color_bot_front);
    this->rect_bot_rear = this->detect_rect(this->color_bot_rear);
    this->rect_target = this->detect_rect(this->color_target);
    this->rect_home = this->detect_rect(this->color_home);

    this->update_angle_bot();
    this->update_angle_to_target();
    this->update_distance_to_target();
    this->update_angle_to_home();
    this->update_distance_to_home();
}

int Detector::get_angle_to_target(){
    return this->angle_target;
}
int Detector::get_angle_bot(){
    return this->angle_bot;
}
int Detector::get_distance_to_target(){
    return this->distance_target;
}
int Detector::get_angle_to_home(){
    return this->angle_home;
}
int Detector::get_distance_to_home(){
    return this->distance_home;
}

bool Detector::has_delta_angle_to_target(int delta){
    if (abs(this->angle_target) > delta)
        return true;
    else
        return false;
}
bool Detector::has_delta_distance_to_target(int delta){
    if (this->distance_target > delta)
        return true;
    else
        return false;
}
bool Detector::has_delta_angle_to_home(int delta){
    if (abs(this->angle_home) > delta)
        return true;
    else
        return false;
}
bool Detector::has_delta_distance_to_home(int delta){
    if (this->distance_home > delta)
        return true;
    else
        return false;
}

void Detector::set_color_bot_front(int h, int s, int v){
    this->color_bot_front.h = h;
    this->color_bot_front.s = s;
    this->color_bot_front.v = v;
}
void Detector::set_color_bot_rear(int h, int s, int v){
    this->color_bot_rear.h = h;
    this->color_bot_rear.s = s;
    this->color_bot_rear.v = v;
}
void Detector::set_color_target(int h, int s, int v){
    this->color_target.h = h;
    this->color_target.s = s;
    this->color_target.v = v;
}
void Detector::set_color_home(int h, int s, int v){
    this->color_home.h = h;
    this->color_home.s = s;
    this->color_home.v = v;
}

void Detector::draw_rect_bot_front(int frame_r, int frame_g, int frame_b){
    cv::rectangle(frame_res, this->rect_bot_front,cv::Scalar(frame_r, frame_g, frame_b),2);
}
void Detector::draw_rect_bot_rear(int frame_r, int frame_g, int frame_b){
    cv::rectangle(frame_res, this->rect_bot_rear,cv::Scalar(frame_r, frame_g, frame_b),2);
}
void Detector::draw_rect_target(int frame_r, int frame_g, int frame_b){
    cv::rectangle(frame_res, this->rect_target,cv::Scalar(frame_r, frame_g, frame_b),2);
}
void Detector::draw_rect_home(int frame_r, int frame_g, int frame_b){
    cv::rectangle(frame_res, this->rect_home,cv::Scalar(frame_r, frame_g, frame_b),2);
}


Rect Detector::detect_rect(color_hsv color){
    vector<cv::Mat> stickers;
    cv::Mat image_hsv;
    std::vector<std::vector<cv::Point>> contours;
    cv::cvtColor(frame, image_hsv, cv::COLOR_BGR2HSV ); 
    cv::Mat tmp_img(frame.size(),CV_8U);
    cv::inRange(image_hsv, cv::Scalar(color.h-6, color.s-25, color.v-35), cv::Scalar(color.h+6, color.s+25, color.v+35), tmp_img);
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
    return max_rect;
}

Detector::~Detector(){
    cap.release();
    destroyAllWindows();
}