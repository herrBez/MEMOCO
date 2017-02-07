CC = g++
CPPFLAGS = -g -Wall -Werror -O2 -std=c++11
LDFLAGS = -DVERBOSE



OBJ = instance_generator.o 

%.o: %.cpp
		$(CC) $(CPPFLAGS) -c $^ -o $@

main: $(OBJ)	
		$(CC) $(CPPFLAGS) $(OBJ) -o instance_generator

clean:
		rm -rf $(OBJ)

.PHONY: clean
