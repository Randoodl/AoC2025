/*
Problem 1 approach:
    Similar to Day 2, construct a map of [int] = int to keep track of all ranges in an ordered way
    Also, in a second (inefficient, but easy to implement) pass, isolate all ingredient IDs

    Then start testing IDs to [key] and [value]
        1. if ID < [key]:  pass
        2. if ID >= [key]:  inspect
            if ID > [key][value]: pass
            if ID <= [key][value]: Tally this entry, as [key] <= ID <= [value] 
                break out of loop, no longer need to check this ID

Problem 2 approach:
    The map already contains all the ranges in order, now we just need to figure out where the overlaps occur

    Looping through each key, compare the keys and values between range n, following m:

        By the ordered property of the list, n[key] > m[key]

        1. if n[key] > m[value]           ->   This is the start of a new range, i.e. there is a gap between range m and range n
        2. if n[key] <= m[value]          ->   The start of range n is partially in range m
            a. if n[value] > m[value]     ->   The range of m is extended by the [value] of n
            b. if n[value] <= m[value]    ->   range n is simply swallowed up by range m

    By keeping track of the terminal ends of each range it becomes possible to collapse all ranges down into a linear collection of separate ranges

    The size of these ranges can then be tallied for a final answer
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <map>
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


std::map<long unsigned int, long unsigned int>* GetRangesAsMap(std::vector<std::string>* p_InputDataVector)
{
    //Iterate through all lines in the input file and isolate only the ranges
    //Store these in a map of [min] = max

    std::map<long unsigned int, long unsigned int>* p_RangeMap = new std::map<long unsigned int, long unsigned int>;

    for(std::string Line : *p_InputDataVector)
    {
        //Exits early when the linebreak separating the ranges from the inputs is detected
        if(Line.size())
        {
            //Isolate the two numerical values
            std::stringstream StringStream(Line);
            std::string IntAsString;
            std::vector<long unsigned int> HoldInts;
            while(getline(StringStream, IntAsString, '-'))
            {
                HoldInts.emplace_back(std::stoul(IntAsString));
            }
            //store [max] value under [min] key
            //The input has some keys that are identical, we need to account for that
            if(p_RangeMap->find(HoldInts[0]) != p_RangeMap->end())
            {
                //This checks if an already listed value is larger than the listed value and stores it if so
                if((*p_RangeMap)[HoldInts[0]] < HoldInts[1])
                {
                    (*p_RangeMap)[HoldInts[0]] = HoldInts[1];
                }
            }
            else
            {
                //Otherwise, just add the new [key]:[value] pair
                (*p_RangeMap)[HoldInts[0]] = HoldInts[1];
            }
        }
        else
        {
            break;
        }
    }

    return p_RangeMap;
}


std::vector<long unsigned int>* GetIngredientIDs(std::vector<std::string>* p_InputDataVector)
{
    //Second pass of the input file, which is inefficient, but less hassle

    std::vector<long unsigned int>* p_IngredientIDs = new std::vector<long unsigned int>;

    //Do not read in any data until the line break has occured
    bool StartReading {false};

    for(std::string Line : *p_InputDataVector)
    {   
        if(StartReading)
        {
            p_IngredientIDs->emplace_back(std::stoul(Line));
        }
        if(!Line.size())
        {
            StartReading = true;
        }
    }

    return p_IngredientIDs;
}


int SolveProblemOne(int& Timed, std::map<long unsigned int, long unsigned int>* p_RangesMap, std::vector<long unsigned int>* p_VectorOfIngredients)
{  
    if(Timed){std::cout << "Starting Problem One:  ";PrintTimeNow();}

    int Tally {0};

    //Go through each ingredient ID
    for(long unsigned int IngredientID : *p_VectorOfIngredients)
    {
        //Check each range
        for(auto Range : *p_RangesMap)
        {
            //If the ingredient fits the range, tally it, then exit early
            if(IngredientID >= Range.first && IngredientID <= Range.second)
            {
                ++Tally;
                break;
            } 
        }
    }

    if(Timed){std::cout << "Ending Problem One:    ";PrintTimeNow();}

    return Tally;
}


long unsigned int SolveProblemTwo(int& Timed, std::map<long unsigned int, long unsigned int>* p_RangesMap)
{  
    if(Timed){std::cout << "Starting Problem Two:  ";PrintTimeNow();}

    long unsigned int Tally {0};

    //Collapse the ranges and store them here
    std::map<long unsigned int, long unsigned int> CollapsedRanges;

    long unsigned int LastMinimum {0};
    long unsigned int LastMaximum {0};

    //Cut down the ranges in the ranges map
    for(auto Range : *p_RangesMap)
    {
        //Check if this is the start of a new range
        if(Range.first > LastMaximum)
        {
            CollapsedRanges[Range.first] = Range.second;
            LastMinimum = Range.first;
            LastMaximum = Range.second;
        }
        //the miminum of this range is within the last range
        else
        {
            //Check if this extends the range
            if(Range.second > LastMaximum)
            {
                CollapsedRanges[LastMinimum] = Range.second;
                LastMaximum = Range.second;
            }
        }
    }

    //With ranges properly collapsed, just get the size of each gap and tally them
    for(auto Range : CollapsedRanges)
    {
        //Remember that the ranges are >inclusive<, therefore add a +1
        Tally += (Range.second - Range.first + 1);
    }

    if(Timed){std::cout << "Ending Problem Two:    ";PrintTimeNow();}
    
    return Tally;
}


int main(int argc, char* argv[])
{  
    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    if(Timed){PrintTimeNow();}
    
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    //Isolate all ranges from the vector and keep them in a map
    //Some range minimums appear multiple times in the input file, KEEP THAT IN MIND
    std::map<long unsigned int, long unsigned int>* p_RangesMap {GetRangesAsMap(p_InputDataVector)};

    //Isolate all ingredient IDs
    std::vector<long unsigned int>* p_VectorOfIngredients {GetIngredientIDs(p_InputDataVector)};

    std::cout << "Problem One:\n" <<  SolveProblemOne(Timed, p_RangesMap, p_VectorOfIngredients) << "\n";
    std::cout << "Problem Two:\n" <<  SolveProblemTwo(Timed, p_RangesMap) << "\n";


    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;
    delete p_RangesMap;
    p_RangesMap = nullptr;
    delete p_VectorOfIngredients;
    p_VectorOfIngredients = nullptr;

    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}