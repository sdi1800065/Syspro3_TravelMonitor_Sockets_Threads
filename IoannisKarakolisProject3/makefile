all: monitorServer travelMonitorClient
monitorServer:main.o HashModule.o skiplist.o bloomfilter.o mainrelated.o
	g++ --std=c++11  main.o HashModule.o skiplist.o bloomfilter.o mainrelated.o -o monitorServer -lpthread
travelMonitorClient:newmain.o HashModule.o skiplist.o bloomfilter.o mainrelated.o
	g++ --std=c++11  newmain.o HashModule.o skiplist.o bloomfilter.o mainrelated.o -o travelMonitorClient
newmain.o :newmain.cpp  modules.h 
	g++ --std=c++11 -c newmain.cpp
main.o :main.cpp  modules.h 
	g++ --std=c++11 -c main.cpp -lpthread
HashModule.o :HashModule.cpp modules.h
	g++ --std=c++11 -c HashModule.cpp
skiplist.o :skiplist.cpp modules.h
	g++ --std=c++11 -c skiplist.cpp
bloomfilter.o :bloomfilter.cpp modules.h
	g++ --std=c++11 -c bloomfilter.cpp
mainrelated.o :mainrelated.cpp modules.h
	g++ --std=c++11 -c mainrelated.cpp
clean:
	rm monitorServer travelMonitorClient newmain.o main.o HashModule.o skiplist.o bloomfilter.o mainrelated.o log*
