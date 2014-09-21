#include "iheadingcontroller.h"

iHeadingController::iHeadingController()
{
    DynamicNonlin = 0.35;
    fedFwd = 1.;
    
    
    
    float Adata[5][5] = {
        {0.2924, 1.3326, -4.3930, 4.0052, 0.3143},
        {-0.0117, 0.8004, 0.6681, -0.5623, -0.0458},
        {-0.0147, 0.1766, 0.4482, 0.1266, 0.0220},
        {0.0025, 0.0342, -0.0362, 0.3119, -0.0311},
        {0.0004, -0.0000, 0.0002, -0.0303, 0.9982}
    };
    float Bdata[5][1] = {
        {0.2494},
        {-0.0311},
        {-0.2058},
        {-0.1034},
        {0.0006},
    };
    float Cdata[1][5] = {
        {-14.6789, 49.6981, -176.3590, 260.3531, 17.2943}
    };

    float Xdata[5][1] = {{0.},{0.},{0.},{0.},{0.}};

    Mat Am(Size(5,5),CV_32FC1,Adata);
    Mat Bm(Size(1,5),CV_32FC1,Bdata);
    Mat Cm(Size(5,1),CV_32FC1,Cdata);
    Mat Xm(Size(1,5),CV_32FC1,Xdata);
    Am.copyTo(a);
    Bm.copyTo(b);
    Cm.copyTo(c);
    Xm.copyTo(x);

    float KAdata[2][2] = {
        {0.8861, -0.0247},
        {0.0942, 0.9242},
    };
    float KBdata[2][2] = {
        {0.1139, 0.0247},
        {0.0058, 0.0758},
    };
    float KCdata[3][2] = {
        {0, 0.9268},
        {1., -0.0279},
        {0, 0.9268}
    };
    float KDdata[3][2] = {
        {0, 0.0732},
        {0, 0.0279},
        {0, 0.0732}
    };

    float KXdata[2][1] = {{0.},{0.}};

    Mat KAm(Size(2,2),CV_32FC1,KAdata);
    Mat KBm(Size(2,2),CV_32FC1,KBdata);
    Mat KCm(Size(2,3),CV_32FC1,KCdata);
    Mat KDm(Size(2,3),CV_32FC1,KDdata);
    Mat KXm(Size(1,2),CV_32FC1,KXdata);
    KAm.copyTo(ka);
    KBm.copyTo(kb);
    KCm.copyTo(kc);
    KDm.copyTo(kd);
    KXm.copyTo(kx);
}

float iHeadingController::heading(float yawSensor,float yawReference)
{
    float motorForce = headingHINF(yawSensor,yawReference);
    float yawVel = estimateVelocity(yawSensor,motorForce);
    float compensateNumber = compensateNonLin(kangle);
    return compensateNumber;
}

float iHeadingController::headingHINF(float yaw,float yawReference)
{
    float errYaw = yawReference - yaw;
    x = a*x + b*errYaw;
    Mat y = c*x;

    float ret = y.at<float>(Point(0,0));
    float motolimit = 0; //5.8

    //ret = ret > motolimit? ret: motolimit;
    //ret = ret > motolimit? ret: -motolimit;
    return ret;
}

float iHeadingController::estimateVelocity(float yawCompensated, float motorForce)
{
    float UData[2][1] = {{motorForce},{yawCompensated}};
    Mat u(Size(1,2),CV_32FC1,UData);

    kx = ka*kx + kb*u;
    Mat y = kc*kx + kd*u;
    float angleVel = y.at<float>(Point(0,1));
    kangle = y.at<float>(Point(0,0));
    return angleVel;
}


float iHeadingController::compensateNonLin(float angVel)
{
    return fedFwd*angVel - DynamicNonlin*abs(angVel)*angVel;
}

void iHeadingController::updateHINF()
{
    //send to the moos
}
