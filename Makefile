result = $(basename PWD)

all:
	g++ -lpthread -pthread -ldl a.cpp -o $(shell basename $(CURDIR))_debug.o; ./$(shell basename $(CURDIR))_debug.o < sample_input.txt > t_output.txt; uniq t_output.txt > output.txt; rm t_output.txt; cat output.txt;

debug:
	g++ -g -lpthread -pthread -ldl a.cpp -o $(shell basename $(CURDIR))_debug.o; ./$(shell basename $(CURDIR))_debug.o < sample_input.txt > t_output.txt; uniq t_output.txt > output.txt; rm t_output.txt; cat output.txt;
	#sed -i -e 1,4d output.txt; cat output.txt


clean:
	rm *.o; rm *output*.txt; clear
