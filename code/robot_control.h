
#include "detector.h"
#include "robot.h"
#include "order_manager.h"
#include "drink_control.h"

enum EState {EWait, EMove, ERotate, EForward, EFillingGlass, ESendPush, EGivingGlass};

class Robot_control{
    Detector *det;
	EState state;
	Robot *robot;
    Drink_control *drink;
    Order_manager *ord;

    bool flag_filling_glass = false;

    void processEvents();

public:
    Robot_control(Detector* d, Robot* r, Order_manager* o, Drink_control *dr);
    void run() ;
};