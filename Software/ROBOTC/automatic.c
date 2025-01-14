#include "PC_FileIO.c"

// Mapping vars
const short D_ROW[] = { 0, 1, 0, -1 }; // Order: WSEN
const short D_COL[] = { 1, 0, -1, 0 };

// DFS
const int MAZE_SIZE = 6;
const int MAX_STACK_SIZE = 70;
bool isValid(short & maze, bool & isVisited, short row, short col);
void dfs(short & maze, bool & isVisited, short & moves, short & path, short row, short col);
void goFinish(short & direction, short & maze, bool & isVisited, short & moves, short & path, short & row, short & col);

// Mapping
short getRow(short data);
short getCol(short data);
short getDirection(short curr, short next);
bool addNode(short & direction, short & maze, short & currRow, short & currCol, int color);

// Stack
struct mStack {
	short elements[MAX_STACK_SIZE]; // array of elements
	short size; // current size of the stack
};
bool push(mStack *stack, short value);
short pop(mStack *stack);
bool isEmpty(mStack *stack);
bool isFull(mStack *stack);

// Robot Movement
void angleAdjust();
void configureAllSensors();
void goRobot(int motorPower);
int grabBlock(bool & blockObtained);
void releaseBlock(bool & blockObtained);
void sortBlock(short & direction, bool & afterSort, int colorInteger, int & colorBlocks, bool & numblocksobtained, bool & blockObtained);
void rotateRobot(float angle, int motorPower, short & direction);
void turnError(short & direction);
void turnLeft(short & direction, int currColor, bool & visited);

//constants for mapping and navigation
const int MOTPOWER = 35;
const int MOTSPINPOWER = 25;
const short NUMBLOCKS = 3;
const float PIECE_SIDELENGTH = 15;
const int PATHCOLOR = (int)colorBlue;
const float ENC_CONV = 180 / (PI * 2.75);

TFileHandle fout;

task main(){
	// Mapping vars
	short maze[MAZE_SIZE * MAZE_SIZE]; // ROW * MAZE_SIZE + COL to collape a 2d array into 1d
	bool isVisited[MAZE_SIZE * MAZE_SIZE];
	short moves[MAX_STACK_SIZE];
	short path[MAX_STACK_SIZE];
	short currRow = -1, currCol = 0, direction = 2; // Start at top left (0,0); direction (N,0)(E,1)(S,2)(W,3)

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
	int blockColor = 5;

	//constants for mapping and navigation
	const short NUMBLOCKS = 3;
	const float PIECE_SIDELENGTH = 15;
	float timetofinish = 0;
	bool blockObtained = false, foundFinish = false, afterSort = false, visited = false;
	int numblocksobtained = 0;
	int colorBlocks[3] = {(int)colorRed, (int)colorGreen, (int)colorBlue};

	// File
	bool fileOkay = openWritePC(fout, "fileWrite.txt");

	// Program start
	configureAllSensors();

	displayString(2, "Maze Runner");
	displayString(3, "Made by JC, ZB, EM, and ES");
	displayString(5, "Press Enter to start/pause");
	//displayString(7, "%hd", colorBlocks[0]);
	while (!(getButtonPress(buttonEnter)))
	{}
	while (getButtonPress(buttonEnter))
	{}
	displayString(3, "                               ");
	displayString(5, "                               ");
	displayString(3, "Press Enter to start/pause");
	time1[T1] = 0;

	while (numblocksobtained < NUMBLOCKS)
	{
		if (SensorValue[S3] == PATHCOLOR)
		{
			visited = addNode(direction, maze, currRow, currCol, PATHCOLOR);

			nMotorEncoder[motorA] = 0;
			goRobot(MOTPOWER);
			while(nMotorEncoder[motorA] < PIECE_SIDELENGTH * ENC_CONV)
			{
				if(SensorValue[S1] != 0)
				{
					goRobot(0);
					blockColor = grabBlock(blockObtained);

					if(foundFinish)
					{
						visited = addNode(direction, maze, currRow, currCol, PATHCOLOR);
						goFinish(direction, maze, isVisited, moves, path, currRow, currCol);
					}

					goRobot(MOTPOWER);
				}
				if(getButtonPress(buttonAny))
					angleAdjust();
			}
			goRobot(0);
		}
		else if (SensorValue[S3] == (int)colorGreen)
		{
			if(!afterSort)
				addNode(direction, maze, currRow, currCol, (int)colorGreen);

			foundFinish = true;
			//displayString(10, "%hd", numblocksobtained); //debugging

			if(blockObtained) // sort
				sortBlock(direction, afterSort, blockColor);
			else  // go back
				turnLeft(direction, (int)colorGreen, false);
		}
		else if (SensorValue[S3] == (int)colorRed)
		{
			bool visited = addNode(direction, maze, currRow, currCol, (int)colorRed);
			turnLeft(direction, (int)colorRed, visited);
		}
		else
			turnError(direction);
	}

	timetofinish = time1[T1]/1000.0;

	displayString(11, "Maze Solved!");
	displayString(13, "Time: %f s", timetofinish);
	wait1Msec(10000);
	closeFilePC(fout);
}

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

void configureAllSensors()
{
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
	wait1Msec(100);
	SensorMode[S4] = modeEV3Gyro_RateAndAngle;
	wait1Msec(100);
}

void goRobot(int motorPower) {
	motor[motorA] = motor[motorD] = motorPower;

	displayString(7, "                                       ");
	displayString(9, "                                       ");
	displayString(7, "Time elapsed %f", time1[T1]/1000.0);
	displayString(9, "Current gyro angle %f", getGyroDegrees(S4));
}

int grabBlock(bool & blockObtained)
{
	blockObtained = true;

	motor[motorB] = -100;
	int color = SensorValue[S2];
	wait1Msec(800);

	motor[motorC] = 20;
	wait1Msec(1000);

	while(!getButtonPress(buttonAny)) {
		if(SensorValue[S1] == 1){ // Manual release and regrab loop
			blockObtained = false;
			motor[motorB] = 100;
			wait1Msec(1000);
			motor[motorB] = 0;
			motor[motorC] = -15;
			wait1Msec(1200);
			motor[motorC] = 0;
			color = grabBlock(blockObtained);
		}
	}
	return color;
}

void releaseBlock(bool & blockObtained){
	motor[motorC] = -15;
	wait1Msec(1200);
	motor[motorC] = 0;
	motor[motorB] = 100;
	wait1Msec(1000);
	motor[motorB] = 0;

	while (!(getButtonPress(buttonAny)))
	{}
	while (getButtonPress(buttonAny))
	{}

	blockObtained = false;
}

void sortBlock(short & direction, bool & afterSort, int colorInteger, int & colorBlocks, bool & numblocksobtained, bool & blockObtained) {
	float ext = 2;

	// Face south
	if(direction == 1)
		rotateRobot(-90, MOTSPINPOWER, direction);
	while(direction != 2){
		rotateRobot(90, MOTSPINPOWER, direction);
	}

	//Position to middle of tile
	nMotorEncoder[motorA] = 0;
	goRobot(MOTPOWER);
	while(nMotorEncoder[motorA] < PIECE_SIDELENGTH * ext * ENC_CONV) {}
	goRobot(0);

	// Turn if needed
	if(colorInteger - colorBlocks[0] == 0) //check red, left
	{
		rotateRobot(90, MOTSPINPOWER, direction);
	}
	else if (colorInteger == colorBlocks[2]) //check blue, right
	{
		rotateRobot(-90, MOTSPINPOWER, direction);
	}

	// Go forward, drop, go back
	nMotorEncoder[motorA] = 0;
	goRobot(MOTPOWER);
	while(nMotorEncoder[motorA] < PIECE_SIDELENGTH *0.85 * ENC_CONV) {}
	goRobot(0);
	releaseBlock(blockObtained);
	wait1Msec(500);
	numblocksobtained++;
	goRobot(-MOTPOWER);
	while(nMotorEncoder[motorA] > 0) {}
	goRobot(0);

	// Back to green
	if(colorInteger == colorBlocks[0]) //check red, left
	{
		rotateRobot(90, MOTSPINPOWER, direction);
	}
	else if (colorInteger == colorBlocks[1])//check green, straight
	{
		rotateRobot(90, MOTSPINPOWER, direction);
		rotateRobot(90, MOTSPINPOWER, direction);
	}
	else // check blue, right
	{
		rotateRobot(-90, MOTSPINPOWER, direction);
	}

	nMotorEncoder[motorA] = 0;
	goRobot(MOTPOWER);
	while(nMotorEncoder[motorA] < PIECE_SIDELENGTH * ext * ENC_CONV) {}
	goRobot(0);

	afterSort = true;
	angleAdjust();
}

void rotateRobot(float angle, int motorPower, short & direction) {
	motorPower = abs(motorPower);
	resetGyro(S4);
	wait1Msec(50);

	if(angle >= 0) //counter-clockwise
	{
		motor[motorA] = -motorPower;
		motor[motorD] = motorPower;
	}
	else //cw
	{
		motor[motorA] = motorPower;
		motor[motorD] = -motorPower;
	}

	while(abs(getGyroDegrees(S4)) < abs(angle))
	{
		if(getButtonPress(buttonAny))
			angleAdjust();
	}
	motor[motorA] = motor[motorD] = 0;

	// Update direction
	if(angle == -90) // clockwise
	{
		switch(direction) {
		case 3:
			direction = 0;
			break;
		default:
			direction++;
			break;
		}
	}
	else if (angle == 90) // counterclockwise
	{
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

void turnLeft(short & direction, int currColor, bool & visited)
{
	int rotate = 90;
	/*
	if(visited)
	rotate = -90;
	*/
	bool firstTurn = true;
	do {
		if(firstTurn)
		{
			rotateRobot(rotate, MOTSPINPOWER, direction);
			firstTurn = false;
		}
		else
			rotateRobot(-rotate, MOTSPINPOWER, direction);

		nMotorEncoder[motorA] = 0;
		goRobot(MOTPOWER);
		while(nMotorEncoder[motorA] < PIECE_SIDELENGTH * ENC_CONV)
		{
			if(getButtonPress(buttonAny))
				angleAdjust();
		}
		goRobot(0);

		if (SensorValue[S3] != PATHCOLOR && SensorValue[S3] != (int)colorRed && SensorValue[S3] != (int)colorGreen) //go back if not valid
		{
			goRobot(-MOTPOWER);
			while(nMotorEncoder[motorA] > 0)
			{
				if(getButtonPress(buttonAny))
					angleAdjust();
			}
			goRobot(0);
		}
		else
			break;
	} while (true);
	//angleAdjust(); manual adjust
}

void turnError(short & direction) //accounts for turning error in both directions
{
	for(int i = 1; i < 6; i++)
	{
		if (i % 2 == 0)
		{
			rotateRobot(5 * i, 5, direction);
			if (SensorValue[S3] == PATHCOLOR || SensorValue[S3] == (int)colorRed || SensorValue[S3] == (int)colorGreen)
				i += 10;
		}
		else
		{
			rotateRobot(-(5 * i), 5, direction);
			if (SensorValue[S3] == PATHCOLOR || SensorValue[S3] == (int)colorRed || SensorValue[S3] == (int)colorGreen)
				i += 10;
		}
	}
}


bool push(mStack *stack, short value)
{
	if (isFull(stack))
		return false;

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

bool isValid(short & maze, bool & isVisited, short row, short col)
{
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

void dfs(short & maze, bool & isVisited, short & moves, short & path, short row, short col)
{
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

	while(!isEmpty(stack))
	{
		// Pop top
		short curr = pop(stack);
		// Check finish
		if (maze[curr] == (int)colorGreen)
		{
			finish = curr;
			break;
		}

		// Mark visited
		isVisited[curr] = true;

		// Push all adj cells
		for (short i = 0; i < 4; i++) {
			short adjx = curr / MAZE_SIZE + D_ROW[i];
			short adjy = curr % MAZE_SIZE + D_COL[i];
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

	while (cell != row * MAZE_SIZE + col)
	{
		for (short i = 0; i < MAX_STACK_SIZE; i++) {
			if (moves[i] / 100 == cell) // Find cell
			{
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

	// debugging
	writeLongPC(fout, currRow);
	writeCharPC(fout, ' ');
	writeLongPC(fout, currCol);
	writeEndlPC(fout);
	for (int i = 0; i < MAX_STACK_SIZE; i++) {
		writeLongPC(fout, (long)path[i]);
		writeCharPC(fout, ' ');
	}
	writeEndlPC(fout);
}

void goFinish(short & direction, short & maze, bool & isVisited, short & moves, short & path, short & row, short & col)
{
	dfs(maze, isVisited, moves, path, row, col);

	writeLongPC(fout, direction);
	short counter = 0;
	while(path[counter] != -1)  // All valid moves
	{
		// Redirects
		if (direction - path[counter] == -1 || (direction == 3 && path[counter] == 0))
			rotateRobot(-90, MOTSPINPOWER); // Right turn

		while(direction != path[counter]) //Left turns
		{
			rotateRobot(90, MOTSPINPOWER);
		}

		// Move forward a block
		nMotorEncoder[motorA] = 0;
		goRobot(MOTPOWER);
		while(nMotorEncoder[motorA] < PIECE_SIDELENGTH * ENC_CONV) {}
		goRobot(0);
		addNode(direction, maze, row, col, SensorValue[S3]);

		counter++;
		/*
		if(counter%3 == 0)
			angleAdjust(); manual adjust
		*/
	}
}

short getRow(short data)
{
	return data / MAZE_SIZE;
}

short getCol(short data)
{
	return data % MAZE_SIZE;
}

short getDirection(short curr, short next)
{
	short rowDiff = getRow(next) - getRow(curr);
	short colDiff = getCol(next) - getCol(curr);

	if (rowDiff == -1 && colDiff == 0)
		return 0;
	else if (rowDiff == 0 && colDiff == 1)
		return 1;
	else if (rowDiff == 1 && colDiff == 0)
		return  2;
	else if (rowDiff == 0 && colDiff == -1)
		return 3;
	else
		return -1;
}

bool addNode(short & direction, short & maze, short & currRow, short & currCol, int color)
{
	bool isVisit = false;

	//debugging
	writeLongPC(fout, direction);
	writeEndlPC(fout);

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
	if(maze[currRow * MAZE_SIZE + currCol] == -1)
		maze[currRow * MAZE_SIZE + currCol] = color;
	else
		isVisit = true;

	// debugging
	for (int row = 0; row < MAZE_SIZE; row++) {
		for (int col = 0; col < MAZE_SIZE; col++) {
			writeLongPC(fout, (long)maze[row * MAZE_SIZE + col]);
			writeCharPC(fout, ' ');
		}
		writeEndlPC(fout);
	}
	writeLongPC(fout, direction);
	writeEndlPC(fout);
	writeEndlPC(fout);

	return isVisit;
}
