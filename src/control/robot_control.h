
#include <memory>
#include "detector.h"
#include "robot.h"
#include "order_manager.h"
#include "drink_control.h"


enum EState {EWait, EMove ,EMoveToTarget, EMoveToHome, ERotateLeft, ERotateRight, EForward, EFillingGlass, ESendPush, EGivingGlass, EMoveToDrink};
enum EMove_state{toHome, toTarget, toDrink};

class Robot_control{
    std::shared_ptr<Detector> det_main;
    std::shared_ptr<Detector> det_drink;
    std::shared_ptr<Robot> robot;
    std::shared_ptr<Order_manager> ord_control;
    std::shared_ptr<Drink_control> dr_control;

	EState state;
    EMove_state move_state;
    
    bool flag_filling_glass = false;
    bool flag_move_to_home = false;
    bool flag_start = false;

    float time_forward;

    void processEvents();

public:
    Robot_control(std::shared_ptr<Detector> d_m, std::shared_ptr<Detector> d_d, std::shared_ptr<Robot> r, std::shared_ptr<Order_manager> o, std::shared_ptr<Drink_control> dr);
    void run() ;
};