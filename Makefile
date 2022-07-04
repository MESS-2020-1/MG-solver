OPTIONS = -Wall -Wfatal-errors -O3

WL_Greedy.exe: WL_Greedy.o WL_Data.o 
	g++ -o WL_Greedy.exe WL_Greedy.o WL_Data.o 

WL_Data.o: WL_Data.cc WL_Data.hh
	g++ $(OPTIONS) -c WL_Data.cc

WL_Greedy.o: WL_Greedy.cc WL_Data.hh
	g++ $(OPTIONS) -c WL_Greedy.cc

clean:
	rm -f WL_Data.o WL_Greedy.o WL_Greedy.exe
