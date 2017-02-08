CC = g++ -std=c++11
CPPFLAGS = -g -Wall -Werror -O2
LDFLAGS =

OBJ = TSPSolverGA.o main.o

%.o: %.cpp
		$(CC) $(CPPFLAGS) -c $^ -o $@

main: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o main 
		
clean:
		rm -rf $(OBJ) main

.PHONY: clean
