#General variables
CXX = g++
SOURCE = ./src/main.cpp
OUTPUT = ./AoC

#Compiler flags
CXXFLAGS = -std=c++20 -Wall -Werror -Wformat -O2

#Compile rule
$(OUTPUT): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^ 

clean:
	rm $(OUTPUT)

.PHONY:
	clean