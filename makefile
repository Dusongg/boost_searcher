.PHONY:all
all:parse search_server

search_server:server.cc
	g++ -o $@ $^ -std=c++20 -ljsoncpp

parse:parse.cc
	g++ -o $@ $^ -std=c++20 -lboost_system -lboost_filesystem

.PHONY:clean
clean:
	rm -f parse search_server