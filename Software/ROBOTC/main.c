#include "PC_FileIO.c"
#define MAZE_SIZE 6
#define MAX_STACK_SIZE 70
// Mapping vars
short maze[MAZE_SIZE * MAZE_SIZE]; // ROW * MAZE_SIZE + COL to collape a 2d array into 1d
bool isVisited[MAZE_SIZE * MAZE_SIZE];
short moves[MAX_STACK_SIZE];
short path[MAX_STACK_SIZE];
short currRow = 0, currCol = 0, direction = 2; // Start at top left (0,0); direction (N,0)(E,1)(S,2)(W,3)
// Order: WSEN
const short dRow[] = { 0, 1, 0, -1 };
const short dCol[] = { 1, 0, -1, 0 };

// Stack
typedef struct {
  short elements[MAX_STACK_SIZE]; // array of elements
  short size; // current size of the stack
} mStack;
bool push(mStack *stack, short value);
short pop(mStack *stack);
bool isEmpty(mStack *stack);
bool isFull(mStack *stack);
// DFS
bool isValid(short row, short col);
void dfs(short row, short col);
void goFinish();
// Mapping
short getRow(short data);
short getCol(short data);
short getDirection(short curr, short next);
void addNode(int color);
// Robot Movement
void angleAdjust();
void configureAllSensors();
void goRobot(int motorPower);
void grabBlock();
bool releaseBlock();
void sortBlock(int colourInteger);
void rotateRobot(float angle, int motorPower);
void turnError();
void turnLeft(int currColor);

//constants to set drive and turning speeds
const int MOTPOWER = 35;
const int MOTSPINPOWER = 25;
//constants for mapping and navigation
const int NUMBLOCKS = 3;
const float PIECE_SIDELENGTH = 15;
int colorInteger = 0;
float timetofinish = 0;
bool blockObtained = false, foundFinish = false, afterTurn = false;
int numblocksobtained = 0;
const int colorBlocks[3] = {(int)colorRed, (int)colorGreen, (int)colorBlue};
const int PATHCOLOR = (int)colorBlue;
const float ENC_CONV = 180 / (PI * 2.75);

TFileHandle fout;

task main(){
    configureAllSensors();

    // Initialize mapping
    for (short row = 0; row < MAZE_SIZE; row++) {
            for (short col = 0; col < MAZE_SIZE; col++) {
                maze[row * MAZE_SIZE + col] = -1;
            }
    }
    for (short i = 0; i < MAX_STACK_SIZE; i++) {
            path[i] = -1;
    }
    maze[0] = 2;

		// File
		bool fileOkay = openWritePC(fout, "fileWrite.txt");

		// Program start
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
		displayString(11, "%hd", direction); // debugging

    //version 1.1 of maze solving: (turn left algorithm which includes intersection lights)
    //version 1.1 of block acquiring - set blockobtained to false to enable blocks. includes multiblocking
    while (numblocksobtained < NUMBLOCKS)
    {
    		displayString(11, "%hd", direction);

    		if(afterTurn) {
    				addNode(PATHCOLOR);
    				/*
    				while (!(getButtonPress(buttonEnter))) {}
				    while (getButtonPress(buttonEnter)) {}
				    */
				    afterTurn = false;
    		}

    		nMotorEncoder[motorA] = 0;
        goRobot(MOTPOWER);

				while(nMotorEncoder[motorA] < PIECE_SIDELENGTH * ENC_CONV) {
						if(SensorValue[S1] != 0) {
                goRobot(0);
                grabBlock();
                if (foundFinish)
                		goFinish();
                goRobot(MOTPOWER);
            }
            if(getButtonPress(buttonAny)) {
            		angleAdjust();
          	}
				}

        if (SensorValue[S3] == PATHCOLOR)
        {
        		goRobot(0);
						addNode(PATHCOLOR);
						/*
						resetGyro(S4);
						while (!(getButtonPress(buttonEnter))) {}
				    while (getButtonPress(buttonEnter)) {}
				    */

						nMotorEncoder[motorA] = 0;
						goRobot(MOTPOWER);
						afterTurn = false;
				}
        else if (SensorValue[S3] == (int)colorGreen)
        {
        		if(!foundFinish) // Adding node for first occurence
        				addNode((int)colorGreen);
        		foundFinish = true;

        		displayString(10, "%hd", numblocksobtained); //debugging

        		if(blockObtained) { // sort
        				sortBlock(SensorValue[S2]);
        				timetofinish += time1[T1];
        		}
        		else { // go back
        				turnLeft((int)colorGreen);
        		}
        }
        else if (SensorValue[S3] == (int)colorRed)
        {
            addNode((int)colorRed);
        		turnLeft((int)colorRed);
        }
        else
      	{
            turnError();
      	}
  	}
    displayString(11, "Maze Solved!");
    displayString(13, "Time: %f s", timetofinish);
    wait1Msec(10000);
    closeFilePC(fout);
}

/*
objectives:
1. To map maze and keep track of everything
2. To include block sorting at the finish line - void blockSort with two color sensor
3. To include maze solving efficiency - A* or D ...
*/

void angleAdjust()
{
    goRobot(0);
    while (getButtonPress(buttonAny))
    {}

    while (!(getButtonPress(buttonAny)))
    {}
    while (getButtonPress(buttonAny))
    {}
    goRobot(MOTPOWER);
}

void configureAllSensors(){
    SensorType[S1] = sensorEV3_Touch;
    wait1Msec(50);
    SensorType[S2] = sensorEV3_Color;
    wait1Msec(50);
    SensorType[S3] = sensorEV3_Color;
    wait1Msec(50);
    SensorType[S4] = sensorEV3_Gyro;
    wait1Msec(50);

    SensorMode[S2] = modeEV3Color_Color;
    wait1Msec(50);
    SensorMode[S3] = modeEV3Color_Color;
    wait1Msec(50);
    SensorMode[S4] = modeEV3Gyro_Calibration;
    wait1Msec(50);

    SensorMode[S4] = modeEV3Gyro_RateAndAngle;
    wait1Msec(50);
}

void goRobot(int motorPower) {
    motor[motorA] = motor[motorD] = motorPower;

    displayString(7, "                                       ");
    displayString(9, "                                       ");
    displayString(7, "Time elapsed %f", time1[T1]/1000.0);
    displayString(9, "Current gyro angle %f", getGyroDegrees(S4));
}

void grabBlock()
{
    numblocksobtained++;
    blockObtained = true;

    motor[motorB] = -100;
		wait1Msec(1500);
    motor[motorC] = 20;
    wait1Msec(1500);
}

bool releaseBlock()
{
    motor[motorC] = -10;
    wait1Msec(1000);
    motor[motorC] = 0;
    motor[motorB] = 100;
    wait1Msec(2000);
    motor[motorB] = 0;

    blockObtained = false;
    return true; // needed?
}

void sortBlock(int colourInteger) //if i were to change to a more dynamic sorting, addition of slotFull array would be added to parameters
{
	 	//Position to middle of tile
    nMotorEncoder[motorA] = 0;
    goRobot(MOTPOWER);
    while(nMotorEncoder[motorA] <= PIECE_SIDELENGTH * (3/2)) {}
    goRobot(0);
    nMotorEncoder[motorA] = 0;

    // Turn if needed
    if(colorInteger == colorBlocks[0]) { //check red, left
		    rotateRobot(-90, MOTSPINPOWER);
		} else if (colorInteger == colorBlocks[2]) { //check blue, right
	      rotateRobot(90, MOTSPINPOWER);
    }

    // Go forward, drop, go back
    goRobot(MOTPOWER);
    while(nMotorEncoder[motorA] <= PIECE_SIDELENGTH * ENC_CONV)
    {}
    releaseBlock();
    goRobot(-MOTPOWER);
    while(nMotorEncoder[motorA] > 0)
  	{}
 		goRobot(0);
 		nMotorEncoder[motorA] = 0;

 		// Back to green
 		if(colorInteger == colorBlocks[0]) { //check red, left
		    rotateRobot(-90, MOTSPINPOWER);
		} else if (colorInteger == colorBlocks[1]) { //check green, straight
	      rotateRobot(-90, MOTSPINPOWER);
	      rotateRobot(-90, MOTSPINPOWER);
    } else { // check blue, right
    		rotateRobot(90, MOTSPINPOWER);
  	}
  	goRobot(MOTPOWER);
  	while(nMotorEncoder[motorA] < PIECE_SIDELENGTH / 2.1 * ENC_CONV)
  	{}
 		goRobot(0);
}

void rotateRobot(float angle, int motorPower) {
    motorPower = abs(motorPower);
    resetGyro(S4);

    if(angle >= 0) //counter-clockwise
    {
    	  motor[motorA] = -motorPower;
        motor[motorD] = motorPower;
    }
    else
    {
        motor[motorA] = motorPower;
        motor[motorD] = -motorPower;
    }

    while(abs(getGyroDegrees(S4)) < abs(angle)) {}
    goRobot(0);
    // Update direction
    if(angle == -90) { // clockwise
    		switch(direction) {
    			case 3:
    					direction = 0;
    					break;
    			default:
    					direction++;
    					break;
    		}
  	} else if (angle == 90) { // counterclockwise
  			switch(direction) {
    			case 0:
    					direction = 3;
    					break;
    			default:
    					direction--;
    					break;
    		}
  	}
  	/*
  	while (!(getButtonPress(buttonEnter))) {}
		while (getButtonPress(buttonEnter)) {}
		*/
}

//turn left algorithm at turns or ends of paths
//TO DO: have it move a certain mimimum distance
void turnLeft(int currColor)
{
		bool firstTurn = true;
    do
    {
    		if(firstTurn)
    		{
        	rotateRobot(90, MOTSPINPOWER);
        	firstTurn = false;
      	}
      	else
      	{
      		rotateRobot(-90, MOTSPINPOWER);
				}

        nMotorEncoder[motorA] = 0;
        goRobot(MOTPOWER); // go until new colour - check if path is available
        while(SensorValue[S3] == currColor) { // could drive to a given distance instead and check colour sensor for some time, then decide from there
        		if(getButtonPress(buttonAny)) {
            		angleAdjust();
          	}
        }
        goRobot(0);

        if (SensorValue[S3] != PATHCOLOR && SensorValue[S3] != (int)colorRed && SensorValue[S3] != (int)colorGreen) //go back if not valid
        {
            goRobot(-MOTPOWER);
            while(nMotorEncoder[motorA] > 0) {
            		if(getButtonPress(buttonAny)) {
		            		angleAdjust();
		          	}
            }
            goRobot(0);
        }
    } while (SensorValue[S3] == currColor);
    // Back to middle of tile
    nMotorEncoder[motorA] = 0;
    goRobot(MOTPOWER);
    while(nMotorEncoder[motorA] <= PIECE_SIDELENGTH / 2.5 * ENC_CONV) {}
    goRobot(0);
    afterTurn = true;
}

//accounts for turning error in both directions
void turnError()
{
    for(int i = 1; i < 6; i++)
    {
        if (i % 2 == 0)
        {
            rotateRobot(5 * i, 5);
            if (SensorValue[S3] == PATHCOLOR || SensorValue[S3] == (int)colorRed || SensorValue[S3] == (int)colorGreen)
                i += 10;
        }
        else
        {
            rotateRobot(-(5 * i), 5);
            if (SensorValue[S3] == PATHCOLOR || SensorValue[S3] == (int)colorRed || SensorValue[S3] == (int)colorGreen)
                i += 10;
        }
    }
}


bool push(mStack *stack, short value)
{
  if (isFull(stack))
  {
    writeDebugStreamLine("Stack already at max size");
    return false;
  }

  // Add the value to the stack and increment size
  stack->elements[stack->size++] = value;
  return true;
}

short pop(mStack *stack)
{
  if (isEmpty(stack))
    return 0;

  // Decrement size and pop and value from the stack
  return stack->elements[--stack->size];
}

bool isEmpty(mStack *stack)
{
    if (stack->size == 0)
        return true;
    return false;
}

bool isFull(mStack *stack)
{
    if (stack->size == MAX_STACK_SIZE)
        return true;
    return false;
}

bool isValid(short row, short col) {
    // If cell is out of bounds
    if (row < 0 || col < 0 || row >= MAZE_SIZE || col >= MAZE_SIZE)
        return false;
    // If the cell is already visited
    if (isVisited[row * MAZE_SIZE + col])
        return false;
    short color = maze[row * MAZE_SIZE + col];
    // If color not path, green, or red, not valid
    if (color != PATHCOLOR && color != (int)colorRed && color != (int)colorGreen)
				return false;
    return true;
}

void dfs(short row, short col) {
	  // Initialize path variables
		for (short i = 0; i < MAX_STACK_SIZE; i++) {
				moves[i] = 0;
				path[i] = -1;
		}
		for (short i = 0; i < MAZE_SIZE * MAZE_SIZE; i++) {
				isVisited[i] = false;
		}
		short moveCounter = 0, pathCounter = 0;
		short finish = 0;

		// Initialize stack
		struct mStack stack;
		stack.size = 0;
		if(!push(stack, row * MAZE_SIZE + col))
				stopAllTasks();

		while(!isEmpty(stack)) {
				// Pop top
				short curr = pop(stack);
				// Check finish
				if (maze[curr] == (int)colorRed) {
						finish = curr;
						break;
				}

				// Mark visited
				isVisited[curr] = true;

				// Print element

				// Push all adj cells
				for (short i = 0; i < 4; i++) {
            short adjx = curr / MAZE_SIZE + dRow[i];
            short adjy = curr % MAZE_SIZE + dCol[i];

            short next = adjx * MAZE_SIZE + adjy;
            // Check if valid
						if (!isValid(adjx, adjy))
								continue;

						moves[moveCounter] = next * 100 + curr; // Tracks moves by child_curr
						moveCounter++;
            if(!push(stack, adjx * MAZE_SIZE + adjy))
            		stopAllTasks();
        }
		}
		// Find path to finish
		short cell = finish;
		short reversePath[MAX_STACK_SIZE];
		for (short i = 0; i < MAX_STACK_SIZE; i++) {
				reversePath[i] = -1;
		}

		while (cell != row * MAZE_SIZE + col) {
				for (short i = 0; i < MAX_STACK_SIZE; i++) {
						if (moves[i] / 100 == cell) { // Find cell
								short next = cell;
								cell = moves[i] % 100;
								reversePath[pathCounter] = getDirection(cell, next);
								pathCounter++;
								break;
						}
				}
		}
		for (short i = pathCounter - 1; i >= 0; i--) { // Reverse path
				path[pathCounter - i - 1] = reversePath[i];
		}
}

void goFinish() {
		dfs(currRow, currCol);

		short counter = 0;
		while(path[counter] != -1) { // All valid moves
				while(direction != path[counter]) { // Redirects
						rotateRobot(90, MOTSPINPOWER);
				}
				// Move forward a block
				nMotorEncoder[motorA] = 0;
				goRobot(MOTPOWER);
				while(nMotorEncoder[motorA] < PIECE_SIDELENGTH * ENC_CONV) {}
				goRobot(0);
				counter++;
		}
}

short getRow(short data) {
	return data / MAZE_SIZE;
}

short getCol(short data) {
	return data % MAZE_SIZE;
}

short getDirection(short curr, short next) {
	short rowDiff = getRow(next) - getRow(curr);
	short colDiff = getCol(next) - getCol(curr);
	if (rowDiff == -1 && colDiff == 0) {
			return 0;
	} else if (rowDiff == 0 && colDiff == 1) {
			return 1;
	} else if (rowDiff == 1 && colDiff == 0) {
			return  2;
	} else if (rowDiff == 0 && colDiff == -1) {
			return 3;
	} else {
			return -1;
	}
}

void addNode(int color) {
	switch(direction) {// Update maze position
	case 0:
			currRow--;
			break;
	case 1:
			currCol++;
			break;
	case 2:
			currRow++;
			break;
	case 3:
			currCol--;
			break;
	default:
			break;
	}
	maze[currRow * MAZE_SIZE + currCol] = color;

	// debugging
	for (int row = 0; row < MAZE_SIZE; row++) {
			for (int col = 0; col < MAZE_SIZE; col++) {
					writeLongPC(fout, (long)maze[row * MAZE_SIZE + col]);
					writeCharPC(fout, ' ');
			}
			writeEndlPC(fout);
	}
	writeEndlPC(fout);
}
