#pragma once
#include <opencv2/opencv.hpp>


using namespace cv;

struct color_hsv{
    int h;
    int s;
    int v;
    color_hsv(int _h, int _s, int _v): h(_h), s(_s), v(_v) {}
};

class Detector{
    Mat frame;
    Mat frame_res;
    VideoCapture cap;

    color_hsv color_bot_front = color_hsv(0, 0 ,0);
    color_hsv color_bot_rear = color_hsv(0, 0, 0);
    color_hsv color_target = color_hsv(0, 0 ,0);
    color_hsv color_home = color_hsv(0, 0 ,0);

    Rect rect_bot_front;
    Rect rect_bot_rear;
    Rect rect_target;
    Rect rect_home;

    Point point_centre_bot;

    int angle_bot;
    int angle_target;
    int distance_target;
    int angle_home;
    int distance_home;

    bool target_is_point; // Цель задана координатами
    Point point_target;

    static Point find_rect_centre(Rect r);
    static Point find_centre_beetwen_point(Point p1, Point p2);
    static int calc_angle_beetwen_point(Point p1, Point p2);
    static int calc_distance_beetwen_point(Point p_a, Point p_b);

    void update_angle_bot();
    void update_angle_to_target();
    void update_distance_to_target();
    void update_angle_to_home();
    void update_distance_to_home();

    Rect detect_rect(color_hsv color);

    public:

    Detector(int id, bool targetIsPoint = false);
    Detector(Detector &d);
    Detector& operator=(Detector &d);

    void set_color_bot_front(int h, int s, int v);
    void set_color_bot_rear(int h, int s, int v);
    void set_color_target(int h, int s, int v);
    void set_color_home(int h, int s, int v);

    void set_point_target(int x, int y);
    void set_target_is_point(bool flag);

    Mat get_image();
    void update_image();
    void draw_image();

    int get_angle_bot();
    int get_angle_to_target();
    int get_distance_to_target();
    int get_angle_to_home();
    int get_distance_to_home();

    bool has_delta_angle_to_target(int delta);
    bool has_delta_distance_to_target(int delta);
    bool has_delta_angle_to_home(int delta);
    bool has_delta_distance_to_home(int delta);


    void draw_rect_bot_front(int frame_r, int frame_g, int frame_b);
    void draw_rect_bot_rear(int frame_r, int frame_g, int frame_b);
    void draw_rect_target(int frame_r, int frame_g, int frame_b);
    void draw_rect_home(int frame_r, int frame_g, int frame_b);
    void draw_circle_target(int radius);


    ~Detector();
};