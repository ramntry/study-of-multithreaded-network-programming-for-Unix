TARGETS = server/testServer client/testClient

all: ${TARGETS}

server/testServer: server/testServer.o server/main.o
	g++ -pthread -g -O2 $^ -lpthread ${LD_LIBRARY_PATH}/libboost_system.a ${LD_LIBRARY_PATH}/libboost_thread.a -o $@ 

server/testServer.o: server/testServer.cpp server/testServer.hpp
	g++ -pthread -g -O2 -c -lpthread server/testServer.cpp -o $@

server/main.o: server/main.cpp
	g++ -pthread -g -O2 -c -lpthread $^ -o $@

client/testClient: client/testClient.o client/main.o
	g++ -pthread -g -O2 $^ -o $@ -lpthread ${LD_LIBRARY_PATH}/libboost_system.a ${LD_LIBRARY_PATH}/libboost_thread.a

client/testClient.o: client/testClient.cpp client/testClient.hpp
	g++ -pthread -g -O2 -c client/testClient.cpp -o $@

client/main.o: client/main.cpp
	g++ -pthread -g -O2 -c $^ -o $@

clean:
	rm server/*.o
	rm client/*.o
	rm ${TARGETS}

