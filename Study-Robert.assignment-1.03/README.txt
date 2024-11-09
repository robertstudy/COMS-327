For this project I have fully met the specification that is required in the assignment 1.03 PDF.
There is always a player character generated on a path but not on a gate or border path type.
After each terrain map is printed, 2 distance maps will be printed, one for the Rival and one for the Hiker trainer types.
I used the same priority queue that was given to us in the "dijkstra_path" to calculate the distance maps for each trainer.
I store the terrain move cost for each trainer type in a 2d array called moveCost.
The dijkstra_playerToNpc algorithm I wrote uses the map, character location, trainer type, a to determine how distance map is generated.
This assignment works with my solution from assignment 1.02 where after you travel to a new map, 2 corelating distance maps will be traveled as well.