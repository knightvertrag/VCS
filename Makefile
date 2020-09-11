CXX		  := g++
CXX_FLAGS := -std=c++17 -ggdb

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=
EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	@echo "run called"
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $ ~/imperium/$(BIN)/$(EXECUTABLE) $(LIBRARIES)

clean:
	@echo "clean called"
	@rm -rf -d ~/imperium/$(BIN)/*