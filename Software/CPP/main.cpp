//all robot code

#include <iostream>
#include "default.hpp"

using namespace std;

int main(){
    configureAllSensors();

    displayString(2, "Maze Runner")
    displayString(3, "Made by Jason Chen, Zachary Barandino, Eric Mak, and Eric Shaw")
    displayString(5, "Press Enter to start");
    while (!(getButtonPress(ButtonEnter)))
    {}
    while (getButtonPress(ButtonEnter))
    {}

    time1[T1] = 0;

    //version 1 of maze solving: (turn left algorithm)
    bool unsolved = true;
    while (unsolved)
    {
        goRobot(50);
        while (SensorValue[S3] == (int)colorRed)
        {}
        goRobot(0);

        if (SensorValue[S3] == (int)colorGreen)
        {
            unsolved = false;
        }
        spinRobot(90, 25);
    }
    /*
    need to introduce a secondary objective: pick up block and sort it
    
    could introduce a way to recognize turns and adjust accordingly
    could introduce A* algorithm to solve maze
    */

    displayString(5, "Maze Solved!");
    displayString(6, "Time: %f s", time1[T1] / 1000.0 );
    wait1Msec(10000);

    return 0;
}


//code to detect, locate, pick up, and sort block

//code to grab block

//code to map maze

//code to solve maze