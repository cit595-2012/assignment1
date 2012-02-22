build:  pieces players
	g++ -o chess Pieces.o Players.o main.cpp 

clean: 
	rm *.o chess

players:  
	g++ -c Players.cpp

pieces:
	g++ -c Pieces.cpp

run: 
	./chess
