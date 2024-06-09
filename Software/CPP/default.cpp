
//touch, ultrasonic, color, gyro
#include <cmath>

void configureAllSensors(){
    sensorType[S1] = sensorEV3_Touch;
    sensorType[S2] = sensorEV3_Ultrasonic;
    sensorType[S3] = sensorEV3_Color;
    sensorType[S4] = sensorEV3_Gyro;
    wait1Msec(50);
    
    SensorMode[S3] = modeEV3Color_Color;
    SensorMode[S4] = modeEV3Gyro_Calibration; 
    wait1Msec(50);

    SensorMode[S4] = modeEV3Gyro_RateAndAngle;
    wait1Msec(50);
}

void goRobot(int motorPower){
    motor[motorA] = motor[motorD] = motorPower;
}

void spinRobot(float angle, int motorPower){
    motorPower = abs(motorPower);
    resetGyro(S4);

    if(angle > 0)
    {
        motor[motorA] = -motorPower;
        motor[motorD] = motorPower;
    }
    else
    {
        motor[motorA] = motorPower;
        motor[motorD] = -motorPower;
    }
    
    while(abs(getGyroDegrees(S4)) < abs(angle)) 
    {}
    goRobot(0);
}