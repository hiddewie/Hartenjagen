# Hartenjagen
An AI for the popular card game Hearts

## Compiling
Run the `make` command.

## Running

After compiling, run the executable `bin/Hearts`.

# Adding your own Hearts player
Implement the `IPlayer` interface, defined in `src/iplayer.cpp`.

Add the index to the player selection in `main.cpp`. 

Add your class to the player initialization in `src/game.cpp`. 

Make sure you don't forget to include your own headers.
