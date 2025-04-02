# CXX Make variable for compiler
CC=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CCFLAGS=-std=c++11 -Wall -g3 -c

# object files
OBJS = receiveAndBufferRequests.o processRequests.o main.o 

# Program name
PROGRAM = processrequests

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -pthread -o $(PROGRAM) $(OBJS)

main.o : main.cpp
	$(CC) $(CCFLAGS) main.cpp
	
receiveAndBufferRequests.o: receiveAndBufferRequests.cpp receiveAndBufferRequests.h
	$(CC) $(CCFLAGS) receiveAndBufferRequests.cpp
    
processRequests.o: processRequests.cpp processRequests.h
	$(CC) $(CCFLAGS) processRequests.cpp

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f *.o *~ $(PROGRAM)

