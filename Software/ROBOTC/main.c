void checkFinish();

void configureAllSensors();

void goRobot(int motorPower);

bool grabBlock();

bool releaseBlock();

void blockSort(int colours[NUM_COLOURS]);

void rotateRobot(float angle, int motorPower);

void turnError();

void turnLeft();

task main() {
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

<<<<<<< Updated upstream
    const int NUM_COLOURS = 3;
=======
    //constants to set number of blocks in maze
    const int NUMBLOCKS = 3;
>>>>>>> Stashed changes

    float timetofinish = 0;
    time1[T1] = 0;  

    displayString(7, "Time elapsed %f", time1[T1]/1000.0);

    //version 1.1 of maze solving: (turn left algorithm which includes intersection lights)
    //version 1.0 of block acquiring - set blockobtained to false to enable blocks
    bool unsolved = true, blockobtained = true;
    while (unsolved)
    {
        goRobot(MOTPOWER);
        while (SensorValue[S3] == (int)colorBlack)
        {}
        goRobot(0);

        if (SensorValue[S3] == (int)colorGreen)
        {
            //can integrate multi block aquiring
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
2. To release blocks - bool releaseBlock
3. To map maze and keep track of everything
4. To include multiple blocks in a maze 
5. To include block sorting at the finish line
6. To include maze solving efficiency - A* or D ...
*/

void checkFinish()
{
    if(blockobtained)
    {
        unsolved = false;
        timetofinish = time1[T1];
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

<<<<<<< Updated upstream
=======
void grabBlock(int & colorInteger)
{
    colorInteger = SensorValue[S3];

    nMotorEncoder[motorB] = 0;

    motor[motorC] = 20;     //*dont know if + or - power closes/opens jaws or raise/lowers hinge, subject to change
    wait1Msec(5000); //*jaws will keep power on to continiously close to hold onto object for entire duration

    motor[motorB] = 20;
    while(nMotorEncoder[motorB] <= 60)
    {}
    motor[motorB] = 0;
}

bool releaseBlock()
{
    motor[motorC] = -20;
    wait1Msec(5000);
    motor[motorC] = 0;

    motor[motorB] = -20;
    while(nMotorEncoder[motorB] >= 0)
    {}
    motor[motorB] = 0;

    return true;
}

void returnGreen()

void sortBlock(int colourInteger, int colorList[NUMBLOCKS]) //if i were to change to a more dynamic sorting, addition of slotFull array would be added to parameters
{
    int colourList = [(int)colorRed, (int)colorGreen, (int)colorBlue];

    int xTravelLim = 10 * 180/(2 * 2.75); //converting the max horizontal dist between each block to revolutions
    int yTravelLim = 10 * 180/(2 * 2.75); // converting the max vertical dist between green to sorting slots to revolutions 

    for(int i = 0; i < NUMBLOCKS; i++) //iterate through list
    {
        if (colorList[i] == colorInteger) //find that a colour has been matched
        {
            if(i == 1) //check red
            {
                nMotorEncoder(motorA) = 0;

                rotateRobot(-90, 20); // ccw rotation
                wait1Msec(100); // wait
                goRobot(20); // move forward

                while(nMotorEncoder(motorA) <= xTravelLim) //
                {}

                nMotorEncoder(motorD) = 0;
                rotateRobot(90, 20);
                goRobot(10)
                while(nMotorEncoder(motorD) < yTravelLim)
                {}

                goRobot(0);
                releaseBlock();
            }
            else if(i == 2) //check green
            {
                nMotorEncoder(motorD) = 0;
                rotateRobot(90, 20);
                goRobot(10)
                while(nMotorEncoder(motorD) < yTravelLim)
                {}

                goRobot(0);
                releaseBlock();
            }
            else //check blue           
                rotateRobot(90, 20);
                wait1Msec(100);
                goRobot(20);

                while(nMotorEncoder(motorA) <= xTravelLim)
                {}

                nMotorEncoder(motorD) = 0;
                rotateRobot(-90, 20);
                goRobot(10);
                while(nMotorEncoder(motorD) < yTravelLim)
                {}

                goRobot(0);
                releaseBlock();
        }
    }
}

>>>>>>> Stashed changes
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

void blockSort(int colour)
{
    int colour 

    // move bot left x amount
    //turn cc go straight, scan first colour
    // go back same distance
    // turn cc move to next colour tile
    // repeat previous motions
    // do until scan colour of var
    // once colour of block is same as tile, move back enough for gripper to place
}