#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef  _FIVECHESS_h_
#define  _FIVECHESS_h_

#define BOARD_SIZE 20

#define EMPTY 0
#define ME 1
#define OTHER 2

#define TRUE 1
#define FALSE 0

enum Key_e {UP, DOWN, LEFT, RIGHT, QUIT, ENTER, UNKNOW};

int board[BOARD_SIZE][BOARD_SIZE];

void Init();
void showGame(int player, int idx, int idy);
int playChess(int player, int x, int y);
int getInput();
int checkDown(int x, int y);
int checkWin(int x, int y, int player);
void exitGame();

#endif   //_FIVECHESS_h_

