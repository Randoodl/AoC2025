/*
Problem 1 approach:
    First and foremost, the input data has to be processed properly. To this end we shall use a combination of stringstream and my cursed take on regex
    Every line will be represented as a struct of three members:
        1. The string with the intended final configuration of lights
        2. The listed buttons and what lights they toggle
        3. The Joltages
    
    All of these structs will be stored in a vector of structs, allowing to iterate over them easily.

    Now for each struct (read: each line) the following can be done:

        1. Check if the current config matches the target config
            if so, neat, we're done
        2. Otherwise, press each button and note down the resulting config
            The idea being that you build a map of how to get to each config by pressing which buttons
        3. If the resulting config is new, jot it down in an ever propagating map Map[ResultingConfig] = vector<ButtonsPressed>
           
           If the resulting config is NOT new, then don't bother keeping track of it, by virtue of keeping track of each button pressed per cycle,
           each set of button presses to a noted config is BY DESIGN the fastest route.
           Since this sequence of presses presents a config that already has been noticed, we don't need to bother checking it any further for: 

                if      ABADCB leads to config .#.#.#
                and BAD-ABADCB leads to the same config

                then the second route is already longer to begin with (there is a faster way to that config, so don't try and resolve it any further)

        4. Loop back to step one, as an optimisation step we can limit ourselves to new button presses each cycle
           Pressing one button over and again will just keep undoing its action.

    As soon as a target config has been reached for a struct/line, tally the total button presses and track them to answer problem one


Problem 2 approach:
    This is a counting problem, essentialy the problem boils down to:

        x(ButtonA) + y(ButtonB) + z(ButtonC) + ... = {Config}   


    As dumb as this sounds, it's just balancing a chemical equation, optimally
    The real crux is figuring out how to determine these factors

*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <regex>
#include <sstream>
#include <algorithm>


struct ManualLine
{
    //This is a quick and dirty struct to keep all the relevant information per line from the input file
    
    std::string TargetConfig {};                   //[.##.]
    std::vector<std::vector<int>> Buttons {};      //(3) (1,3) (2) (2,3) (0,2) (0,1)
    std::vector<int> Joltages {};                  //{3,5,4,7}
};


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


void PrintInput(std::vector<ManualLine>* p_RegexdInputData)
{
    //This is just to easily visualise the input file for troubleshooting purposes

    for(int i_Object {0}; i_Object < (int)p_RegexdInputData->size(); ++i_Object)
    {
        std::cout << "\n  Config:             " << (*p_RegexdInputData)[i_Object].TargetConfig << std::endl;
        std::cout << "  With Buttons:       ";
        for(auto Buttons : (*p_RegexdInputData)[i_Object].Buttons)
        {
            std::cout << "(";
            for(int i_Button {0}; i_Button < (int)Buttons.size(); ++i_Button)
            {
                if(i_Button != (int)Buttons.size() - 1)
                {
                    std::cout << Buttons[i_Button] << ",";
                }
                else
                {
                    std::cout << Buttons[i_Button];
                }
            }
            std::cout << ")";
        }
        std::cout << "\n  And the Joltages:   {";
        for(int i_Jolt {0}; i_Jolt < (int)(*p_RegexdInputData)[i_Object].Joltages.size(); ++i_Jolt)
        {
            if(i_Jolt != (int)(*p_RegexdInputData)[i_Object].Joltages.size() - 1)
            {
                std::cout << (*p_RegexdInputData)[i_Object].Joltages[i_Jolt] << ",";
            }
            else
            {
                std::cout << (*p_RegexdInputData)[i_Object].Joltages[i_Jolt];
            }
        }
        std::cout << ")\n\n";  
    }
}


std::vector<ManualLine>* GenerateManualLines(int& Timed, std::vector<std::string>* p_InputDataVector)
{
    if(Timed){std::cout<< "Starting processing input file: "; PrintTimeNow();}
    
    //Go through each of the lines in the input and separate its contents into relevant substructures
    //This will be a regex chop shop

    std::vector<ManualLine>* p_ManualLines = new std::vector<ManualLine>;

    //Patterns to isolate
    std::regex Config("\\[([.#]+)\\]");     //Grab bracketed substring containing one or more '.' or '#'
    std::regex Buttons("\\(([^\\s]*)\\)");  //Grab ALL braced substrings without spaces
    std::regex Joltage("\\{([^]*)\\}");     //Grab curly braced substring with anything inside (for this input thtat's ok)

    for(std::string Line : *p_InputDataVector)
    {
        //Holds the result of regex_search
        std::smatch Match;

        //Iterator for iterating over all buttons in the buttons substring
        auto LineBegin = std::sregex_iterator(Line.begin(), Line.end(), Buttons);
        auto LineEnd   = std::sregex_iterator();

        //Setting up some StringStream variables
        std::string SingleIntAsString {};

        //Create a new ManuaLine struct to hold all neatly chopped up information
        ManualLine NewEntry {};

        //Isolate the Target Configuration
        if(std::regex_search(Line, Match, Config))
        {
            //By accessing Match[1], we're snagging the contents of the first capturing group
            //Which is just the string without the braces
            NewEntry.TargetConfig = Match[1];
        }

        //Isolate all the Button and their effects as a nested vector
        if(std::regex_search(Line, Match, Buttons))
        {   
            //Nested vector to hold all button data
            std::vector<std::vector<int>> ButtonsVector {};

            for(auto IterateButtons {LineBegin}; IterateButtons != LineEnd; ++IterateButtons)
            {
                std::vector<int> SingleButtonCombination {};

                //As of here, (*IterateButtons)[1] will point to a substring of just the comma separated numbers
                //Now it is time to start isolating the ints from the string
                //Probably more elegant with regex, but regex scary

                std::stringstream Stream ((*IterateButtons)[1]);
                while(getline(Stream, SingleIntAsString, ','))
                {
                    SingleButtonCombination.emplace_back(std::stoi(SingleIntAsString));
                }
                ButtonsVector.emplace_back(SingleButtonCombination);
            }

            NewEntry.Buttons = ButtonsVector;
        }

        //Isolate the Joltage requirements
        if(std::regex_search(Line, Match, Joltage))
        {
            //Similar to the Buttons extraction, just not nested

            std::vector<int> JoltageVector {};
            std::stringstream Stream (Match[1]);

            while(getline(Stream, SingleIntAsString, ','))
            {
                JoltageVector.emplace_back(std::stoi(SingleIntAsString));
            }

            NewEntry.Joltages = JoltageVector;
        }

        //As of now the ManualLine struct "NewEntry" has all its members populated
        //This will make a fine addition to my collection
        p_ManualLines->emplace_back(NewEntry);
    }

    if(Timed){std::cout<< "Finished processing input file: "; PrintTimeNow();}

    return p_ManualLines;
}


std::string PressButtonLights(std::string OriginalConfig, std::vector<int>& Button)
{
    //Go through the instructions as listed by the button and build a new string with the resulting config

    std::string ReturnString {OriginalConfig};

    for(auto i_String : Button)
    {
        if(ReturnString[i_String] == '.')
        {
            ReturnString[i_String] = '#';
        }
        else
        {
            ReturnString[i_String] = '.';
        }
    }

    return ReturnString;
}


std::map<std::string, std::vector<int>> GenerateNewConfigs(std::map<std::string, std::vector<int>>& MapOfConfigsAndPaths, ManualLine& ProblemLine)
{
    //Take a config and the path it has taken to get here, press new buttons and see what the result is

    //Copy the map to not have button presses in one branch bleed over into another
    std::map<std::string, std::vector<int>> New_MapOfConfigsAndPaths(MapOfConfigsAndPaths);

    for(auto IterateConfigs : MapOfConfigsAndPaths)
    {
        //In order to get to this config, we press these buttons (one POSSIBLE path)
        std::vector<int> PathToConfig {MapOfConfigsAndPaths[IterateConfigs.first]};

        //Now start pressing new buttons
        for(int i_Button {0}; i_Button < (int)ProblemLine.Buttons.size(); ++i_Button)
        {
            //Don't undo last buttons press
            if(i_Button != MapOfConfigsAndPaths[IterateConfigs.first].back())
            {   
                //Press the button, store the resulting config
                std::string NewConfig {PressButtonLights(IterateConfigs.first, ProblemLine.Buttons[i_Button])};

                //See if the new config is truly new
                if(MapOfConfigsAndPaths.find(NewConfig) == MapOfConfigsAndPaths.end())
                {
                    //The resulting config is new! Add the path to get here and store it in the map
                    std::vector<int> UpdatedPath(IterateConfigs.second);
                    UpdatedPath.emplace_back(i_Button);

                    New_MapOfConfigsAndPaths[NewConfig] = UpdatedPath;
                }
            }
        }
    }

    return New_MapOfConfigsAndPaths;
}


int SolveProblemOne(int& Timed, std::vector<ManualLine>* p_RegexdInputData)
{
    //For each line, build a map linking configuration strings to the steps needed to get there
    //Check if the target config has been reached each step, as soon as it does we have >one of the< shortest paths

    if(Timed){std::cout << "Starting Problem One:  "; PrintTimeNow();}

    int TotalPathLengths {0};

    std::map<std::string, std::vector<int>> MapOfConfigsAndPaths {};

    for(int i_Entry {0}; i_Entry < (int)p_RegexdInputData->size(); ++i_Entry)
    {
        //Make sure to start with a clean map
        MapOfConfigsAndPaths.clear();
        
        //Confusing and cumbersome way of setting up the initial map entry
        //Essentially:
        //  The key:     As many '.' as there are characters in the target config
        //  The value:   A path of {-1} to start off with
        MapOfConfigsAndPaths[std::string((*p_RegexdInputData)[i_Entry].TargetConfig.size(), '.')] = {-1};

        //As long as the target config has not been found in the keys of the config and paths map
        while(MapOfConfigsAndPaths.find((*p_RegexdInputData)[i_Entry].TargetConfig) == MapOfConfigsAndPaths.end())
        {
            MapOfConfigsAndPaths = GenerateNewConfigs(MapOfConfigsAndPaths, (*p_RegexdInputData)[i_Entry]);      
        }

        //The config has been found, this is its path with the initial -1 step
        std::vector<int> PathToConfig {MapOfConfigsAndPaths.find((*p_RegexdInputData)[i_Entry].TargetConfig)->second};

        //And this is with the -1 step snapped off
        PathToConfig.erase(PathToConfig.begin());
        
        //Add the path's length to the running total
        TotalPathLengths += (int)PathToConfig.size();
    }

    if(Timed){std::cout << "Ending Problem One:    "; PrintTimeNow();}
    
    return TotalPathLengths;
}


int SolveProblemTwo(int& Timed, std::vector<ManualLine>* p_RegexdInputData)
{
    return -1;
}


int main(int argc, char* argv[])
{  
    //Keep track of if this run should be timed or not, default is False
    int Timed {0};
    if(argc >= 3){Timed = std::stoi(argv[2]);}
    if(Timed){PrintTimeNow();}

    //Debugging toggle
    int Debug {0};
    if(argc >= 4){Debug = std::stoi(argv[3]);}
    
    //The Puzzle Input Data as a POINTER to a vector of strings
    std::vector<std::string>* p_InputDataVector {GetFileData(argv[1])};

    //Processing the input data into neat little objects
    std::vector<ManualLine>* p_RegexdInputData {GenerateManualLines(Timed, p_InputDataVector)};

    if(Debug){PrintInput(p_RegexdInputData);}

    std::cout << "Problem One:\n" << SolveProblemOne(Timed, p_RegexdInputData) << std::endl;
    std::cout << "Problem Two:\n" << SolveProblemTwo(Timed, p_RegexdInputData) << std::endl;

    //Cleaning up
    delete p_InputDataVector;
    p_InputDataVector = nullptr;
    delete p_RegexdInputData;
    p_RegexdInputData = nullptr;

    std::cout << "\nTotal runtime: " <<  1.0 * clock() /CLOCKS_PER_SEC << "s\n";

    return 0;
}