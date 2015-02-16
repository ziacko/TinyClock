all: ./
	g++ -std=c++11 -DSO -w -g -fpermissive -o ./TinyClockTest Example/Example.cpp -I./include/ 2> errors.txt
