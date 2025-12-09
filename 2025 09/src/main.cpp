/*
Problem 1 approach:
    Loop through each line in the input file and from each following line get the absolute differences in X and Y.
    Use these absolute X and Y values to calculate, and keep track of, the largest area
    No need to check every line against every other line, since a -> b == a <- b
    
    Remember that the difference has to be +1'd, since a point itself is already a 1x1 square

    Keep track of the largest area

Will need to revisit this in the future to properly solve problem 2
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>


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


long unsigned int GetSquareArea(std::string& OriginXY, std::string& TerminalXY)
{
    //Turn the strings into numerical values and get the area between them

    //Stores {Xo, Yo, Xt, Yt}
    std::vector<int> ExtractedInts {};

    for(std::string XY : {OriginXY, TerminalXY})
    {
        //Isolate the two numerical values
        std::stringstream StringStream(XY);
        std::string IntAsString;
        while(getline(StringStream, IntAsString, ','))
        {
            ExtractedInts.emplace_back(std::stoi(IntAsString));
        }
    }
    
    //Get the absolute area
    //+1 the differences, since a tile by itself is already 1x1 
    long unsigned int Area {(long unsigned int)(1 + std::abs(ExtractedInts[0] - ExtractedInts[2])) * 
                            (long unsigned int)(1 + std::abs(ExtractedInts[1] - ExtractedInts[3]))};

    return Area;
}


long unsigned int SolveProblemOne(int& Timed, std::vector<std::string>* p_InputDataVector)
{
    if(Timed){std::cout << "Starting Problem One:  ";PrintTimeNow();}

    //Go through each item in the list and then get the absolute differences to each other item
    
    //Some bounds data
    int MaxLine {(int)p_InputDataVector->size()};

    //Keep track of values that result in the largest area
    long unsigned int LargestArea {0};

    //Loop through each line and extract XYs
    for(int i_Line {0}; i_Line < MaxLine; ++i_Line)
    {
        //For this line, compare all following lines
        for(int i_FollowingLine {i_Line + 1}; i_FollowingLine < MaxLine; ++i_FollowingLine)
        {
            long unsigned int Area {GetSquareArea((*p_InputDataVector)[i_Line], (*p_InputDataVector)[i_FollowingLine])};

            if(Area > LargestArea)
            {
                LargestArea = Area;
            }
        }
    }

    if(Timed){std::cout << "Ending Problem One:    ";PrintTimeNow();}
    return LargestArea;
}


int main(int argc, char* argv[])
{  
    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    if(Timed){PrintTimeNow();}
    
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    std::cout << "Problem One:\n" << SolveProblemOne(Timed, p_InputDataVector) << "\n";

    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;

    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}