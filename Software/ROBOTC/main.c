void angleAdjust();

void checkFinish(int & numblocksobtained, bool & blockObtained);

void configureAllSensors();

void goRobot(int motorPower);

void grabBlock(int colorInteger, bool & blockObtained);

bool releaseBlock();

void rotateRobot(float angle, int motorPower);

void turnError();

void turnLeft(int currColor, int numblocksobtained);

//constants to set drive and turning speeds
const int MOTPOWER = 15;
const int MOTSPINPOWER = 10;

//constants to set number of blocks in maze
const int NUMBLOCKS = 1;

const int PIECE_SIDELENGTH = 5;

int colorInteger = 0;

float timetofinish = 0;

bool blockObtained = true;

task main(){
    configureAllSensors();

    displayString(2, "Maze Runner");
    displayString(3, "Made by JC, ZB, EM, and ES");
    displayString(5, "Press Enter to start/pause");
    while (!(getButtonPress(buttonEnter)))
    {}
    while (getButtonPress(buttonEnter))
    {}

    displayString(3, "                               ");
    displayString(5, "                               ");
    displayString(3, "Press Enter to start/pause");

    time1[T1] = 0;


    int numblocksobtained = 0;
    //version 1.1 of maze solving: (turn left algorithm which includes intersection lights)
    //version 1.1 of block acquiring - set blockobtained to false to enable blocks. includes multiblocking
    while (numblocksobtained < NUMBLOCKS)
    {
        goRobot(MOTPOWER);

        while (SensorValue[S3] == (int)colorBlack)
        {
            if(SensorValue[S1] == 1)
            {
                goRobot(0);
                grabBlock(colorInteger, blockObtained);
                goRobot(MOTPOWER);
            }
            else if(getButtonPress(buttonEnter))
            {
                while (getButtonPress(buttonEnter))
                {}
                goRobot(0);
                angleAdjust(); //incomplete

                while (!(getButtonPress(buttonEnter)))
                {}
                while (getButtonPress(buttonEnter))
                {}
                goRobot(MOTPOWER);
            }

        }
        goRobot(0);

        if (SensorValue[S3] == (int)colorGreen)
        {
            checkFinish(numblocksobtained, blockObtained);
            releaseBlock();
            blockObtained = false;
        }
        else if (SensorValue[S3] == (int)colorRed)
        {
            turnLeft((int)colorRed, numblocksobtained);
        }
        else
      	{
            turnError();
    		}
  	}
    displayString(11, "Maze Solved!");
    displayString(13, "Time: %f s", timetofinish);
    wait1Msec(10000);
}

/*
objectives:
1. To map maze and keep track of everything
2. To include block sorting at the finish line - void blockSort with two color sensors
3. To include multiple blocks in a maze
4. To include maze solving efficiency - A* or D ...
*/

void angleAdjust()
{
}

void checkFinish(int & numblocksobtained, bool & blockObtained)
{
    if(blockObtained)
    {
        numblocksobtained++;
         //include block sorting here. reset the time1[T1] after
        timetofinish += time1[T1];
    }
    else
        turnLeft((int)colorGreen, numblocksobtained);
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

    displayString(7, "                                       ");
    displayString(9, "                                       ");
    displayString(7, "Time elapsed %f", time1[T1]/1000.0);
    displayString(9, "Current gyro angle %f", getGyroDegrees(S4));
}

void grabBlock(int & colorInteger, bool & blockObtained)
{
    colorInteger = SensorValue[S3];

    nMotorEncoder[motorB] = 0;

    motor[motorC] = 20;     //*dont know if + or - power closes/opens jaws or raise/lowers hinge, subject to change
    wait1Msec(5000); //*jaws will keep power on to continiously close to hold onto object for entire duration

    motor[motorB] = 20;
    while(nMotorEncoder[motorB] <= 60)
    {}
    motor[motorB] = 0;
    blockObtained = true;
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
void turnLeft(int currColor, int numblocksobtained)
{
		nMotorEncoder[motorA] = 0;
		goRobot(MOTPOWER);
		while(nMotorEncoder[motorA] < (PIECE_SIDELENGTH/2.0) * (360.0 / (2 * PI * 2.75)))
		{}
		goRobot(0);
    do
    {
    		bool firstTurn = true;
    		if(firstTurn)
    		{
        	rotateRobot(90, MOTSPINPOWER);
        	firstTurn = false;
      	}
      	else
      		rotateRobot(-90, MOTSPINPOWER);

        nMotorEncoder[motorA] = 0;
        goRobot(MOTPOWER); // go until new colour - check if path is available
        while(SensorValue[S3] == currColor) // could drive to a given distance anstead
        {}
        goRobot(0);

        if (SensorValue[S3] == (int)colorGreen) //is this correct?
        {
            checkFinish(numblocksobtained, blockObtained);
            releaseBlock();
            blockObtained = false;
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
