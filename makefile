all:
	g++ -o Main Main.cpp datastructure/*.cpp -std=c++0x

clean:
	rm main *.o
