#include "iheadingcontroller.h"

iHeadingController::iHeadingController()
{
    DynamicNonlin = 0.35;
    fedFwd = 1.;
    float Adata[5][5] = {
        {0.197256482648792, 1.59540314598849, 4.49798010253664, -3.71776829599392, 0.300123924813506},
        {-0.00621131141495130, 0.759731533313859, -0.685176910215340, 0.547387817981330, -0.0450683664607837},
        {0.0146347815053511, -0.170901816165014, 0.550453855157298, 0.0406100731570936, -0.0155990464674315},
        {-0.00153580351247490, -0.0362653741260259, -0.0211178973384089, 0.305283481543664, 0.0296900874424064},
        {0.000293001798299378, 0.000255776045928881, 0.000246755112570867, 0.0276890693987905, 0.998376332616215},
    };
    float Bdata[5][1] = {
        {0.273103219717465},
        {-0.0179292130016475},
        {0.206521330295216},
        {0.110601840954218},
        {0.000521323997792441},
    };
    float Cdata[1][5] = {
        {-16.0062512802928, 58.3996253615540, 178.658222489516, -258.946581201241, 16.8045790623509},
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


    //#kalman iteration
    //#    a=
    //#    0.817434207010430        -0.0270295049541631
    //#    0.0905652303625428    0.917467766665371
    //#    b=
    //#    0.182565792989570         0.0270295049541631
    //#    0.00943476963745717    0.0825322333346289
    //#
    //#
    //#    c=
    //#    0    0.920462421552291
    //#    1    -0.0330662758205544
    //#    0    0.920462421552291
    //#
    //#    d=
    //#    0    0.0795375784477090
    //#    0    0.0330662758205544
    //#    0    0.0795375784477090


    float KAdata[2][2] = {
        {0.817434207010430, -0.0270295049541631},
        {0.0905652303625428,    0.917467766665371},
    };
    float KBdata[2][2] = {
        {0.182565792989570,     0.0270295049541631},
        {0.00943476963745717,   0.0825322333346289},
    };
    float KCdata[3][2] = {
        {0,    0.920462421552291},
        {1,    -0.0330662758205544},
        {0,    0.920462421552291}
    };
    float KDdata[3][2] = {
        {0,    0.0795375784477090},
        {0,    0.0330662758205544},
        {0,    0.0795375784477090}
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
//    cout << motorForce << endl
//         << yawVel << endl
//         << compensateNumber << endl;
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
