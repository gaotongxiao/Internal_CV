# Internal_CV_Client

This is the implementation of Client and a simple demo program, you'll need to use class Client to send your answer to server.

This program can only run on linux.
Compile & Run:
```
g++ -o client Client.cpp
./client SERVER_IP_ADDRESS
```
After successful connection to server, you can enter a digit and press enter for sending.

Enter `-1` or use `Ctrl+C` to exit the program.

**You can only send ONE digit per second, otherwise we'll judge your answer to be false.**

For more details, please refer to the source code.
