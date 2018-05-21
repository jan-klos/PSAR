CXX=g++
CXXFLAGS=-std=c++11 -W -Wall -g
CXXFLAGS+= $(shell pkg-config --cflags libnl-genl-3.0)
CXXLINK= -pthread $(shell pkg-config --libs libnl-genl-3.0) -lssl -lcrypto

DIR_SRC=src/
DIR_INC=inc/
DIR_OBJ=obj/
DIR_BIN=bin/

SRC=$(shell find $(DIR_SRC) -name "*.cpp" -exec basename {} \;)

OBJ=$(SRC:%.cpp=$(DIR_OBJ)%.o)

EXE=$(DIR_BIN)main

$(EXE): $(OBJ) | $(DIR_BIN)
	$(CXX) -o $@ $^ $(CXXLINK)

$(DIR_OBJ)%.o: $(DIR_SRC)%.cpp | $(DIR_OBJ)
	$(CXX) $(CXXFLAGS) -I$(DIR_INC) -MMD -c -o $@ $<

$(DIR_BIN):
	@mkdir $@

$(DIR_OBJ):
	@mkdir $@

clean:
	@rm -rf $(DIR_BIN)
	@rm -rf $(DIR_OBJ)

debug: $(EXE)
	@cd $(DIR_RUN) && gdb ../$<

.PHONY: clean debug