# InternalRune
This is code for simulating our internal rune. It uses socket to receive the answer from client.

To compile & run it, you should first install SDL2 on your computer. 
```
sudo apt-get install libsdl2-dev
```
Compile & run:
```
g++ src/main.cpp -I /usr/local/include -lSDL2 -lpthread
./a.out
```
The including path `/usr/local/include` may vary depending on your computer, you may replace it with installation path of SDL2.

You can press Esc to exit the program.
