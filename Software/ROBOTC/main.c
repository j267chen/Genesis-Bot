void checkFinish();

void configureAllSensors();

void goRobot(int motorPower);

bool grabBlock();

bool releaseBlock();

void rotateRobot(float angle, int motorPower);

void turnError();

void turnLeft();

task main(){
    configureAllSensors();

    displayString(2, "Maze Runner");
    displayString(3, "Made by Jason Chen, Zachary Barandino, Eric Mak, and Eric Shaw");
    displayString(5, "Press Enter to start");
    while (!(getButtonPress(buttonEnter)))
    {}
    while (getButtonPress(buttonEnter))
    {}

    //constants to set drive and turning speeds
    const int MOTPOWER = 15;
    const int MOTSPINPOWER = 10;

    //constants to set number of blocks in maze
    const int NUMBLOCKS = 1;

    float timetofinish = 0;
    time1[T1] = 0;

    displayString(7, "Time elapsed %f", time1[T1]/1000.0);

    //version 1.1 of maze solving: (turn left algorithm which includes intersection lights)
    //version 1.1 of block acquiring - set blockobtained to false to enable blocks. includes multiblocking

    int numblocksobtained = 0;
    bool blockobtained = true; //will be replaced with the return of bool function grabBlock
    while (numblocksobtained < NUMBLOCKS)
    {
        goRobot(MOTPOWER);
        while (SensorValue[S3] == (int)colorBlack)
        {}
        goRobot(0);

        if (SensorValue[S3] == (int)colorGreen)
        {
            checkFinish();
        }
        else if (SensorValue[S3] == (int)colorRed)
        {
            turnLeft((int)colorRed);
        }
        else
      	{
            turnError();
    	}
  	}
    displayString(9, "Maze Solved!");
    displayString(11, "Time: %f s", timetofinish);
    wait1Msec(10000);
}

/*
objectives:
1. To detect and grab blocks - bool grabBlock
    the bool is necessary to understand if block is obtained. so the value of the funtion will be read once finish is detected.
2. To release blocks - bool releaseBlock
3. To map maze and keep track of everything=
4. To include block sorting at the finish line - void blockSort with two color sensors
5. To include multiple blocks in a maze
6. To include maze solving efficiency - A* or D ...
*/

void checkFinish()
{
    if(blockobtained)
    {
        numblocksobtained += 1;
         //include block sorting here. reset the time1[T1] after
        timetofinish += time1[T1];
    }
    else
        turnLeft((int)colorGreen);
}

void configureAllSensors(){
    SensorType[S1] = sensorEV3_Touch;
    SensorType[S2] = sensorEV3_Ultrasonic;
    SensorType[S3] = sensorEV3_Color;
    SensorType[S4] = sensorEV3_Gyro;
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

//turn left algorithm at turns or ends of paths
//TO DO: have it move a certain mimimum distance
void turnLeft(int currColor)
{
    do
    {
        rotateRobot(90, MOTSPINPOWER);
        nMotorEncoder[motorA] = 0;
        goRobot(10); // go until new colour - check if path is available
        while(SensorValue[S3] == currColor)
        {}
        goRobot(0);

        if (SensorValue[S3] == (int)colorGreen) //is this correct?
        {
            checkFinish();
        }
        else if (SensorValue[S3] != (int)colorBlack) //go back
        {
            goRobot(-MOTPOWER);
            while(nMotorEncoder[motorA] > 0)
            {}
            goRobot(0);
        }
        else
            turnError();
    } while (SensorValue[S3] == currColor); 
    //SensorValue[S3] != (int)colorBlack || SensorValue[S3] != (int)colorGreen
    //does the turn left algorithm until it detects new colours
    //but if we see a colour other than red, green, or black, what happens?
}

//accounts for turning error in both directions
void turnError()
{
    for(int i = 1; i < 6; i++)
    {
        if (i % 2 == 0)
        {
            rotateRobot(5 * i, MOTSPINPOWER);
            if (SensorValue[S3] == (int)colorBlack)
                i += 10;
        }
        else 
        {
            rotateRobot(-(5 * i), MOTSPINPOWER); 
            if (SensorValue[S3] == (int)colorBlack)
                i += 10;
        }
    }
}
