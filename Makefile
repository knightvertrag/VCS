CXX		  := g++
CXX_FLAGS := -std=c++17 -ggdb

BIN		:= ./bin
SRC		:= ./src
INCLUDE	:= ./include 
LIB		:= ./lib/
LIBRARIES	:= -l:libboost_program_options.a -l:libboost_iostreams.a -lz
EXECUTABLE	:= imperium


all: $(BIN)/$(EXECUTABLE)

run: clean all
	@echo "run called"
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp $(SRC)/*.h $(SRC)/cli/*.h ./include/ordered_map.h
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) -I ./src -L $(LIB) $(SRC)/main.cpp  $(SRC)/repository.cpp  $(SRC)/imperium_object.cpp $(SRC)/blob_object.cpp $(SRC)/tree_object.cpp $(SRC)/commit_object.cpp $(SRC)/refs.cpp $(SRC)/lockfile.cpp $(SRC)/index.cpp -o $(BIN)/$(EXECUTABLE) $(LIBRARIES)

tt: 
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) -L $(LIB) ./tests/test.cpp -o ./tests/test $(LIBRARIES) 

clean:
	@echo "clean called"
	@rm -rf -d $(BIN)/*

clean-objects:
	@rm -rf -d $(BIN)/.imperium/objects/*