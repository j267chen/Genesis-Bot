#define MAZE_SIZE 10
#define MAX_STACK_SIZE 50
int maze[MAZE_SIZE][MAZE_SIZE];
bool isVisited[MAZE_SIZE][MAZE_SIZE];

// Stack
typedef struct {
  int value;                            // value held by element
} stackElement;

typedef struct {
  stackElement elements[MAX_STACK_SIZE]; // array of elements
  int size;                               // current size of the   stack
} tStack;

bool push(tStack *stack, int value);

int pop(tStack *stack);

bool isEmpty(tStack *stack);

bool isFull(tStack *stack);

// DFS
bool isValid(bool vis[][MAZE_SIZE], int row, int col);

void dfs(int *maze);

// int dfsTraversal()

// Mapping


task main() {
    struct tStack stack;

    // Order: WSEN
    int dRow[] = { 0, 1, 0, -1 };
		int dCol[] = { -1, 0, 1, 0 };


}

bool push(tStack *stack, int value)
{
  if (isFull(stack))
  {
    writeDebugStreamLine("Stack already at max size");
    return false;
  }

  // Add the value to the stack and increment size
  stack->elements[stack->size++].value = value;
  return true;
}

int pop(tStack *stack)
{
  if (isEmpty(stack))
    return 0;

  // Decrement size and pop and value from the stack
  return stack->elements[--stack->size].value;
}

bool isEmpty(tStack *stack)
{
    if (stack->size == 0)
        return true;
    return false;
}

bool isFull(tStack *stack)
{
    if (stack->size == MAX_STACK_SIZE)
        return true;
    return false;
}

bool isValid(bool vis[][MAZE_SIZE], int row, int col) {
    // If cell is out of bounds
    if (row < 0 || col < 0 || row >= MAZE_SIZE || col >= MAZE_SIZE)
        return false;

    // If the cell is already visited
    if (vis[row][col])
        return false;

    // Otherwise, it can be visited
    return true;
}
