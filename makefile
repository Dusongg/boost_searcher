parse:parse.cc
	g++ -o $@ $^ -std=c++20 -lboost_system -lboost_filesystem

.PHONY:clean
clean:
	rm -f parse