CXX		  := g++
CXX_FLAGS := -std=c++17 -ggdb

BIN		:= ./bin
SRC		:= ./src
INCLUDE	:= ./lib/include
LIB		:= ./lib/lib/
LIBRARIES	:= -l:libboost_program_options.a
EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	@echo "run called"
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE)  -L $(LIB) ./src/main.cpp ./src/repository.cpp -o $ $(BIN)/$(EXECUTABLE) $(LIBRARIES) 

clean:
	@echo "clean called"
	@rm -rf -d $(BIN)/*