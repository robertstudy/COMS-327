#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateBorder(char[21][80]);
void printMap(char[21][80]);
void generateTerrain(char[21][80]);
void generateRoads(char[21][80]);
void generateCenter(char[21][80]);
void generateMart(char[21][80]);

int northExitLocation;
int westExitLocation;
int eastExitLocation;
int southExitLocation;

int main(int argc, char *argv[]){

    char map[21][80];
    srand(time(NULL));
    generateTerrain(map);
    generateBorder(map);
    generateRoads(map);
    generateCenter(map);
    generateMart(map);
    printMap(map);
}

void printMap(char map[21][80]){
    int i, j;
    for(i = 0; i < 21; ++i){
        for(j = 0; j < 80; ++j){
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

void generateTerrain(char map[21][80]){
    char shortGrass = '.';
    char tallGrass = ':';
    char water = '~';
    char tree = '^';
    char mountain = '%';
    
    for (int y = 0; y < 21; y++) { 
        for (int x = 0; x < 80; x++) {
            map[y][x] = shortGrass; // Initialize the world with a default terrain to fill missing spaces
        }
    }

    int numRectangles = rand() % 30 + 40; // Random number of rectangles being placed

    for (int i = 0; i < numRectangles; i++) {
        char terrainType;
        int randomSeed = rand() % 5; // Randomly choose terrain type
        switch (randomSeed) {
            case 0:
                terrainType = tallGrass;
                break;
            case 1:
                terrainType = water;
                break;
            case 2:
                terrainType = tree;
                break;
            case 3:
                terrainType = mountain;
                break;
            case 4:
                terrainType = shortGrass;
                break;
        }

        int rectWidth = rand() % 25 + 1; // Randomly generate a rectangle size
        int rectHeight = rand() % 15 + 1;

        int rectX = rand() % (80 - rectWidth); // Randomly choose where to place a rectangle
        int rectY = rand() % (21 - rectHeight);

        for (int y = rectY; y < rectY + rectHeight; y++) { // Place the Rectangles
            for (int x = rectX; x < rectX + rectWidth; x++) {
                map[y][x] = terrainType;
            }
        }
    }
}

void generateBorder(char map[21][80]){
    
    for(int i = 0; i < 21; ++i){
        map[i][0] = '%';
        map[i][79] = '%';
    }
    for(int i = 0; i < 80; ++i){
        map[0][i] = '%';
        map[20][i] = '%';
    }
    
    northExitLocation = (rand() % 78) + 1; // generate random locations
    southExitLocation = (rand() % 78) + 1;
    westExitLocation = (rand() % 19) + 1;
    eastExitLocation = (rand() % 19) + 1;

    map[20][northExitLocation] = '#'; // place exit locations
    map[0][southExitLocation] = '#';
    map[eastExitLocation][79] = '#';
    map[westExitLocation][0] = '#';
}

void generateRoads(char map[21][80]){

    int x = (rand() % 78) + 1;
    int y = (rand() % 19) + 1;
  
    for(int i = 1; i <= x; i++){
        map[westExitLocation][i] = '#';
    }
    for(int i = x; i < 79; i++){
        map[eastExitLocation][i] = '#';
    }
    if(westExitLocation > eastExitLocation){
        for(int i = eastExitLocation; i < westExitLocation; i++){
            map[i][x] = '#';
        }
    }else{
        for(int i = westExitLocation; i < eastExitLocation; i++){
            map[i][x] = '#';
        }
    }

    for(int i = 1; i <= y; i++){
        map[i][southExitLocation] = '#';
    }
    for(int i = y; i < 20; i++){
        map[i][northExitLocation] = '#';
    }
    if(southExitLocation > northExitLocation){
        for(int i = northExitLocation; i < southExitLocation; i++){
            map[y][i] = '#';
        }
    }else{
        for(int i = southExitLocation; i < northExitLocation; i++){
            map[y][i] = '#';
        }
    }
}

void generateCenter(char map[21][80]){
    
    int x = (rand() % 75) + 3;
    int y = (rand() % 15) + 3;

    if (x < 78 && y < 20 && map[y][x] == '#' && map[y-1][x] != '#' && map[y-2][x] != '#' && map[y-1][x-1] != '#' && map[y-2][x-1] != '#') {
        map[y-1][x] = 'C';
        map[y-2][x] = 'C';
        map[y-1][x-1] = 'C';
        map[y-2][x-1] = 'C';
        return;
    } else {
        generateCenter(map);
    }
}

void generateMart(char map[21][80]){

    int x = (rand() % 75) + 3;
    int y = (rand() % 15) + 3;

    if (x < 78 && y < 20 && map[y][x] == '#' && map[y-1][x] != '#' && map[y-2][x] != '#' && map[y-1][x-1] != '#' && map[y-2][x-1] != '#'&& map[y-1][x] != 'C' && map[y-2][x] != 'C' && map[y-1][x-1] != 'C' && map[y-2][x-1] != 'C' ){
        map[y-1][x] = 'M';
        map[y-2][x] = 'M';
        map[y-1][x-1] = 'M';
        map[y-2][x-1] = 'M';
        return;
    } else {
        generateMart(map);
    }
}