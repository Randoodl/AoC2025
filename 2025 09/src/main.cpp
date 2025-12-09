/*
General problem approach:   
    Generate a map of all areas per tiles pair
        i.e.  Map[50] = {{2,5},{11,1}}
    
    By definition, the largest possible area is the last entry in the map
    >This will answer problem 1

    For problem 2, generate all green tiles by just marching through the tile coordinates
    Now, for each area as listed in the previously generated map, starting at the largest area

        1. Get the coordinates for the area 
        2. Check if ANY of the green tiles are within the area (perimeter is ok)
            if so: the perimeter dissects the area and therefore the area cannot be valid, break
        3. first pair of tiles to create an area which is not dissected by green tiles is, by definition, the largest possible area
        4. Weep, because this seems like a daft way to sledgehammer this problem

*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
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


long unsigned int GetRectArea(std::string OriginXY, std::string ProjectedXY)
{
    //Get the area between the two opposite points of a rectangle

    //Stores {Xo, Yo, Xt, Yt}
    std::vector<int> ExtractedInts {};

    for(std::string XY : {OriginXY, ProjectedXY})
    {
        //Isolate the two numerical values
        std::stringstream StringStream(XY);
        std::string IntAsString;
        while(getline(StringStream, IntAsString, ','))
        {
            ExtractedInts.emplace_back(std::stoi(IntAsString));
        }
    }

    //Remember to +1 the area, as the square itself is already 1x1
    long unsigned int TotalArea {(long unsigned int)(std::abs(ExtractedInts[0] - ExtractedInts[2]) + 1) *
                                 (long unsigned int)(std::abs(ExtractedInts[1] - ExtractedInts[3]) + 1) };

    return TotalArea;
}


std::map<long unsigned int, std::vector<std::vector<int>>>* GenerateAreasMap(std::vector<std::string>* p_InputDataVector)
{
    //Create a map where each resulting area links to the coordinate pair that constitutes it
    std::map<long unsigned int, std::vector<std::vector<int>>>* p_AreasMap = new std::map<long unsigned int, std::vector<std::vector<int>>> {};
    
    //Bounds data
    int MaxLine {(int)p_InputDataVector->size()};

    //Get the areas between each coordinate point and the following coordinate points
    //No need to do each to all each iteration since a -> b is the same as a <- b
    for(int i_Line {0}; i_Line < MaxLine; ++i_Line)
    {
        for(int i_FollowingLine {0}; i_FollowingLine < MaxLine; ++i_FollowingLine)
        {
            std::vector<int> Lines {i_Line, i_FollowingLine};
            long unsigned int Area = GetRectArea((*p_InputDataVector)[i_Line], (*p_InputDataVector)[i_FollowingLine]);
            (*p_AreasMap)[Area].emplace_back(Lines);
        }
    }

    return p_AreasMap;
}


long unsigned int SolveProblemOne(std::map<long unsigned int, std::vector<std::vector<int>>>* p_MapOfPoorLifeChoices)
{
    //Look, I made this.
    // have to live with this.
    //How do you think >I< feel, writing this?

    //Extract the final key 
    auto LastPair {std::prev(p_MapOfPoorLifeChoices->end())};
    long unsigned int LargestArea = LastPair->first;

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

    //This is a map of all possible areas as constructed from the tiles given in the input, as indices of the tile in said input
    //It is also the thing that makes me wonder if I should keep trudging on doing AoC puzzles
    std::map<long unsigned int, std::vector<std::vector<int>>>* p_MapOfPoorLifeChoices {GenerateAreasMap(p_InputDataVector)};

    std::cout << "Problem One:\n" << SolveProblemOne(p_MapOfPoorLifeChoices) << "\n";

    

    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;
    delete p_MapOfPoorLifeChoices;
    p_MapOfPoorLifeChoices = nullptr;


    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}