#define MAZE_SIZE 9
#define MAX_STACK_SIZE 50
int maze[MAZE_SIZE * MAZE_SIZE];
bool isVisited[MAZE_SIZE * MAZE_SIZE];
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
bool isValid(bool *vis, int data);

void dfs(int *maze);

// Mapping
int makeData(int row, int col, int color);

int getColor(int data);

int getRow(int data);

int getCol(int data);

// void addNode();


task main() {
    struct mStack stack;

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

bool isValid(bool *vis, int data) {
		int row = getRow(data), col = getCol(data), color = getColor(data);
    // If cell is out of bounds
    if (row < 0 || col < 0 || row >= MAZE_SIZE || col >= MAZE_SIZE)
        return false;

    // If the cell is already visited
    if (vis[row * col + col])
        return false;

    // If color not black, green, or red, not valid
    if (color != (int)colorBlack && color != (int)colorRed && color != (int)colorGreen)
				return false;
    return true;
}

void dfs(int *maze, bool *vis, int row, int col, int color) {
	  // Initialize stack
		struct mStack stack;
		stack.size = 0;
		if(!push(stack, makeData(row, col, color))
				stopAllTasks();

		while(!isEmpty(stack)) {
				// Pop top
				int curr = pop(stack);

				// Check finish
				if (getColor(curr) == (int)colorRed)
						break;

				// Mark visited
				vis[row * col + col];

				// Print element

				// Push all adj cells
				for (int i = 0; i < 4; i++) {
            int adjx = row + dRow[i];
            int adjy = col + dCol[i];

            // Check if valid
						if (!isValid(vis, curr))
								continue;
            push(stack, adjx * adjy + adjy);
        }
		}
}

int makeData(int row, int col, int color) {
	return color * 100 + row * 10 + col;
}

int getColor(int data) {
	return data / 100;
}

int getRow(int data) {
	return (data % 100) / 10;
}

int getCol(int data) {
	return data % 10;
}
