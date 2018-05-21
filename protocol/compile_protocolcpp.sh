g++ -c -fPIC protocol.cpp -o protocol.o;
g++ -shared -Wl,-soname,libprotocol.so -o libprotocol.so protocol.o
