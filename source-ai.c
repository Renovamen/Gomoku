#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 20
#define EMPTY 0
#define ME 1
#define OTHER 2

#define TRUE 1
#define FALSE 0

#define MAX_BYTE 10000

#define START "START"
#define PLACE "PLACE"
#define DONE  "DONE"
#define TURN  "TURN" 
#define BEGIN "BEGIN"
#define END   "END"

struct Position
{
    int x;
    int y;
}; 

char buffer[MAX_BYTE] = {0};
char board[BOARD_SIZE][BOARD_SIZE] = {0};

void debug(const char *str)
{
    printf("DEBUG %s\n", str);
    fflush(stdout);
}

int MEtable[20][20][2005], OTtable[20][20][2005], win[2][2005];
int Start, Count;

int Around(int x, int y); // 周围是否有子存在，无子的就加入考虑 
int ScoretTable(int Number, int Empty); // 积分表 
int CountScore(int n[], int turn); // 算单个数组分 
int Evaluate();
int Max_AlphaBeta(int Dep, int alpha, int beta); // 轮到AI下子时，AI作的选择 
int Min_AlphaBeta(int Dep, int alpha, int beta); // 轮到对手下子时，对手作的选择 
int Map[BOARD_SIZE][BOARD_SIZE];

const int INT_MAX = 0x7fffffff / 2;
const int INT_MIN = - 0x7fffffff / 2;

void initAI()
{

}

int Around(int x,int y) // 空子只算旁边有子的
{
    int i,j;   
    for(i = (x-2>0?x-2:0); i <= x + 2 && i < 20; i++)
        for(j = (y-2>0?y-2:0); j <= y + 2 && j < 20; j++)
            if( Map[i][j] != EMPTY ) return 1;
    return 0;
}

/*
    按照成五 100000、活四 10000、活三 1000、活二 100、活一 10、死四 1000、死三 100、死二 10 的规则给棋盘上的所有棋子打分，
    然后把所有 AI 和对手棋子的得分分别相加，ScoreME 为 AI 得分，ScoreOTHER 为对手得分，
    ScoreME - ScoreOTHER 即为当前局势的总分数
*/

int ScoreTable(int Number, int Empty) // 计分板
{
    if(Number >= 5) return 100000;
    else if(Number == 4)
    {
        if(Empty == 2) return 10000;
        else if(Empty == 1) return 1000;
    }
    else if(Number == 3)	
    {
        if(Empty == 2) return 1000;
        else if(Empty == 1) return 100;
    }
    else if(Number == 2)
    {
        if(Empty == 2) return 100;
        else if(Empty == 1) return 10;
    }
    else if(Number == 1 && Empty == 2) return 10;
    return 0;
}

int CountScore(int n[], int turn) // 正斜线、反斜线、横、竖，均转成一维数组来计算 
{
    int Scoretmp = 0, L = n[0], Empty = 0, Number = 0;
    if(n[1] == 0) Empty++;
    else if(n[1] == turn) Number++;
    
    int i;
    for(i = 2; i <= L; i++)
    {
        if(n[i] == turn) Number++;
        else if(n[i] == 0)
        {
            if(Number == 0) Empty = 1;
            else
            {
                Scoretmp += ScoreTable(Number, Empty + 1);
                Empty = 1;
                Number = 0;
            }
        }
        else
        {
            Scoretmp += ScoreTable(Number, Empty);
            Empty = 0;
            Number = 0;
        }
    }
    Scoretmp += ScoreTable(Number, Empty);
    return Scoretmp;
}

int Evaluate()//评估函数，评估局势
{
    int ScoreME = 0, ScoreOTHER = 0;
    //横排
    int n[505] = {0}, i, j;
    for(i = 0; i < 20; i++)
    {
        //vector<int> n;
        for(j = 0; j < 20; j++)n[++n[0]] = Map[i][j];
        ScoreME += CountScore(n, ME);
        ScoreOTHER += CountScore(n, OTHER);
        n[0] = 0;
    }
    
    //竖排
    for(j = 0; j < 20; j++)
    {
        for(i=0;i<20;i++)n[++n[0]] = Map[i][j];
        ScoreME += CountScore(n,ME);
        ScoreOTHER += CountScore(n,OTHER);
        n[0] = 0;
    } 
    
    //上半正斜线 
    for(i = 0; i < 20; i++)
    {
        int x, y;
        for(x = i, y = 0; x < 20 && y < 20; x++, y++) n[++n[0]] = Map[y][x];
        ScoreME += CountScore(n, ME);
        ScoreOTHER += CountScore(n, OTHER);
        n[0] = 0;
    } 

    //下半正斜线
    for(j = 1; j < 20; j++)
    {
        int x, y;
        for(x = 0, y = j; y < 20 && x < 20; x++, y++) n[++n[0]] = Map[y][x];
        ScoreME += CountScore(n,ME);
        ScoreOTHER += CountScore(n,OTHER);
        n[0] = 0;
    } 
    
    //上半反斜线
    for(i = 0; i < 20; i++)
    {
        int x, y;
        for(y = i, x = 0; y >= 0 && x < 20; y--, x++) n[++n[0]] = Map[y][x];
        ScoreME += CountScore(n, ME);
        ScoreOTHER += CountScore(n, OTHER);
        n[0] = 0;
    } 

    //下半反斜线
    for(j = 1; j < 20; j++)
    {
        int x, y;
        for(y = j, x = 19; y < 20 && x >= 0; y++, x--) n[++n[0]] = Map[x][y];
        ScoreME += CountScore(n, ME);
        ScoreOTHER += CountScore(n, OTHER);
        n[0] = 0;
    } 
    return ScoreME - ScoreOTHER;
} 

int Min_AlphaBeta(int Dep, int alpha, int beta) // 当 min（对手）走步时，对手的最好情况 
{
    int res = Evaluate();
    if(Dep == 0)return res;
  
    struct Position v[505];
    //EmptyPoint(v);
    v[0].x = 0;	
    int i,j;
    for(i = 0; i < 20; i++)
        for(j = 0; j < 20; j++)
            if(Map[i][j] == EMPTY && Around(i,j))
            {
                v[++v[0].x].x = i;
                v[v[0].x].y = j;
            }  
	
    int L = v[0].x;
    int best = INT_MAX;
    for(i = 1; i <= L; i++)
    {
        Map[v[i].x][v[i].y] = OTHER; 
        // 我是极小层，我要的是更小的数。我找过的孩子中，目前为止找到的最小的数是best，如果best小于了
        // 前辈们之前找到的最小值，那么我将更新它，并且告诉下面未遍历过的孩子，比alpha大的数就不要再给我了 
        int tmp = Max_AlphaBeta(Dep-1, best<alpha?best:alpha, beta); 
        Map[v[i].x][v[i].y] = EMPTY;
        if(tmp < best)best = tmp;
        if(tmp < beta)break;
    } 
    return best;
}

int Max_AlphaBeta(int Dep, int alpha, int beta)//AI走步时应该考虑最好的情况 
{
    int res=Evaluate();
    if(Dep==0)return res;
  
    struct Position v[505];
    //EmptyPoint(v);
    int i,j;
    v[0].x=0;	
    for(i=0;i<20;i++)
        for(j=0;j<20;j++)
            if(Map[i][j]==EMPTY&&Around(i,j))
            {
                v[++v[0].x].x=i;
                v[v[0].x].y=j;
            }  
	
    int L=v[0].x;
    int best = INT_MIN;
    for(i=1;i<=L;i++)
    {
        Map[v[i].x][v[i].y]=ME;
        int tmp=Min_AlphaBeta(Dep-1,alpha,best>beta?best:beta);
        Map[v[i].x][v[i].y]=EMPTY; 
        if(tmp>best)best=tmp; 
        // 这是极大层，电脑选取最大值点。到目前为止(i=0)，已知tmp。那么该层向上返回的值将不小于tmp
        if(tmp>alpha)break;

        // 我的上一层告诉我，它找到的最小的数是alpha，它是极小层，他需要更小的数。
        // 如果我找到的tmp比alpha大，那么就不要找了，因为我是极大层，我只会返回更大的数给上一层
    } 
    return best;
}

struct Position MinMax_AlphaBeta(int Dep)//极大极小值算法搜索n步后的最优解 
{
    struct Position v[505],v2[505];
    //EmptyPoint(v);
    v[0].x = 0;
    int i, j;	
    for(i = 0; i < 20; i++)
        for(j = 0; j < 20; j++)
            if(Map[i][j] == EMPTY && Around(i,j))
            {
                v[++v[0].x].x=i;
                v[v[0].x].y=j;
            }  
 
    int best = INT_MIN;
    int L = v[0].x;
    v2[0].x = 0; // v2[0].x 表示 v2 中的元素数量 
    for(i = 1; i <= L; i++)
    {
        Map[v[i].x][v[i].y] = ME;//选该子，将该子置白，防止后面递归时，再递归到 
        int tmp = Min_AlphaBeta(Dep, INT_MAX, INT_MIN);
        if(tmp == best)v2[++v2[0].x] = v[i];
        if(tmp > best)
        {
            best = tmp;
            v2[0].x = 0;
            v2[++v2[0].x] = v[i];
        }
        Map[v[i].x][v[i].y] = EMPTY;	//假设完之后，该子需要重新置空，恢复原来的样子 
    }
    L = v2[0].x;
    
    int k = (int)(rand()%L) + 1;
    return v2[k];
}

struct Position aiBegin(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
    struct Position preferedPos;
 
    preferedPos.x = 10;
    preferedPos.y = 10;
    return preferedPos;
}

struct Position aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me, int otherX, int otherY)
{
    int i,j;
    for(i=0;i<20;i++)
        for(j=0;j<20;j++)Map[i][j] = board[i][j];

    struct Position preferedPos = MinMax_AlphaBeta(2);
    return preferedPos;
}

/*
 * YOUR CODE END
 * 你的代码结束 
 */

void place(int x, int y, int z)
{
    board[x][y] = z;
    
    int i;
    if(z == OTHER)
    {
      for(i = 0; i < Count; i++)
	  {
	    // 修改OTHER下子后，棋盘的变化状况
	    if(OTtable[x][y][i] && win[0][i] != 7) win[0][i]++;
	    if(MEtable[x][y][i])
	    { 
	      MEtable[x][y][i] = 0;
	      win[1][i] = 7;
	    }
	  } 
	}
    
    else if(z == ME)
	{ 
      for(i = 0; i < Count; i++)
      {
        //修改ME下子后，棋盘的变化状况
	    if(MEtable[x][y][i] && win[1][i]!=7) win[1][i]++;
	    if(OTtable[x][y][i])
  	    {
	      OTtable[x][y][i] = 0;
	      win[0][i] = 7;
	    }
      }
    }    
}

void done()
{
    printf("OK\n");
    fflush(stdout);
}

void start()
{
    memset(board, 0, sizeof(board));
    initAI();
}

void turn(int x, int y)
{
    board[x][y] = OTHER;
    
    // AI
    int i;
	for(i = 0; i < Count; i++)
	{
	  if(OTtable[x][y][i] && win[0][i] != 7) win[0][i]++;
	  if(MEtable[x][y][i])
	  { 
	    MEtable[x][y][i] = 0;
	    win[1][i] = 7;
	  }
	}
	
    struct Position pos = aiTurn((const char (*)[20])board, ME, x, y);
    board[pos.x][pos.y] = ME;
    printf("%d %d\n", pos.x, pos.y);
    								
    fflush(stdout);
}

void end(int x)
{
    
}

void loop()
{
	while (TRUE)
    {
        memset(buffer, 0, sizeof(buffer));
        gets(buffer);
        
        if (strstr(buffer, START))
        {
            start();
        }
        else if (strstr(buffer, PLACE))
        {
            char tmp[MAX_BYTE] = {0};
            int x, y, z;
            sscanf(buffer, "%s %d %d %d", tmp, &x, &y, &z);
            place(x, y, z);
        }
        else if (strstr(buffer, DONE))

        {
            done();
        }
        else if (strstr(buffer, BEGIN))
        {
    		// AI
    		struct Position pos = aiBegin((const char (*)[20])board, ME);    		
    		board[pos.x][pos.y] = ME;
    		printf("%d %d\n", pos.x, pos.y);
    		fflush(stdout);
		}
        else if (strstr(buffer, TURN))
        {
            char tmp[MAX_BYTE] = {0};
            int x,  y;
            sscanf(buffer, "%s %d %d", tmp, &x, &y);
            turn(x, y);
        }
        else if (strstr(buffer, END))
        {
            char tmp[MAX_BYTE] = {0};
            int x;
            sscanf(buffer, "%s %d", tmp, &x);
            end(x);
        }
    }
}


int main(int argc, char *argv[]) {
    loop();
    return 0;
}
