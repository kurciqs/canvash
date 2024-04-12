# canvash
discount processing / p5.js in C

## about

can render simple shapes, like p5.js

## usage

### linux

this isn't an app, this is a library. so you have to clone the entire repository.
you can clone it with a command through the shell.
```shell
git clone https://github.com/kurciqs/canvash 
```
once you have done that all that's left is to edit the main.c file to your liking (that is basically where you write your script).
to compile it, you have to run cmake and then make.
```shell
cd canvash # enter the project directory if you haven't yet
cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" . -B release/
cd release/
make
./canvash_dev
```
basically, if you now want to change the script you just have to edit the main.c file and recompile.

### windows
i recommend downloading the repo by clicking the "code" button and the "download zip".
then unzip it and open the directory with the code editor of your liking (i recommend clion (https://www.jetbrains.com/de-de/clion/)) and set it up to compile with the [CMakeLists.txt](CMakeLists.txt) file.
then you can edit the main.c file and hit compile and your program will run :D

if you set CMAKE_BUILD_TYPE=Release you will get much better performance since that will add the -O3 flag to the compiler, which makes the program much faster.

## docs

they will be here one day (maybe)