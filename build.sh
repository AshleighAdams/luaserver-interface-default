g++ -fPIC -std=c++11 -shared ./src/*.cpp -o luaserver-interface-default.so -lrt -lstdc++ -lboost_thread -lboost_system
