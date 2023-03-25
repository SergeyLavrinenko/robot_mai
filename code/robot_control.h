
#include <memory>
#include "detector.h"
#include "robot.h"
#include "order_manager.h"
#include "drink_control.h"


enum EState {EWait, EMove, ERotate, EForward, EFillingGlass, ESendPush, EGivingGlass};

class Robot_control{
    std::shared_ptr<Detector> det;
    std::shared_ptr<Robot> robot;
    std::shared_ptr<Order_manager> ord_control;
    std::shared_ptr<Drink_control> dr_control;

	EState state;
    
    bool flag_filling_glass = false;

    void processEvents();

public:
    Robot_control(std::shared_ptr<Detector> d, std::shared_ptr<Robot> r, std::shared_ptr<Order_manager> o, std::shared_ptr<Drink_control> dr);
    void run() ;
};