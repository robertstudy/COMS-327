#include <stdio.h>
#include <stdlib.h>

int board[5][5];
void printTour(int[25]);
int canMove(int, int, int[25], int, int);
void createTour(int, int, int, int[25]);

int main(int argc, char *argv[]){
    
    int tour[25];
    int x, y;

    int tileNumber = 1;
    for(x = 0; x < 5; x++){
        for(y = 0; y < 5; y++){
            board[x][y] = tileNumber;
            tileNumber++;
        }
    }
    tileNumber = 0;
    for(x = 0; x < 5; x++){
        for(y = 0; y < 5; y++){
            tour[tileNumber] = board[x][y];
            createTour(x, y, tileNumber, tour);
            tileNumber = 0;
        }
    }
}

void printTour(int tour[25]){
    int i;
    for(i = 0; i < 25; i++){
        if(i == 24){
            printf("%d\n" , tour[i]);
            
        }else{
             printf("%d, " , tour[i]);
        }
    }
}

void createTour(int x, int y, int tilesVisited, int tour[25]){
    tilesVisited++;
    if(tilesVisited == 25){
        printTour(tour);
        return;
    }
    if(canMove(x + 1, y + 2, tour, board[x + 1][ y + 2], tilesVisited) == 0){
        tour[tilesVisited] = board[x + 1][ y + 2];
        createTour(x + 1, y + 2,tilesVisited ,tour);
    }
    if(canMove(x + 1, y - 2, tour, board[x + 1][ y - 2], tilesVisited) == 0){
        tour[tilesVisited] = board[x + 1][ y - 2];
        createTour(x + 1, y - 2,tilesVisited ,tour);
    }
     if(canMove(x - 1, y + 2, tour, board[x - 1][ y + 2], tilesVisited) == 0){
        tour[tilesVisited] = board[x - 1][ y + 2];
        createTour(x - 1, y + 2,tilesVisited ,tour);
    }
    if(canMove(x - 1, y - 2, tour, board[x - 1][ y - 2], tilesVisited) == 0){
        tour[tilesVisited] = board[x - 1][ y - 2];
        createTour(x - 1, y - 2,tilesVisited ,tour);
    }
    if(canMove(x + 2, y + 1, tour, board[x + 2][ y + 1], tilesVisited) == 0){
        tour[tilesVisited] = board[x + 2][ y + 1];
        createTour(x + 2, y + 1,tilesVisited ,tour);
    }
     if(canMove(x + 2, y - 1, tour, board[x + 2][ y - 1], tilesVisited) == 0){
        tour[tilesVisited] = board[x + 2][ y - 1];
        createTour(x + 2, y - 1,tilesVisited ,tour);
    }
    if(canMove(x - 2, y + 1, tour, board[x - 2][ y + 1], tilesVisited) == 0){
        tour[tilesVisited] = board[x - 2][ y + 1];
        createTour(x - 2, y + 1,tilesVisited ,tour);
    }
     if(canMove(x - 2, y - 1, tour, board[x - 2][ y - 1], tilesVisited) == 0){
        tour[tilesVisited] = board[x - 2][ y - 1];
        createTour(x - 2, y - 1,tilesVisited ,tour);
    }
}

int canMove(int x, int y, int tour[25] , int tileNumber, int tilesVisited){ // 0 is true,  1 is false

    if(x < 5 && x >= 0 && y >= 0 && y < 5){
        int i;
        for(i = 0; i < tilesVisited; ++i){
            if(tileNumber == tour[i]){
                return 1;
            }
        }
        return 0;
    }else{
        return 1;
    }
}