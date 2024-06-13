//all code
//#include "EV3_API.h"

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

void rotateRobot(float angle, int motorPower){
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

/*
void turnLeft()

bool checkFinish()
{
    if (SensorValue[S3] == (int)colorGreen)
    {
        unsolved = false;
        timetofinish = time1[T1];
        // is unsolved in scope?
    }
}
*/

task main(){
    configureAllSensors();

    displayString(2, "Maze Runner")
    displayString(3, "Made by Jason Chen, Zachary Barandino, Eric Mak, and Eric Shaw")
    displayString(5, "Press Enter to start");
    while (!(getButtonPress(ButtonEnter)))
    {}
    while (getButtonPress(ButtonEnter))
    {}

    float timetofinish = 0;
    time1[T1] = 0;

    //version 1.1 of maze solving: (turn left algorithm which includes intersection lights)
    bool unsolved = true;
    while (unsolved)
    {
        goRobot(50);
        while (SensorValue[S3] == (int)colorBlack)
        {}
        goRobot(0);

        //check for finish
        if (SensorValue[S3] == (int)colorGreen)
        {
            unsolved = false;
            timetofinish = time1[T1];
        }
        //turn left algorithm at turns or ends of paths
        else if (SensorValue[S3] == (int)colorRed)
        {
            do
            {
                rotateRobot(90, 25);
                nmotorEncoder[motorA] = 0;
                goRobot(10); // go until new colour - check if path is available
                while(SensorValue[S3] == (int)colorRed)
                {}
                goRobot(0);

                if (SensorValue[S3] == (int)colorGreen)
                {
                    unsolved = false;
                    timetofinish = time1[T1];
                }
                else if (SensorValue[S3] != (int)colorBlack) //go back
                {
                    goRobot(-10);
                    while(nMotorEncoder[motorA] > 0)
                    {}
                    goRobot(0);
                }
            } while (SensorValue[S3] != (int)colorBlack || SensorValue[S3] != (int)colorGreen); //does the turn left algorithm until it detects black or green
        }
        else //accounts for turning error in both directions
            for(int i = 1; i < 9; i++)
            {
                if (i % 2 == 0)
                {
                    rotateRobot(5 * i, 25);
                    if (SensorValue[S3] != (int)colorBlack)
                        i += 10;
                }
                else 
                {
                    rotateRobot(-(5 * i), 25); 
                    if (SensorValue[S3] != (int)colorBlack)
                        i += 10;
                }
            }
    }
    /*
    need to introduce a secondary objective: pick up block and sort it
    
    could introduce a way to recognize turns and adjust accordingly
    could introduce A* algorithm to solve maze
    */
    displayString(5, "Maze Solved!");
    displayString(6, "Time: %f s", timetofinish);
    wait1Msec(10000);

    return 0;
}
