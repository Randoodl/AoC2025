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
        for(int i_FollowingLine {i_Line + 1}; i_FollowingLine < MaxLine; ++i_FollowingLine)
        {
            std::vector<int> Lines {i_Line, i_FollowingLine};
            long unsigned int Area = GetRectArea((*p_InputDataVector)[i_Line], (*p_InputDataVector)[i_FollowingLine]);
            (*p_AreasMap)[Area].emplace_back(Lines);
        }
    }

    return p_AreasMap;
}


std::vector<int> GetConnectedTileCoordinates(std::string Origin, std::string Projection)
{
    //Take in two tile coordinate strings and turn them into a single vector for processing

    //Xo, Yo, Xp, Yp
    std::vector<int> ExtractedInts {}; 

    for(std::string XY : {Origin, Projection})
    {
        std::stringstream StringStream(XY);
        std::string IntAsString;
        while(getline(StringStream, IntAsString, ','))
        {
            ExtractedInts.emplace_back(std::stoi(IntAsString));
        }
    }

    //Sort them in order from smallest to largest
    if(ExtractedInts[0] > ExtractedInts[2])
    {
        int TempInt {ExtractedInts[0]};
        ExtractedInts[0] = ExtractedInts[2];
        ExtractedInts[2] = TempInt;
    }
    if(ExtractedInts[1] > ExtractedInts[3])
    {
        int TempInt {ExtractedInts[1]};
        ExtractedInts[1] = ExtractedInts[3];
        ExtractedInts[3] = TempInt;
    }

    return ExtractedInts;
}


std::map<int, std::vector<int>>* GetMapOfGreenTiles(std::vector<std::string>* p_InputDataVector)
{
    //Process all the input tile coordinates to generate a map where each X coordinate
    //maps to the collection of Y coordinates

    std::map<int, std::vector<int>>* p_MapOfTiles = new std::map<int, std::vector<int>> {};

    //Setting bounds
    int MaxLine {(int)p_InputDataVector->size()};

    for(int i_Line {0}; i_Line < MaxLine; ++i_Line)
    {
        //Start from this tile
        std::string OriginTile {(*p_InputDataVector)[i_Line]};
        std::string ProjectTile;

        if(i_Line == MaxLine - 1)
        {
            //Connect the last Tile to the first Tile
            ProjectTile  = (*p_InputDataVector)[0];
        }
        else
        {
            //Connect this Tile to the next Tile in the input
            ProjectTile = (*p_InputDataVector)[i_Line + 1];
        }

        //Get the rectangle coordinates from smallest XY corner to largest XY corner
        std::vector<int> ExtractedInts {GetConnectedTileCoordinates(OriginTile, ProjectTile)};

        //March along the Y axis if X is constant
        if(ExtractedInts[0] == ExtractedInts[2])
        {
            for(int StepY {ExtractedInts[1]}; StepY <= ExtractedInts[3]; ++StepY)
            {
                (*p_MapOfTiles)[ExtractedInts[0]].emplace_back(StepY);
            }
        }
        //March along the X axis if Y is constant
        else
        {
            for(int StepX {ExtractedInts[0]}; StepX <= ExtractedInts[2]; ++StepX)
            {
                (*p_MapOfTiles)[StepX].emplace_back(ExtractedInts[1]);
            }
        }
    }

    return p_MapOfTiles;
}


long unsigned int SolveProblemOne(int& Timed, std::map<long unsigned int, std::vector<std::vector<int>>>* p_MapOfPoorLifeChoices)
{
    if(Timed){std::cout << "Starting Problem One:    ";PrintTimeNow();}
    //Look, I made this.
    //I have to live with this.
    //How do you think >I< feel, writing this?

    //Extract the final key 
    auto LastPair {std::prev(p_MapOfPoorLifeChoices->end())};
    long unsigned int LargestArea = LastPair->first;

    if(Timed){std::cout << "Ending Problem One:      ";PrintTimeNow();}
    return LargestArea;
}


long unsigned int SolveProblemTwo(int& Timed, std::map<long unsigned int, std::vector<std::vector<int>>>* p_MapOfPoorLifeChoices, std::map<int, std::vector<int>>* p_MapOfGreenTiles,
                                  std::vector<std::string>* p_InputDataVector)
{
    //And here we come now at the rising cresendo of this opera of inefficiency
    //The magnum opus of making sure my laptop hates me
    //Starting with the largest area, for the points listed that make up the area
    //check if any of the green tiles are within the body of the rect (perimeter is fine)

    if(Timed){std::cout << "Starting Problem Two:    ";PrintTimeNow();}
    
    //Iterate over the map in reverse order
    for(auto IterateRects {p_MapOfPoorLifeChoices->rbegin()}; IterateRects != p_MapOfPoorLifeChoices->rend(); ++IterateRects)
    {
        //For each area, look at each two tiles that make up that area
        for(std::vector<int> PairOfTileLines : IterateRects->second)
        {
            bool Fits {true};

            std::string Origin  {(*p_InputDataVector)[PairOfTileLines[0]]};
            std::string Project {(*p_InputDataVector)[PairOfTileLines[1]]};

            //{Xmin, Ymin, Xmax, Ymax}
            std::vector<int> ExtractedInts {GetConnectedTileCoordinates(Origin, Project)};

            //Check each green tile between (Xmin,Ymin) and (Xmax,Ymax) if it is within the body of the rect that can be constructed from the connected tiles
            //It is ok to cry
            
            //The downright hamfisted loop here basically means:
            //Between Xmin and Xmax exclusive, check for green tiles
            //Exclusive, not inclusive, because we do not care about green tiles in the perimeter
            for(auto IterateGreenTiles {p_MapOfGreenTiles->find(ExtractedInts[0] + 1)}; IterateGreenTiles != p_MapOfGreenTiles->find(ExtractedInts[2] - 1); ++IterateGreenTiles)
            {   
                for(int GreenTileY : IterateGreenTiles->second)
                {
                    if(GreenTileY > ExtractedInts[1] && GreenTileY < ExtractedInts[3])
                    {
                        //There exists a green tile within the body of the rectangle, so it cannot be valid
                        Fits = false;
                        break;
                    }
                }
                
                if(!Fits)
                {
                    //stop checking invalid rectangles
                    break;
                }
            }
            if(Fits)
            {   
                //return the first Area that is able to pass the filtering check
                if(Timed){std::cout << "Ending Problem Two:      ";PrintTimeNow();}
                return IterateRects->first;
            }
        }
    }
    
    return 0;
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

    //This is a map linking all [x] tiles to their collection of available [y] tiles
    //There WILL be around 500 duplicates scattered about the [y] vectors
    //but I am a tired man and simply wish to move on
    std::map<int, std::vector<int>>* p_MapOfGreenTiles {GetMapOfGreenTiles(p_InputDataVector)};

    std::cout << "Problem One:\n" << SolveProblemOne(Timed, p_MapOfPoorLifeChoices) << "\n";
    std::cout << "Problem Two:\n" << SolveProblemTwo(Timed, p_MapOfPoorLifeChoices, p_MapOfGreenTiles, p_InputDataVector) << std::endl;

    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;
    delete p_MapOfPoorLifeChoices;
    p_MapOfPoorLifeChoices = nullptr;
    delete p_MapOfGreenTiles;
    p_MapOfGreenTiles = nullptr;


    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}