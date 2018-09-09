LIBS=-L/usr/lib -lboost_system -lboost_thread-mt -lboost_regex

all: server client
	chmod +x client.sh

client: client.o
	c++ $(LIBS) -o client client.o

client.o: client.cpp
	c++ $(LIBS) -c client.cpp

server: common.o server.o
	c++ $(LIBS) -o server server.o common.o

server.o: server.cpp
	c++ $(LIBS) -c server.cpp

common.o: common.cpp common.hpp
	c++ $(LIBS) -c common.cpp

clean:
	rm -f common.o server server.o client client.o
