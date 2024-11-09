#include <iostream>
#include <ncurses.h>
#include <cstdlib>  
#include <ctime> 

const int NUM_ROWS = 6;
const int NUM_COLS = 7;

char board[NUM_ROWS][NUM_COLS];

void initializeBoard() {
    int i;
    int j;
    for (i = 0; i < NUM_ROWS; i++) { 
        for (j = 0; j < NUM_COLS; j++) {
            board[i][j] = '-';
        }
    }
}

void initializeColors() {
    start_color(); 
    init_pair(1, COLOR_WHITE, COLOR_BLACK); 
    init_pair(2, COLOR_RED, COLOR_BLACK);   
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
}

bool checkWinner(char symbol);
bool findLine(char symbol, int row, int col, int x_dir, int y_dir, int length); 
bool boardFull();
bool columnFull(int col);

void displayBoard(int cursorCol, char currentPlayer) {

    clear();

    printw("Use the arrow keys to move the cursor, use enter to drop the piece.\n");

    int k;
    for (k = 0; k < NUM_COLS; k++) {
        if (k == cursorCol) {
            if (currentPlayer == 'X') { // Show and highlight piece at top of board
                attron(COLOR_PAIR(2));
            } else {
                attron(COLOR_PAIR(3));
            } 
            attron(A_REVERSE); 
        }
        printw("%c ", (k == cursorCol) ? currentPlayer : ' '); 
        if (k == cursorCol) {
            if (currentPlayer == 'X') {
                attroff(COLOR_PAIR(2));
            } else {
                attroff(COLOR_PAIR(3));
            }
            attroff(A_REVERSE);
        }
    } 
    printw("\n");

    int i;
    int j;
    for (i = 0; i < NUM_ROWS; i++) {
        for (j = 0; j < NUM_COLS; j++) {
            if (j == cursorCol) {
                if (currentPlayer == 'X') { 
                    attron(COLOR_PAIR(2));
                } else {
                    attron(COLOR_PAIR(3));
                }  
                attron(A_REVERSE); // Highlight the column where the piece will be dropped
            }
            if (board[i][j] == 'X' || board[i][j] == 'O') {
                if (board[i][j] == 'X') { 
                    attron(COLOR_PAIR(2));
                } else {
                    attron(COLOR_PAIR(3));
                } 
                printw("%c ", board[i][j]);
                if (board[i][j] == 'X') { 
                    attroff(COLOR_PAIR(2));
                } else {
                    attroff(COLOR_PAIR(3));
                } 
            } else {
                printw("%c ", board[i][j]); 
            }
            if (j == cursorCol) {
                if (currentPlayer == 'X') { 
                    attroff(COLOR_PAIR(2));
                } else {
                    attroff(COLOR_PAIR(3));
                }
                attroff(A_REVERSE);
            }
        }
        printw("\n");
    }
    printw("---------------\n");
    refresh();
}

int moveCursor(int currentCol, int maxCol, int direction) {
    int newCol = currentCol + direction;
    if (newCol < 0)
        newCol = 0;
    else if (newCol >= maxCol)
        newCol = maxCol - 1;
    return newCol;
}


void dropPiece(int col, char symbol) {
    int row = 0;
    while (row < NUM_ROWS && board[row][col] == '-') { // adds an "animation" of the piece dropping
        board[row][col] = symbol;
        displayBoard(col, symbol);
        board[row][col] = '-';
        row++;
        napms(100); // falling speed
    }

    if (row > 0){
        board[row - 1][col] = symbol;
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    srand(time(nullptr));

    initializeBoard();
    initializeColors();

    int cursorCol = 0;
    char currentPlayer;
    if (rand() % 2 == 0) { 
        currentPlayer = 'X';
    } else {
        currentPlayer = 'O';
    } 
    bool isGameOver = false;

    while (!isGameOver) {
        displayBoard(cursorCol, currentPlayer);

        int ch = getch();
        switch (ch) {
            case KEY_LEFT:
                cursorCol = moveCursor(cursorCol, NUM_COLS, -1);
                break;
            case KEY_RIGHT:
                cursorCol = moveCursor(cursorCol, NUM_COLS, 1);
                break;
            case '\n': // Enter key
                if(columnFull(cursorCol) == false){
                    dropPiece(cursorCol, currentPlayer);
                    if (checkWinner(currentPlayer) || boardFull()) { // if the board is full or there is a winner, end the game.
                        displayBoard(cursorCol, currentPlayer);
                        isGameOver = true;
                    } else {
                        if (currentPlayer == 'X') {
                            currentPlayer = 'O';
                        } else {
                            currentPlayer = 'X';
                        }
                    }
                }
                break;
            case 'q': 
            case 'Q':
                isGameOver = true;
                break;
        }
    }

    displayBoard(cursorCol, currentPlayer);
    if (checkWinner(currentPlayer)) {
        mvprintw(NUM_ROWS + 2, 0, "Player %c wins! Press 'q' to quit.", currentPlayer);
    } else if(boardFull()) {
        mvprintw(NUM_ROWS + 2, 0, "The board is full. It's a draw! Press 'q' to quit.");
    }
    refresh();

    while (getch() != 'q');

    endwin();

    return 0;
}

bool checkWinner(char symbol) {
    int i;
    int j;
    for (i = 0; i < NUM_ROWS; i++) {
        for (j = 0; j < NUM_COLS; j++) {
            if (board[i][j] == symbol) {
                if (findLine(symbol, i, j, -1, 0, 1) || findLine(symbol, i, j, 0, 1, 1) || findLine(symbol, i, j, -1, 1, 1) || findLine(symbol, i, j, -1, -1, 1)) {  
                    // iterating from the bottom left up, so these coordinates are the ones that haven't been checked yet.   
                    return true;
                }
            }
        }
    }
    return false;
}

bool findLine(char symbol, int row, int col, int x_dir, int y_dir, int length) {
    if (length == 4) {
        return true;
    }

    int new_row = row + x_dir;
    int new_col = col + y_dir;

    if (new_row >= 0 && new_row < NUM_ROWS && new_col >= 0 && new_col < NUM_COLS && board[new_row][new_col] == symbol) {
        return findLine(symbol, new_row, new_col, x_dir, y_dir, length + 1);
    }
    return false;
}

bool boardFull() {
    int i;
    int j;

    for (i = 0; i < NUM_ROWS; i++) {
        for (j = 0; j < NUM_COLS; j++) {
            if (board[i][j] == '-') {
                return false;  
            }
        }
    }
    return true;
}

bool columnFull(int col) { 
    int i;
    for (i = 0; i < NUM_ROWS; i++) {
        if (board[i][col] == '-') {
            return false;
        }
    }
    return true;  
}