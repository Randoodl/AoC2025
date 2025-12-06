/*
General approach:
    The input can be cut into 'chunks' using the operator symbol as a delimiter
    When considering the test case, it can be chunked as such:

        |123 |328 | 51 |64 
        | 45 |64  |387 |23 
        |  6 |98  |215 |314
        |*   |+   |*   |+  
    
    While reading and chunking the information in this manner, we can process the numbers in each chunk for both
    problem 1 and two at the same time

    problem 1:  
        Read line by line, index by index
    problem 2: 
        Read index by index, line by line
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


int GetVectorSum(std::vector<int>& VectorOfInts)
{
    int Total {0};

    for(int Element : VectorOfInts)
    {
        Total += Element;
    }

    return Total;
}


long unsigned int GetVectorProduct(std::vector<int>& VectorOfInts)
{
    long unsigned int Total {(long unsigned int)VectorOfInts[0]};
    int VectorMaxIndex {(int)VectorOfInts.size()};

    for(int i_Element {1}; i_Element < VectorMaxIndex; ++i_Element)
    {
        Total *= VectorOfInts[i_Element];
    }

    return Total;
}


int GetCleanNumber(std::string& LineFromInput, int NumberRoot, int NumberEnd)
{
    //Isolate a number from a given string segment in a line

    int TenFolder {1};    
    int IsolatedNumber {0};
    
    //Read the number from right to left, adding each multiple of ten to a running total
    for(int i_NumberInString {NumberEnd}; i_NumberInString >= NumberRoot; --i_NumberInString)
    {
        //Disregard everything that isn't a number, which should be only spaces
        if(LineFromInput[i_NumberInString] != ' ')
        {   
            IsolatedNumber += (LineFromInput[i_NumberInString] - '0') * TenFolder;
            TenFolder *= 10;
        }
    }

    return IsolatedNumber;
}


int GetCephalodNumber(std::vector<std::string>* p_InputDataVector, int i_NumberInString, int LastLineIndex)
{
    //Very similar to GetCleanNumber, just adapted to read index by index, line by line

    int TenFolder {1};    
    int IsolatedNumber {0};

    //Read numbers from bottom to top, adding each multiple of ten to a running total
    for(int i_LineInInput {LastLineIndex - 1}; i_LineInInput >= 0; --i_LineInInput)
    {
        //Disregard everything that isn't a number, which should be only spaces
        if((*p_InputDataVector)[i_LineInInput][i_NumberInString] != ' ')
        {   
            IsolatedNumber += ((*p_InputDataVector)[i_LineInInput][i_NumberInString] - '0') * TenFolder;
            TenFolder *= 10;
        }
    }

    return IsolatedNumber;
}


void ProcessNumberBlock(char OperatorSymbol, std::vector<int>& VectorOfNumbers, long unsigned int& RunningTotal)
{
    //Either sum or multiply an entire vector and add it to a running total

    if(OperatorSymbol == '+')
    {
        RunningTotal += GetVectorSum(VectorOfNumbers);
    }
    else if(OperatorSymbol == '*') //Doing this explicitly
    {
        RunningTotal += GetVectorProduct(VectorOfNumbers);
    }
}


std::vector<long unsigned int> ReadNumbers(int& Timed, std::vector<std::string>* p_InputDataVector)
{
    if(Timed){std::cout << "Starting combined Problem One and Two:    ";PrintTimeNow();}

    //Based on the index of the operator in the last line, the entire input can be 'chunked'
    //The numbers in such a chunk can then be processed and summed to a running total

    long unsigned int RunningTotalOne {0};
    long unsigned int RunningTotalTwo {0};

    //This is the index of the line that contains all the operator symbols
    int LastLineIndex {(int)p_InputDataVector->size() - 1};

    //The line of operators will be used to slice the rest of the input into chunks
    std::string OperatorsLine {(*p_InputDataVector)[LastLineIndex]};
    int LineLength {(int)OperatorsLine.size()};
    int NumberIndexRoot {0};

    //Go through each index in the operator line
    for(int i_Line {0}; i_Line < LineLength; ++i_Line)
    {
        //At the current index there is either a + or *
        if(OperatorsLine[i_Line] != ' ')
        {
            //This is relevant to figure out the bounds of each number as a string, within the line in the input file
            NumberIndexRoot = i_Line;
            int NumberLength {0};

            //These numbers will be summed or multiplied
            std::vector<int> IsolatedNumbersOne {};
            std::vector<int> IsolatedNumbersTwo {};

            //Find the length of the number
            for(int i_Number {NumberIndexRoot + 1}; i_Number < LineLength; ++i_Number)
            {
                if(OperatorsLine[i_Number] == ' ')
                {
                    ++NumberLength;
                }
                else
                {
                    break;
                }
            }

            //As of here, we have the indices of
            //  where each number begins:  NumberIndexRoot
            //  where each number ends:    i_Line + NumberLength    (alternatively, where the >next< number begins)
            
            //Process the numbers for this block for problem 1
            //Go through each line and isolate numbers between the index of where the number starts in the string and where it ends
            for(int LineInInput {0}; LineInInput < LastLineIndex; ++LineInInput)
            {   
                int IsolatedNumberOne {GetCleanNumber((*p_InputDataVector)[LineInInput], NumberIndexRoot, (i_Line + NumberLength))};
                if(IsolatedNumberOne) //Multiplying by 0 might negate some terms
                {
                    IsolatedNumbersOne.emplace_back(IsolatedNumberOne);
                }
            }

            //Process the numbers in this block for problem 2
            //Go through each index of where a number is present and isolate the number across several input lines
            for(int i_NumberInString {NumberIndexRoot}; i_NumberInString <= (i_Line + NumberLength); ++i_NumberInString)
            {
                int IsolatedNumberTwo {GetCephalodNumber(p_InputDataVector, i_NumberInString, LastLineIndex)};
                if(IsolatedNumberTwo) //Multiplying by 0 might negate some terms
                {
                    IsolatedNumbersTwo.emplace_back(IsolatedNumberTwo);
                }
            }

            //Now determine whether to sum or multiply and add it to a running total
            ProcessNumberBlock(OperatorsLine[i_Line], IsolatedNumbersOne, RunningTotalOne);
            ProcessNumberBlock(OperatorsLine[i_Line], IsolatedNumbersTwo, RunningTotalTwo);
        }
    }

    std::vector<long unsigned int> ReturnTotals {RunningTotalOne, RunningTotalTwo};

    if(Timed){std::cout << "Ending combined Problem One and Two:      ";PrintTimeNow();}

    return ReturnTotals;
}


int main(int argc, char* argv[])
{  
    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    if(Timed){PrintTimeNow();}
    
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    //Process the whole input file, chunking it into blocks as delimited by the operator in the last line of the input file
    //Whenever considering such a chunk, process the numbers for both problem 1 as well as problem 2 
    std::vector<long unsigned int> BothProblemAnswers {ReadNumbers(Timed, p_InputDataVector)};

    std::cout << "Problem One:\n" << BothProblemAnswers[0] << std::endl;
    std::cout << "Problem Two:\n" << BothProblemAnswers[1] << std::endl;

    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;

    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}