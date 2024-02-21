.PHONY:all
all:parse http_server

parse:parse.cc
	g++ -o $@ $^ -std=c++20 -lboost_system -lboost_filesystem

search_server:searcher_debug.cc
	g++ -o $@ $^ -std=c++20 -ljsoncpp


http_server:httpserver.cc
	g++ -o $@ $^ -std=c++20 -ljsoncpp -lpthread

.PHONY:clean
clean:
	rm -f parse http_server
