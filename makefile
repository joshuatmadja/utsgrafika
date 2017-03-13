all:
	g++ -o main main.cpp datastructure/*.cpp -std=c++11

clean:
	rm main *.o
