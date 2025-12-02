/*
Problem 1 approach:
    Generate invalid IDs and see if they fall within one of the given ranges

    First, create a map where each first range digit links to its max range digit
        i.e. 11-22 becomes Map[11] = 22
    Then, start generating invalid IDs by incrementing a counter and doubling the number sequence
        1 > 11
        2 > 22
        .
        .
        .
        123 > 123123
    Test if the invalid ID is greater than a key in the map, and then if it is smaller than the key's value.
    If so, this invalid ID exists within the range.

Problem 2 approach:
    Once again, generate invalid IDs by incrementing a counter and progressively multiply that unit

    11
    111
    .
    .
    22
    222
    .
    .
    3434
    343434

    and so on

    using a map to stop duplicate entries since:
        21     >  21212121
        2121   >  21212121

    Then check those against the ranges again.

    Like problem 1, just more obtuse
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <map>
#include <sstream>
#include <algorithm>


std::vector<std::string>* GetFileData(std::string DataPath)
{
    //Read the input text file line by line and create a vector that can be used during runtime
    
    //Setting up variables to stream data and store it into a vector
    std::vector<std::string>* p_VectorOfLines = new std::vector<std::string>;
    std::ifstream FileStream(DataPath);
    std::string Line;

    //Read the data stream and store lines in Line
    //Since the data stream is just one long line, use the delimiter ',' to store the number pairs
    if(FileStream.is_open())
    {
        while(getline(FileStream, Line, ','))
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


std::map<long unsigned int, long unsigned int>* GetRangeMap(std::vector<std::string>* p_VectorOfRangePairs)
{
    //Take each int-int pair string and turn it into an map[int] = int 
    //This could also be done within GetFileData, but separating this for verbose clarity
    //Since the puzzle input digits get quite large, this needs to be done using long ints

    //Setting up variables to store the long ints in a map
    std::map<long unsigned int, long unsigned int>* p_MapOfRanges = new std::map<long unsigned int, long unsigned int>;

    for(std::string IntPairString : *p_VectorOfRangePairs)
    {
        //Isolating the ints from the string
        std::stringstream StringStream(IntPairString);
        std::string IntAsString;
        std::vector<long unsigned int> VectorOfInts;

        while(getline(StringStream, IntAsString, '-'))
        {
            VectorOfInts.emplace_back(std::stoul(IntAsString));
        }

        //Add the maximum int as a value to the minimum int's key
        (*p_MapOfRanges)[VectorOfInts[0]] = VectorOfInts[1];
    }

    return p_MapOfRanges;
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


int CountDigits(long unsigned int FullNumber)
{
    //Recursively count how many digits are within a number
    //You know. For fun!

    if(!(FullNumber / 10))
    {
        //If there are no more multiples of 10, we've reached the 1-9 range of digits base case
        return 1;
    }
    
    //There's still multiples of 10 left, recursively call CountDigits on FullNumber with a digit stripped off
    return 1 + (CountDigits(FullNumber / 10));
}


int GetMaxRangeNumber(std::map<long unsigned int, long unsigned int>* p_MapOfRanges)
{
    //Figuring out what reasonable max I should set for the increment loop

    //Count the amount of digits in the largest number given in the map of ranges
    //Which is the value of the last key in the map
    //Then divide that number by 2, since we're duplicating the number sequence to get the invalid IDs
    int MaximumDigitsNeeded {CountDigits((*p_MapOfRanges).rbegin()->second) / 2};

    //Quick and dirty char array magic to create a maximum number string , then convert that to a long unsigned int
    //Remember to manually terminate the char array
    char MaxAsString[MaximumDigitsNeeded + 1];
    MaxAsString[MaximumDigitsNeeded] = '\0';
    for(int Digit {0}; Digit < MaximumDigitsNeeded; ++Digit)
    {
        MaxAsString[Digit] = '9';
    }
    int MaxRangeNumber {std::stoi(MaxAsString)};
    
    return MaxRangeNumber;
}


long unsigned int GenerateInvalidID(int HalfNumber)
{
    //Take a number half and duplicate it, generating an invalid ID
    
    //Cast int at string and generate the invalid ID
    std::string InvalidIDString {std::to_string(HalfNumber) + std::to_string(HalfNumber)};

    //Cast back into long unsigned int
    long unsigned int InvalidID {stoul(InvalidIDString)};

    return InvalidID;
}


std::vector<long unsigned int>* GetInvalidIDsOne(std::map<long unsigned int, long unsigned int>* p_MapOfRanges)
{
    //Generate a whole vector of IDs that are invalid
    std::vector<long unsigned int>* p_VectorOfInvalidIDs = new std::vector<long unsigned int>;

    //For good practice, figure out a reasonable maximum number
    //All of this can be replaced by just quickly looking at the input data and plugging in a number
    //but where would the fun in that be? Let's be robust about it
    int MaxIncrement {GetMaxRangeNumber(p_MapOfRanges)};

    //Now build some invalid IDs and check 'em, we can start at 1
    for(int HalfNumber {1}; HalfNumber <= MaxIncrement; ++HalfNumber)
    {
        //Grab invalid ID and add it to vector
        long unsigned int InvalidID {GenerateInvalidID(HalfNumber)};
        p_VectorOfInvalidIDs->emplace_back(InvalidID);
    }

    return p_VectorOfInvalidIDs;
}


long unsigned int SolveProblemOne(int& Timed, std::map<long unsigned int, long unsigned int>* p_MapOfRanges,  std::vector<long unsigned int>* p_VectorOfInvalidIDs)
{
    if(Timed){std::cout << "Starting Problem One:  ";PrintTimeNow();}
    
    long unsigned int SumInvalidIDs {0};

    //Loop through all invalid IDs and check them against the range values
    for(long unsigned int InvalidID : *p_VectorOfInvalidIDs)
    {
        for(auto Ranges : *p_MapOfRanges)
        {
            //Check if an invalid ID falls in any of the ranges
            if(InvalidID >= Ranges.first && InvalidID <= Ranges.second)
            {
                SumInvalidIDs += InvalidID;
                break; //No need to check higher ranges if it already fit
            }
        }        
    }
    
    if(Timed){std::cout << "Ending Problem One:    ";PrintTimeNow();}

    return SumInvalidIDs;
}


std::map<long unsigned int, bool>* GetInvalidIDsTwo(std::map<long unsigned int, long unsigned int>* p_MapOfRanges)
{
    //Generate a whole map of IDs that are invalid
    std::map<long unsigned int, bool>* p_MapOfInvalidIDs = new std::map<long unsigned int, bool>;

    //Essentially already did this for problem 1, but might as well run it again
    int MaximumDigitsAllowed {CountDigits((*p_MapOfRanges).rbegin()->second)};

    //See GetMaxRangeNumber for notes
    char MaxAsString[(MaximumDigitsAllowed / 2) + 1];
    MaxAsString[MaximumDigitsAllowed / 2] = '\0';
    for(int Digit {0}; Digit < (MaximumDigitsAllowed / 2); ++Digit)
    {
        MaxAsString[Digit] = '9';
    }
    long unsigned int MaxRangeNumber {std::stoul(MaxAsString)};

    //Time to start churnin' out those invalid IDs
    for(long unsigned int NumberSegment {1}; NumberSegment <= MaxRangeNumber; ++NumberSegment)
    {
        std::string SegmentAsString {std::to_string(NumberSegment)};

        //Start building from an empty string
        std::string StringToAddAsNumber {SegmentAsString};

        while(StringToAddAsNumber.size() < (long unsigned int)MaximumDigitsAllowed)
        {
            //Keep adding a segment to the string
            StringToAddAsNumber += SegmentAsString;

            //cast it back into long unsigned int
            long unsigned int AddNumber {std::stoul(StringToAddAsNumber)};

            //Add it to the map with a dummy bool value
            (*p_MapOfInvalidIDs)[AddNumber] = true;
        }
    }

    return p_MapOfInvalidIDs;
}


long unsigned int SolveProblemTwo(int& Timed, std::map<long unsigned int, long unsigned int>* p_MapOfRanges,  std::map<long unsigned int, bool>* p_MapOfInvalidIDs)
{
    if(Timed){std::cout << "Starting Problem Two:  ";PrintTimeNow();}
    
    long unsigned int SumInvalidIDs {0};

    //Loop through all invalid IDs and check them against the range values
    for(auto InvalidIDPair : *p_MapOfInvalidIDs)
    {
        for(auto Ranges : *p_MapOfRanges)
        {
            //Check if an invalid ID falls in any of the ranges
            if(InvalidIDPair.first >= Ranges.first && InvalidIDPair.first <= Ranges.second)
            {
                SumInvalidIDs += InvalidIDPair.first;
                break; //No need to check higher ranges if it already fit
            }
        }        
    }
    
    if(Timed){std::cout << "Ending Problem Two:    ";PrintTimeNow();}

    return SumInvalidIDs;
}


int main(int argc, char* argv[])
{
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    if(Timed){PrintTimeNow();}

    //Changing the puzzle input from strings to a map of ints
    std::map<long unsigned int, long unsigned int>* p_MapOfRanges {GetRangeMap(p_InputDataVector)};

    //All invalid IDs for Problem 1
    std::vector<long unsigned int>* p_VectorOfInvalidIDsOne {GetInvalidIDsOne(p_MapOfRanges)};

    //All invalid IDs for Problem 2, as a map to stop duplicate entries
    std::map<long unsigned int, bool>* p_VectorOfInvalidIDsTwo {GetInvalidIDsTwo(p_MapOfRanges)}; 

    std::cout << "Problem One:\n" << SolveProblemOne(Timed, p_MapOfRanges, p_VectorOfInvalidIDsOne) << "\n";
    std::cout << "Problem Two:\n" << SolveProblemTwo(Timed, p_MapOfRanges, p_VectorOfInvalidIDsTwo) << "\n";
    
    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;
    delete p_MapOfRanges;
    p_MapOfRanges = nullptr;
    delete p_VectorOfInvalidIDsOne;
    p_VectorOfInvalidIDsOne = nullptr;
    delete p_VectorOfInvalidIDsTwo;
    p_VectorOfInvalidIDsTwo = nullptr;

    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}