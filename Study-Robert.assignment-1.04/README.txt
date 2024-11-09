For this project I have fully met the specification that is required in the assignment 1.04 PDF.
The player pc is always placed on a road.
The hikers and rivals use the pathfind method navigate to the player.
The Pacers, Wanderers, and Explorers move according to their respective movement criteria.
The Sentry does not move.
My generateTrainers method takes numtrainers takes an integer count of the number of trainers to scatter around and there is a default number of trainers to place on the map when the switch is not present.
My spawnTrainers method initializes the characters on the map, inserts them into the queue, and determines initial move for Pacers, Wanderers, and Explorers
My moveTrainers method checks each valid adjacent square to find the best path for hikers and rivals, or follows the respective movement criteria for the other characters.
moveTrainers also adds each character into the queue after each move
Each NPCs turn is determined by the priority queue.
generateTrainers (which calls to spawnTrainers) is included in main and moveTrainers is in an infinite loop (in main) which moves the characters and prints the map every 1/4 second.


