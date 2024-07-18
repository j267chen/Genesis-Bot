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
void dfs(short *maze, short row, short col);
// Mapping
short getRow(short data);
short getCol(short data);
short getDirection(short curr, short next);
void addNode(short *maze);

task main() {

		for (short row = 0; row < MAZE_SIZE; row++) {
				for (short col = 0; col < MAZE_SIZE; col++) {
					maze[row * MAZE_SIZE + col] = (short)colorBlack;
				}
		}
		maze[35] = (int)colorRed;
		for (short i = 0; i < MAX_STACK_SIZE; i++) {
				path[i] = -1;
		}
		dfs(maze, currRow, currCol);
		for(short i = 0; i < MAX_STACK_SIZE; i++) {
				writeDebugStreamLine("%hd", path[i]);
		}

		while(!getButtonPress(buttonEnter)) {}

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
    // If color not black, green, or red, not valid
    if (color != (int)colorBlack && color != (int)colorRed && color != (int)colorGreen)
				return false;
    return true;
}

void dfs(short *maze, short row, short col) {
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

void addNode(short *maze) {
	short newRow = currRow, newCol = currCol;
	switch(direction) {// Update maze position
	case 0:
			newRow--;
			break;
	case 1:
			newCol++;
			break;
	case 2:
			newRow++;
			break;
	case 3:
			newCol--;
			break;
	default:
			break;
	}
	maze[newRow * MAZE_SIZE + newCol] = SensorValue[S3];
}
