/*
Problem 1 approach:
    Scan the grid using a double for loop 
        for(Line in Grid)
        {
            for(Location in Line)
            {
                if(Grid[Line][Location] == '@')
                    //do something
            }
        }
    This will look at each YX coordinate in the Grid that contains a @

    This YX coordinate can then be used to generate 8 surrounding YX coordinates by 
    using different combinations of subtracting and adding from and to either, or both, the Y and X coordinate
    [Y-1][X-1] will, for example, look at a coordinate to the topleft of YX

    Each of these coordinate points can be looked up in the Grid to see if they contain a @ char and a tally can be
    kept about whether or not they do. If this tally goes over 3, we discard the current coordinate and move on.

    for(YX in Grid)
    {
        SurroundingLocations = GetSurroundingLocations(YX)

        if(ContainsLessThanThree)
        {
            Add YX location to a vector
            in main, just get the size of this vector
        }
    }

    where: 

    ContainsLessThanThree()
    {
        Tally = 0

        for(Neighbour in SurroundingLocations)
        {
            if(Neighbour == @)
            {
                ++Tally;
            }

            if(Tally > 3)
            {
                return false
            }
        }

        return true
    }

    This leaves a vector of all removeable rolls' YX location, which can then just be .size()'d
    to get a total amount of rolls and sets up for problem 2.

Problem 2 approach:

    Use the GetClearRollLocations function to snag a list of all currently available rolls
    Tally the rolls
    Remove them from the grid
    Rinse and repeat while GetClearRolls not yet returns 0

    As soon as GetClearRolls returns 0, all possible rolls have been removed

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


bool LocationIsOpen(int OriginY, int OriginX, int LenY, int LenX, std::vector<std::string>* p_InputDataVector)
{
    //Take an origin location and cycle through its neighbours (if possible)
    //Keep a running total of nearby rolls, if this ever exceeds 3 exit early

    int NeighbouringRolls {0};

    //Locate all neighbours by looping through Y, looping through X coordinates
    for(int OffsetY {OriginY - 1}; (OffsetY <= OriginY + 1); ++OffsetY)
    {
        //Bounds checking for Y
        if(OffsetY >= 0 && OffsetY < LenY)
        {
            for(int OffsetX {OriginX - 1}; (OffsetX <= OriginX + 1); ++OffsetX)
            {
                //Bounds checking for X
                if(OffsetX >= 0 && OffsetX < LenX)
                {
                    //Filter out the location of the roll itself (Y, X = OffsetY, OffsetX)
                    if(!((OriginY == OffsetY) && (OriginX == OffsetX)))
                    {
                        //If a neighbour is a roll, tally it
                        //If the total amount of neighbouring rolls exceeds 3, stop counting and return 
                        //"Nope, this roll isn't free"
                        if((*p_InputDataVector)[OffsetY][OffsetX] == '@')
                        {
                            ++NeighbouringRolls;

                            if(NeighbouringRolls == 4)
                            {
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    //Since the false condition hasn't triggered, this roll must be free
    return true;  
}


std::vector<int*>* GetClearRollLocations(std::vector<std::string>* p_InputDataVector)
{
    //Essentially, just loop through YX coordinates, get its neighbours YX, then check its neighbours char in the grid
    //if more than 3 neighbours are @, exit early
    //if not, track that YX coordinate
    std::vector<int*>* p_VectorOfRemoveableYXLocations = new std::vector<int*>;

    //Setting up some size data
    int LenY = (*p_InputDataVector).size();
    int LenX = (*p_InputDataVector)[0].size();

    //Look at each YX coordinate
    for(int Y {0}; Y < LenY; ++Y)
    {
        for(int X {0}; X < LenX; ++X)
        {
            //Only test coordinates with rolls in 'em
            if((*p_InputDataVector)[Y][X] == '@')
            {
                if(LocationIsOpen(Y, X, LenY, LenX, p_InputDataVector))
                {
                    int* p_YXCoordinate = new int[2];
                    p_YXCoordinate[0] = Y;
                    p_YXCoordinate[1] = X;
                    p_VectorOfRemoveableYXLocations->emplace_back(p_YXCoordinate);
                }
            }
        }
    }

    return p_VectorOfRemoveableYXLocations;
}


void CleanUpVectorData(std::vector<int*>* p_VectorOfRemoveables)
{
    //Since this gets tossed around a lot, might as well chuck it into its own function
    for(auto delete_YX : *p_VectorOfRemoveables)
    {
        delete[] delete_YX;
        delete_YX = nullptr;
    }
    delete p_VectorOfRemoveables;
    p_VectorOfRemoveables = nullptr;
}


int SolveProblemOne(int& Timed, std::vector<std::string>* p_InputDataVector)
{
    if(Timed){std::cout << "Starting Problem One:  ";PrintTimeNow();}

    int TotalAccesibleRolls {0};

    //Get a vector of all roll coordinates that can be removed
    std::vector<int*>* p_VectorOfRemoveables = GetClearRollLocations(p_InputDataVector);

    //Then just grab the size of that vector
    TotalAccesibleRolls = p_VectorOfRemoveables->size();

    //Clean up
    CleanUpVectorData(p_VectorOfRemoveables);

    if(Timed){std::cout << "Ending Problem One:    ";PrintTimeNow();}

    return TotalAccesibleRolls;
}


int SolveProblemTwo(int& Timed, std::vector<std::string>* p_InputDataVector)
{
    if(Timed){std::cout << "Starting Problem Two:  ";PrintTimeNow();}

    //Summing all removed rolls here
    int TotalRemoved {0};

    //Grab all currently available rolls
    std::vector<int*>* p_VectorOfRemoveables = GetClearRollLocations(p_InputDataVector);

    //While you can still remove rolls
    while(p_VectorOfRemoveables->size() != 0)
    {   
        //Tally the removable ones
        TotalRemoved += p_VectorOfRemoveables->size();

        //Remove all rolls that can be removed
        for(int* p_YX : *p_VectorOfRemoveables)
        {
            (*p_InputDataVector)[p_YX[0]][p_YX[1]] = '.';
        }

        //Clean up 
        CleanUpVectorData(p_VectorOfRemoveables);

        //Look for newly removeable rolls
        p_VectorOfRemoveables = GetClearRollLocations(p_InputDataVector);
    }

    //Final clean up after while loop exits
    CleanUpVectorData(p_VectorOfRemoveables);

    if(Timed){std::cout << "Ending Problem Two:    ";PrintTimeNow();}

    return TotalRemoved;
}


int main(int argc, char* argv[])
{  
    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    if(Timed){PrintTimeNow();}
    
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    //Problem 2 is just Problem 1, but remove known accesible locations each iteration
    //during a while loop until none can be removed any longer
    std::cout << "Problem One:\n" <<  SolveProblemOne(Timed, p_InputDataVector) << "\n";
    std::cout << "Problem Two:\n" <<  SolveProblemTwo(Timed, p_InputDataVector) << "\n";

    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;

    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}