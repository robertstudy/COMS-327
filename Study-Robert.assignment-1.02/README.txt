For this project I have fully met the specification that is required in the assignment PDF.
You can move north, south, east, west with corresponding key inputs n, s, e, w respectively.
You can fly around the map using the f input followed by 2 integers within the bounds of the map being -200 to 200 inclusive in both directions.
You can quit the program by hitting the q input key and the program will exit on its own.
Each new map will not be generated until it is visited and corresponding gate locations will match up to previously loaded maps if there were any.
If you move in a direction that is not in the bounds of the world or you input an operation that is invalid, an error message will occur and the same map will be printed again.
I implemented a world_maps_generate method which is used to generate all maps after the origin.
This function uses the manhattan distance to compute the chance of a pokemart or pokecenter spawning. The change of the building spawning decreases the further from the origin map.
The main method uses a switch case nested in a while loop to follow user inputs until the user prompts the program to exit.