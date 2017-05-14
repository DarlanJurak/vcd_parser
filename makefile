all:	
	clear
	g++ -c source/Signal.cpp
	g++ -c source/Module.cpp
	g++ source/VCD_INTERPRETER.cpp source/Module.cpp source/Signal.cpp -o executable/VCD_INTERPRETER 