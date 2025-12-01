/*
Problem 1 approach:
    Since each value can only be between 0 and 99, there are a total of 100 options
    So each turn, whether left OR right, has to be modulo'd by 100 to get the relative turn, disregarding "full turns"
        -A turn of 19  will be a turn of 19
        -A turn of 100 will be a turn of 0, essentially nothing has changed
        -A turn of 101 will be a turn of 1, counting the first 100 as a "full turn"

    This will be known as the Turn Window
    
    
    Right turns simply increment the current value by the Turn Window
        This means a right turn can be higher than 99 
    Left turns decrement, or seen from another angle; increment the current value by (100 - Turn Window) 
        This means a left turn can never go below 0

    Lastly, since a right turn can go over 99, the final value has one more % 100 step to bring it within the 0-99 range

    if a Current value is 0, tally it

Problem 2 approach:
    There are 4 options to tally and test for  
        1. The dial ends up at 0               >>> Test if CurrentValue % TotalOptions == 0             
        2. The dial ends up overshooting 99    >>> Test if CurrentValue + Window > (TotalOptions - 1)
        3. The dial ends up undershooting 0    >>> Test if CurrentValue - Window is negative
        4. The dial does multiple turns        >>> Floor divide Clicks by TotalOptions 

    Using Problem 1's approach as a basic framework, isolate each of these 4 separate cases and tally them individually

*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>


std::vector<std::string>* GetFileData(std::string DataPath)
{
    //Read the input text file line by line and create a vector that can be used during runtime
    
    //Setting up variables to stream data and store it into a vector
    std::vector<std::string>* p_VectorOfLines = new std::vector<std::string>;
    std::ifstream FileStream(DataPath);
    std::string Line;

    //Read the data stream and store lines in Line
    if(FileStream.is_open())
    {
        while(getline(FileStream, Line))
        {
            p_VectorOfLines->emplace_back(Line);
        }
    }
    else //Error handling if file cannot be opened
    {
        std::cout << "Cannot open file" << std::endl;
    }

    return p_VectorOfLines;
}


void PrintTimeNow()
{
    //Function to print specific time points if required

    std::time_t Now;
    struct tm* Local;
    std::time(&Now);
    Local = std::localtime(&Now);
    std::cout << std::asctime(Local); 
}


int SolveProblemOne(int& Timed, std::vector<std::string>*& p_VectorInput, int CurrentValue, int TotalOptions)
{
    if(Timed){std::cout << "Starting Problem One:  "; PrintTimeNow();}

    //Keep track of how often it lands on 0
    int TallyZeroes {0};

    //Go through each instruction in the vector
    for(std::string Line : (*p_VectorInput))
    {
        //Determine the Turn Window
        int TurnWindow = (std::stoi(Line.substr(1))) % TotalOptions;

        //Differentiate between Left and Right
        if(Line[0] == 'R')
        {
            CurrentValue += TurnWindow;
        }
        else if(Line[0] == 'L') //could just be 'else', but verbose for clarity
        {
            CurrentValue += (TotalOptions - TurnWindow);
        }

        //Scale back to the 0-99 range
        CurrentValue = CurrentValue % TotalOptions;

        //Check for zeroes
        if(!CurrentValue){++TallyZeroes;}
    }

    if(Timed){std::cout << "Ending Problem One:    "; PrintTimeNow();}

    return TallyZeroes;
}


int SolveProblemTwo(int& Timed, std::vector<std::string>*& p_VectorInput, int CurrentValue, int TotalOptions)
{
    if(Timed){std::cout << "Starting Problem Two:  "; PrintTimeNow();}

    //Keep track of how often 0 is PASSED
    int TallyZeroes {0};

    //Go through each instruction in the vector
    for(std::string Line : (*p_VectorInput))
    {
        //Isolating the number of clicks 
        int Clicks {std::stoi(Line.substr(1))};

        //See how many full turns are executed
        TallyZeroes += int(Clicks / TotalOptions);

        //Determine the Turn Window
        int TurnWindow = Clicks % TotalOptions;

        //Differentiate between Left and Right
        if(Line[0] == 'R')
        {
            CurrentValue += TurnWindow;

            if((CurrentValue > (TotalOptions - 1)) && (CurrentValue % TotalOptions))
            {
                //The Current Value is higher than 99, so it must've passed 0
                //If it IS zero, this is excluded, to stop double counting
                ++TallyZeroes;
            }
        }
        else if(Line[0] == 'L') //could just be 'else', but verbose for clarity
        {
            if(((CurrentValue - TurnWindow) < 0) && (CurrentValue % TotalOptions))
            {
                //The Current Value is negative, so it must've passed 0
                //If it IS zero, this is excluded, to stop double counting
                ++TallyZeroes;
            }
            CurrentValue += (TotalOptions - TurnWindow);
        }

        //Scale back to the 0-99 range
        CurrentValue = CurrentValue % TotalOptions;

        //Check for zeroes that have not been counted before
        if(!CurrentValue){++TallyZeroes;}
    }

    if(Timed){std::cout << "Ending Problem Two:    "; PrintTimeNow();}

    return TallyZeroes;
}


int main(int argc, char* argv[])
{  
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    
    std::cout << "Problem One:\n" << SolveProblemOne(Timed, p_InputDataVector, 50, 100) << std::endl;
    std::cout << "Problem Two:\n" << SolveProblemTwo(Timed, p_InputDataVector, 50, 100) << std::endl;

    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;

    return 0;
}