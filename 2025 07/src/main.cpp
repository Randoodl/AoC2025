/*
General approach: 

    Iteratively build a map of beam location indices, where each index corresponds to how many ways a beam could get there

    Starting on the first line, isolate the index of S, this will be the first beam index set to 1
    For each line afterwards, loop through each index in the line
    
    if Line[index] == '^'                  <- split the beam

        Line[index - 1] += Line[index]     <- Tally up the different ways a beam could get to that index
        Line[index + 1] += Line[index]
            
        Line[index] = 0;                   <- Because the splitter blocks further traversal down   

    This simulates how the beam splits

    Every time the beam splits, tally it

    Eventually, all that remains is the final beam indices and in how many different ways they could've gotten there
    summing all these up gives the final total of timelines.
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <map>
#include <algorithm>


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


void SimulateBeamSplitting(std::vector<std::string>* p_InputDataVector, int& TotalSplits, long unsigned int& CumulativeBeams, int& Timed)
{
    if(Timed){std::cout << "Simulating beam splitting:    ";PrintTimeNow();}
    
    //Loop through each line, each index in the line and compare active beam indices to found '^' chars

    //Snagging some grid data
    std::string TopLine {(*p_InputDataVector)[0]};
    int GridWidth {(int)TopLine.size()};
    auto it {std::find(TopLine.begin(), TopLine.end(), 'S')};

    //Tracking active beams here, starting with just the index of the 'S' in the TopLine
    std::map<int, unsigned long int> ActiveBeams {{std::distance(TopLine.begin(), it), true}};

    //Simulate the beam traversing down the grid
    for(std::string Line : (*p_InputDataVector))
    {
        for(int i_Line {0}; i_Line < GridWidth; ++i_Line)
        {
            if(Line[i_Line] == '^' && ActiveBeams[i_Line])
            {
                //Beam is splitting to -1 and +1 the current index, leaving the current index blank
                
                ActiveBeams[i_Line - 1] += ActiveBeams[i_Line];
                ActiveBeams[i_Line + 1] += ActiveBeams[i_Line];
                ActiveBeams[i_Line]      = 0;

                //Tally this as a location where the beam is split
                ++TotalSplits;
            }
        }
    }

    //After the simulation has been run, all that is left is a final map with all (active and inactive) beam indices
    //And in how many ways they could've gotten there
    for(auto Beam : ActiveBeams)
    {
        //Filter out inactive indices 
        if(Beam.second) 
        {
            CumulativeBeams += Beam.second;
        }
    }

    if(Timed){std::cout << "Simulation finished:          ";PrintTimeNow();}
}


int main(int argc, char* argv[])
{  
    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    if(Timed){PrintTimeNow();}
    
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    //Keep track of how often the beam is split
    int TotalBeamSplits {0};
    long unsigned int CumalativeBeams {0};

    //Run the beam splitting simulation
    SimulateBeamSplitting(p_InputDataVector, TotalBeamSplits, CumalativeBeams, Timed);

    std::cout << "Problem One:\n" <<  TotalBeamSplits << "\n";
    std::cout << "Problem Two:\n" <<  CumalativeBeams << "\n";



    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;

    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}