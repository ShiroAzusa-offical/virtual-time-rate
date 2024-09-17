**This project is be use to adjust the speed of game or other programs main loop speed**

**note:only avaliable on Linux**

How this works:

I wrapped many syscalls such as sleep() usleep() nanosleep() and clock_gettime(),especially the clock_gettime().This function always be used to calculde the delay of every two ticks.by add a layer to multiply the return value,we can change the speed of game freely;

**How to build:**

just run the ```make``` command

**How to use:**

type ```LD_PRELOAD=/path/to/libvtime.so your_program``` to launch the program;

use ```setration``` program to adjust the ration;

For example:

I want to multiply the speed of minecraft

```LD_PRELOAD=./libvtime.so startminecraft.sh``` this command will start the game

note: please generate the startminecraft.sh in your gamelauncher

```./setration 2``` this command will change th e speed to 2x

```./setration 3/2``` this command will change the speed to 1.5x

```./setration 1/2``` this command will change the speed to 0.5x

**kowning issue:**

I only tested this codes in minecraft,and if you don't load the world with the ration of 1,you are unable to load the game.the save world process have the same problem,It just simply stuck here and what you can do is just kill the whole process.I suggest you wait for the autosave of minecraft then just kill the game process to exit the game;

**and I suggest you test the program first to ensure this tool will not damage the critical function**
