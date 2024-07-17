#define MAZE_SIZE 10
#define MAX_STACK_SIZE 80
// Mapping vars
int maze[MAZE_SIZE * MAZE_SIZE]; // ROW * MAX_ROW + COL to collape a 2d array into 1d
bool isVisited[MAZE_SIZE * MAZE_SIZE];
int moves[MAX_STACK_SIZE];
char path[MAX_STACK_SIZE];
int currRow = 0, currCol = 0, direction = 2; // Start at top left (0,0); direction (N,0)(E,1)(S,2)(W,3)
// Order: WSEN
const int dRow[] = { 0, -1, 0, 1 };
const int dCol[] = { 1, 0, -1, 0 };

// Stack
typedef struct {
  int elements[MAX_STACK_SIZE]; // array of elements
  int size; // current size of the stack
} mStack;

bool push(mStack *stack, int value);

int pop(mStack *stack);

bool isEmpty(mStack *stack);

bool isFull(mStack *stack);

// DFS
bool isValid(int row, int col);

char* dfs(int *maze, int row, int col);

// Mapping

int getRow(int data);

int getCol(int data);

char getDirection(int curr, int next);

void addNode(int *maze);

task main() {
		for (int row = 0; row < MAZE_SIZE; row++) {
				for (int col = 0; col < MAZE_SIZE; col++) {
					maze[row * MAZE_SIZE + col] = (int)colorBlack;
				}
		}
		maze[90] = (int)colorGreen;

		char path[MAX_STACK_SIZE] = dfs(maze, currRow, currCol);
		writeDebugStreamLine("%s", path);

		while(!getButtonPress(buttonEnter)) {}
}

bool push(mStack *stack, int value)
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

int pop(mStack *stack)
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

bool isValid(int row, int col) {
		int color = maze[row * MAZE_SIZE + col];
    // If cell is out of bounds
    if (row < 0 || col < 0 || row >= MAZE_SIZE || col >= MAZE_SIZE)
        return false;
    // If the cell is already visited
    if (isVisited[row * MAZE_SIZE + col])
        return false;
    // If color not black, green, or red, not valid
    if (color != (int)colorBlack && color != (int)colorRed && color != (int)colorGreen)
				return false;
    return true;
}

char* dfs(int *maze, int row, int col) {
	  // Initialize path variables
		for (int i = 0; i < MAX_STACK_SIZE; i++) {
				moves[i] = 0;
				isVisited[i] = false;
				path[i] = '0';
		}
		int moveCounter = 0, pathCounter = 0;
		int finish = 0;

		// Initialize stack
		struct mStack stack;
		stack.size = 0;
		if(!push(stack, row * MAZE_SIZE + col))
				stopAllTasks();

		while(!isEmpty(stack)) {
				// Pop top
				int curr = pop(stack);

				// Check finish
				if (maze[curr] == (int)colorRed) {
						finish = curr;
						break;
				}

				// Mark visited
				isVisited[curr] = true;

				// Print element

				// Push all adj cells
				for (int i = 0; i < 4; i++) {
            int adjx = row + dRow[i];
            int adjy = col + dCol[i];

            int next = maze[adjx * MAZE_SIZE + adjy];
            // Check if valid
						if (!isValid(adjx, adjy))
								continue;

						moves[moveCounter] = next * 100 + curr; // Tracks moves by child_curr
            if(!push(stack, adjx * adjy + adjy))
            		stopAllTasks();
        }
		}
		// Find path to finish
		int cell = finish;
		while (cell != row * MAZE_SIZE + col) {
				for (int i = 0; i < MAX_STACK_SIZE; i++) {
						if (moves[i] / 100 == cell) { // Find cell
								int next = cell;
								cell = moves[i] % 100;
								path[pathCounter] = getDirection(cell, next);
								pathCounter++;
								break;
						}
				}
		}
		char reversePath[MAX_STACK_SIZE];
		for (int i = pathCounter - 1; i >= 0; i--) { // Reverse path
				reversePath[pathCounter - i - 1] = path[i];
		}
		return reversePath;
}

int getRow(int data) {
	return data / MAZE_SIZE;
}

int getCol(int data) {
	return data % MAZE_SIZE;
}

char getDirection(int curr, int next) {
	int rowDiff = getRow(next) - getRow(curr);
	int colDiff = getCol(next) - getCol(curr);

	if (rowDiff == -1 && colDiff == 0) {
			return 'N';
	} else if (rowDiff == 0 && colDiff == 1) {
			return 'E';
	} else if (rowDiff == 1 && colDiff == 0) {
			return  'S';
	} else if (rowDiff == 0 && colDiff == -1) {
			return 'W';
	} else {
			return '-';
	}
}

void addNode(int *maze) {
	int newRow = currRow, newCol = currCol;
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
