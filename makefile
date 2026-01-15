cpu: main.o alu.o cpu.o
	g++ -o cpu main.o alu.o cpu.o

main.o: main.cpp alu.h cpu.h
	g++ -c main.cpp

alu.o: alu.cpp alu.h
	g++ -c alu.cpp

cpu.o: cpu.cpp cpu.h alu.h
	g++ -c cpu.cpp

clean:
	rm cpu *.o
