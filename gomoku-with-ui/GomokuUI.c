#include "GomokuUI.h"

// 初始化棋盘界面
void Init()
{
    memset(board, 0, sizeof(board));
        
    printf("\033[2J");      // 清屏
    system("stty -icanon"); // 关闭缓冲
    system("stty -echo");   // 关闭回显
    printf("\033[?25l");    // 隐藏鼠标
    fflush(stdout);         // 刷新
}

// 显示当前棋盘局面
void showGame(int player, int idx, int idy)
{
    printf("\033[1;1H"); // 定位到第一行第一列
    fflush(stdout);

    int i, j;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        printf("    ");
        for (j = 0; j < BOARD_SIZE; j++)
        {
            // 光标位置
            if (i == idy && j == idx)
            {
                switch (board[i][j])
                {
                    case EMPTY: 
                        printf(" \033[42m+\033[0m"); 
                        break;
                    case OTHER: 
                        printf(" \033[42mO\033[0m"); 
                        break;
                    case ME: 
                        printf(" \033[42mX\033[0m"); 
                        break;
                }
            }
           
            // 其他位置
            else
            {
                switch (board[i][j])
                {
                    case EMPTY: 
                        printf(" +"); 
                        break;
                    case OTHER: 
                        printf(" \033[33mO\033[0m"); 
                        break;
                    case ME: 
                        printf(" \033[36mX\033[0m"); 
                        break;
                }
            }
        }
        putchar('\n');
    }
    putchar('\n');
    printf("\033[1;35m%s\033[0m's Turn", player == OTHER ? "Human(O)":"AI(X)");
    fflush(stdout);
}

// 如果当前位置可以落子，则落子
int playChess(int player, int x, int y)
{
    // 检查是否可以落子
    if (checkDown(x, y) == 1)
    {
    	board[y][x] = player;
    	return 1;
    }
    return 0;
}

// 获取用户输入
int getInput()
{
    int key;
    char ch;
    ch = getchar();

    if (ch=='\033' && getchar()=='[') //方向键
    {
        ch = getchar();
    	switch (ch)
    	{
            case 'A':
                key = UP;    
                break;
    	    case 'B': 
                key = DOWN;  
                break;
    	    case 'C': 
                key = RIGHT; 
                break;
    	    case 'D': 
                key = LEFT;  
                break;
    	}
    }
    else if (ch == '\n') key = ENTER;
    else if (ch == 'q') key = QUIT;
    else key = UNKNOW;

    return key;
}

// 检查当前位置是否能落子
int checkDown(int x, int y)
{
    if (board[y][x] == EMPTY) return 1;

    else // 如果不能下则提示不能下
    {
        printf(" \033[s\033[33mYou can't put chess here.\033[0m\n");
        sleep(1); // 等待1秒钟
        printf("\033[u\033[K"); // 清除提示信息
        return 0;
    }
}

// 检查当前下棋方是否胜利
int checkWin(int x, int y, int player)
{
    int i, j, count = 0, isWin = 0;

    for (i = x-4; i <= x+4; i++) // 水平方向
    {
    	if (i < 0 || i > BOARD_SIZE-1) continue; // 检查越界
    	if (board[y][i] == player) // 棋子计数
    	{
    	    count++;
    	    if (count == 5)
    	    {
    	        isWin = 1;
    		    return isWin;
    	    }
    	}
    	else count = 0;// 清零重新计数
    }

    count = 0;  // 必须清零

    for (j = y-4; j <= y+4; j++) // 垂直方向
    {
        if (j < 0 || j > BOARD_SIZE-1) continue; // 检查越界
    	if (board[j][x] == player) // 棋子计数
    	{
    	    count++;
    	    if (count == 5)
    	    {
    	        isWin = 1;
                return isWin;
    	    }
    	}
    	else count = 0; // 清零重新计数
    }

    count = 0;  // 必须清零

    for (i = x-4, j = y+4; i <= x+4 && j>=0; i++, j--) // 斜线方向
    {
        if (x < 0 || x > BOARD_SIZE - 1 || y < 0 || y > BOARD_SIZE - 1) continue; // 越界检查
    	if (board[j][i] == player) // 棋子计数
    	{
    	    count++;
    	    if (count == 5)
    	    {
    	        isWin = 1;
                return isWin;
    	    }
    	}
    	else count = 0; // 清零以重新计数
    }
    count = 0;
    
    for (i = x - 4, j = y - 4; i <= x + 4 && j <= y + 4; i++, j++) // 反斜线方向
    {
	    if (x < 0 || x > BOARD_SIZE - 1 || y < 0 || y > BOARD_SIZE - 1) continue; // 越界检查
    	if (board[j][i] == player) // 棋子计数 
    	{
    	    count++;
    	    if (count == 5)
    	    {
    	        isWin = 1;
    		    return isWin;
    	    }
    	}
    	else count = 0;// 清零以重新计数
    }

    return isWin;
}

// 退出游戏
void exitGame()
{
    system("stty icanon"); // 开缓冲
    system("stty echo");   // 开回显
    printf("\033[?25h");   // 显示鼠标
    putchar('\n');
    exit(0);
}

