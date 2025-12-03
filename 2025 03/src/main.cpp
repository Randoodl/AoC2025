/*
General approach for both problems:
    Given an input for maximum number digit lenght (i.e. 4), start scanning number strings for the largest multiple of tens
    working your way backwards to isolate the leftmost highest multiple
    the 'leftmost' part is important, since this gives the most area remaining to look for smaller multiples

    The sequence 6564327 with 4 digits as an example

    find the largest 1000, 
        scan [6564]327 (last three digits not relevant, since these cannot contain a full thousand)
            largest leftmost digit is 6 at index 0
    find largest 100, start at previous largest multiple's index + 1, since we do not want to count the same index twice 
        scan 6[5643]27 (last two digits not relevant now, since these cannot contain a full hundred)
            largest leftmost digit is 6 at index 2
    find largest 10, again move the origin index by previous index + 1
        scan 656[432]7 (last digit cannot contain a full ten)
            largest leftmost digit is 4 at index 3
    find largest 1
        scan 6564[327]
            largest leftmost digit is 7 at index 6

    combining those all, it is clear that the largest possible number out of the sequence for 4 digits is 6647

    The above can be done iteratively using a loop that starts with the needed 10eN, where N = (digits required - 1)
    Summing up the Largest Multiple * 10eN for each iteration.

*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <cmath>


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


int* GetArrayOfBatteryNumbers(std::string& BatteryString, int NumberOfBatteries)
{
    //Turn the string of numbers into an array of ints

    //Set aside the array memory
    int* p_BatteryArray = new int[NumberOfBatteries];

    for(int Index {0}; Index < NumberOfBatteries; ++Index)
    {
        //convert the char to an int by subtracting '0' ASCII magic
        p_BatteryArray[Index] = BatteryString[Index] - '0';
    }

    return p_BatteryArray;
}


long unsigned int GetHighestNumber(int* p_BatteryArray, int NumberOfBatteries, int NumberOfDigits)
{
    //Loop through each multiple of tens in descending order
    //i.e. find the largest thousand first, then the largest hundred, largest ten, largest one

    long unsigned int LargestNumber {0};

    //This is to keeep track where the leftmost point in the number array is, which updates after finding 
    //each largest 10eN (stops scanning to the left of the largest 10eN)
    int i_LeftMostRoot {0};

    //Start with the largest needed multiple, the power of 10 is the amount of needed digits - 1
    //i.e. "2 digits" means highest 10e1 + highest 10e0
    for(int PowerOfTen {NumberOfDigits - 1}; PowerOfTen >= 0; --PowerOfTen)
    {
        //Check the array from right to left, starting at the last number where a multiple can still occur
        //no need to check for the largest 10e3 in ....321
        
        //Sum all 10eN..10e0 here
        long unsigned int LargestMultiple   {0};

        //Temp value to link back to i_LeftMostRoot
        int i_LargestMultiple {0};

        for(int i_IntArray {(NumberOfBatteries - 1) - PowerOfTen}; i_IntArray >= i_LeftMostRoot; --i_IntArray)
        {
            //Need to cast this as a long unsigned int to deal with the large numbers for problem 2
            if((long unsigned int)p_BatteryArray[i_IntArray] >= LargestMultiple)
            {
                //Single out the leftmost largest multiple
                LargestMultiple = p_BatteryArray[i_IntArray];
                i_LargestMultiple = i_IntArray;
            }
        }

        LargestNumber += (LargestMultiple * std::pow(10, PowerOfTen));
        i_LeftMostRoot = i_LargestMultiple + 1;
    }

    return LargestNumber;
}


int SolveProblemOne(int& Timed, std::vector<std::string>* p_InputDataVector, int AmountOfDigits)
{
    if(Timed){std::cout << "Starting Problem One:  ";PrintTimeNow();}

    int BatteryTotals {0};

    //Only calculate the max line size once, then pass it around
    //Sure, you can input this as just a number, but this is more fun
    int NumberOfBatteries {(int)(*p_InputDataVector)[0].size()};

    for(std::string Line : *p_InputDataVector)
    {
        //Turn string into an array of ints
        int* p_BatteryArray {GetArrayOfBatteryNumbers(Line, NumberOfBatteries)};

        //Extract the largest number out of the currently checking array and sum it
        BatteryTotals += GetHighestNumber(p_BatteryArray, NumberOfBatteries, AmountOfDigits);

        //Cleaning up
        delete[] p_BatteryArray;
        p_BatteryArray = nullptr;
    }

    if(Timed){std::cout << "Ending Problem One:    ";PrintTimeNow();}

    return BatteryTotals;
}


long unsigned int SolveProblemTwo(int& Timed, std::vector<std::string>* p_InputDataVector, int AmountOfDigits)
{
    //The same as SolveProblemOne
    //just changed int to long unsigned int to allow for those horrificly large numbers
    if(Timed){std::cout << "Starting Problem Two:  ";PrintTimeNow();}

    long unsigned int BatteryTotals {0};

    int NumberOfBatteries {(int)(*p_InputDataVector)[0].size()};

    for(std::string Line : *p_InputDataVector)
    {
        int* p_BatteryArray {GetArrayOfBatteryNumbers(Line, NumberOfBatteries)};
        BatteryTotals += GetHighestNumber(p_BatteryArray, NumberOfBatteries, AmountOfDigits);
        delete[] p_BatteryArray;
        p_BatteryArray = nullptr;
    }

    if(Timed){std::cout << "Ending Problem Two:    ";PrintTimeNow();}

    return BatteryTotals;
}


int main(int argc, char* argv[])
{  
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    if(Timed){PrintTimeNow();}
    
    std::cout << "Problem One:\n" <<  SolveProblemOne(Timed, p_InputDataVector, 2) << "\n";
    std::cout << "Problem Two:\n" <<  SolveProblemTwo(Timed, p_InputDataVector, 12) << "\n";
   
    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;

    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}