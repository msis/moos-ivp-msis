#ifndef IHEADINGCONTROLLER_H
#define IHEADINGCONTROLLER_H

#include <opencv2/core/core.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class iHeadingController
{
public:
    iHeadingController();

    float kangle, DynamicNonlin, fedFwd;
    Mat a,b,c,x,
        ka,kb,kc,kd,kx;

    float heading(float yawSensor,float yawReference);
    float headingHINF(float yaw,float yawReference);
    float compensateNonLin(float angVel);
    float estimateVelocity(float yawCompensated, float motorForce); // kalman
    void updateHINF();
    void fixDelay();
};

#endif // IHEADINGCONTROLLER_H
