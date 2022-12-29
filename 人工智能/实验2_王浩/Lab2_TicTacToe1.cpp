/* *******************************************************
* Author:     Pihai Sun                                  *
* University: Qingdao University                         *
* College:    College of Computer Science and Technology *
*********************************************************/
#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <cstdio>
#include <typeinfo>

using namespace std;

enum GameRes {HumanWin, AIWin, Draw, NotOver};
enum PosiType {Side, Corner, Central};
enum GameType {AIFirst, HumanFist};
enum CheckRes {BoundartErr, RepeatErr, LegalPos};
const int PIECE_NUM = 9;
const int INF = 0x3fffffff;
const int MAX_DEP = 2 * 4;
const int MAX_NODE = 1;
const int MIN_NODE = 0;
const char AI_PIECE = 'X';
const char HUMAN_PIECE = 'O';
const char BLANK_PIECE = ' ';

struct Node
{
    string situ;
    bool isFull()
    {
        for (int i = 0; i < PIECE_NUM; i++)
        {
            if (situ[i] == BLANK_PIECE)
                return false;
        }
        return true;
    }
};
class TicTacToe
{
private:
    string piece;
public:
    TicTacToe();
    GameType chooseFirst();
    void printBoard();
    void humanRound();
    void AIRound();
    int getHumanChoose();
    CheckRes checkPos(int r, int c);
    bool checkOver();
    GameRes gameIsOver();
    GameRes getRole(char piecetype);
    int EvaFun(string situ);
    PosiType checkPosType(int pos);
    void playGame();
    int willWin(string s, char tar);
    int MIN_LAY(int dep, Node nowSta, int alpha, int beta);
    int MAX_LAY(int dep, Node nowSta, int alpha, int beta);
};

int main()
{
    TicTacToe game;
    game.playGame();
    system("pause");
    return 0;
}
// ��ʼ�� ȫΪ��
TicTacToe::TicTacToe()
{
    for (int i = 1; i <= PIECE_NUM; i++)
        piece += " ";
}
// 
void TicTacToe::playGame()
{
    if (chooseFirst() == AIFirst)
    {
        AIRound();
    }

    while (true)
    {
        printBoard();
        if (checkOver()) break;
        humanRound();

        printBoard();
        if (checkOver()) break;
        AIRound();
    }
}

GameType TicTacToe::chooseFirst()
{
    int type;
    puts("Please enter 1 to select human first or enter 0 to select AI first!");
    while (true)
    {
        cin >> type;
        puts("");
        if (type == 0)
        {
            puts("You have chosen AI first!");
            break;
        }
        else if (type == 1)
        {
            puts("You have chosen Human first!");
            break;
        }
        else
        {
            puts("ERROR: Please enter 1 or 0!");
        }
    }
    return (GameType)type;
}

void TicTacToe::printBoard()
{
    cout<<"-----"<<endl;
    cout<<piece[0]<<"|"<<piece[1]<<"|"<<piece[2]<<endl;
    cout<<"-----"<<endl;
    cout<<piece[3]<<"|"<<piece[4]<<"|"<<piece[5]<<endl;
    cout<<"-----"<<endl;
    cout<<piece[6]<<"|"<<piece[7]<<"|"<<piece[8]<<endl;
    cout<<"-----"<<endl;
}

int TicTacToe::getHumanChoose()
{
    puts("****** YOUR ROUND ******");
    puts("Please enter the position you want place");
    puts("EX: 1 2 means you want to place a piece in the 1st row and 2nd column");
    int row, col;
    while (true)
    {
        cin >> row >> col;
        puts("");
        if (checkPos(row, col) == LegalPos)
        {
            puts("Successful placement!");
            break;
        }
        else if (checkPos(row, col) == BoundartErr)
        {
            puts("ERROR: Placement position is out of boundary!");
        }
        else if (checkPos(row, col) == RepeatErr)
        {
            puts("ERROR: Please do not repeat the same position!");
        }
    }
    return (row - 1) * 3 + col - 1;
}


void TicTacToe::humanRound()
{
    int nxtpos = getHumanChoose();
    piece[nxtpos] = HUMAN_PIECE;
}

CheckRes TicTacToe::checkPos(int r, int c)
{
    bool f1 = r > 0 && r <= 3 && c > 0 && c <= 3;
    bool f2 = piece[(r - 1) * 3 + c - 1] == ' ';
    if (!f1) return BoundartErr;
    if (!f2) return RepeatErr;// ԭλ�÷ǿ�
    return LegalPos;
}

void TicTacToe::AIRound()
{
    puts("****** AI ROUND ******");
    int alpha = -INF, beta = INF, nxtpos = -1;
    nxtpos = willWin(piece, AI_PIECE);
    if (nxtpos != -1)// AI �Ѿ�Ӯ��
    {
        puts("hh");
        piece[nxtpos] = AI_PIECE;
        return;
    } 

    nxtpos = willWin(piece, HUMAN_PIECE);
    if (nxtpos != -1)// ����������Ӯ��λ��
    {
        puts("hhh");
        piece[nxtpos] = AI_PIECE;
        return;
    }

    Node nowNode = {piece};
    // ѭ����ÿ���ڵ���һ�£����ĸ�λ�ú���
    for (int i = 0; i < PIECE_NUM; i++)
    {
        if (nowNode.situ[i] != BLANK_PIECE) continue;
        Node newNode= {nowNode.situ}; 
        newNode.situ[i] = AI_PIECE;
        int val = MIN_LAY(1, newNode, alpha, beta);
        // alpha Ϊ��ǰֵ��ѭ����MAX�����ҵ����п���Ӯ��λ��
        if (alpha < val)
        {
            alpha = val;
            nxtpos = i;
        }
    }

    piece[nxtpos] = AI_PIECE;
}
// tar AI piece��������һ����Ӯ��λ��
int TicTacToe::willWin(string s, char tar)
{
    // �����ж��ܲ���Ӯ
    for (int r = 0; r < 3; r++)
    {
        int tarnum = 0, blank = 0, blankpos = -1;
        for (int c = 0; c < 3; c++)
        {
            int pos = r * 3 + c;
            if (s[pos] == tar) tarnum++;
            if (s[pos] == BLANK_PIECE) blankpos = pos, blank++;
        }
        if (tarnum == 2 && blank == 1) return blankpos;
    }

    for (int c = 0; c < 3; c++)
    {
        int tarnum = 0, blank = 0, blankpos = -1;
        for (int r = 0; r < 3; r++)
        {
            int pos = r * 3 + c;
            if (s[pos] == tar) tarnum++;
            if (s[pos] == BLANK_PIECE) blankpos = pos, blank++;
        }
        if (tarnum == 2 && blank == 1) return blankpos;
    }

    int tarnum = 0, blank = 0, blankpos = -1;
    for (int i = 0; i < PIECE_NUM; i += 4)
    {
        if (s[i] == tar) tarnum++;
        if (s[i] == BLANK_PIECE) blankpos = i, blank++;
    }
    if (tarnum == 2 && blank == 1) return blankpos;

    tarnum = blank = blankpos = 0;
    for (int i = 2; i < PIECE_NUM - 1; i += 2)
    {
        if (s[i] == tar) tarnum++;
        if (s[i] == BLANK_PIECE) blankpos = i, blank++;
    }
    if (tarnum == 2 && blank == 1) return blankpos;
    return -1;
}
// alpha ��һ��� alpha ��һ������Сֵ
int TicTacToe::MAX_LAY(int dep, Node nowSta, int alpha, int beta)
{
    if (dep == MAX_DEP || nowSta.isFull())
    {
        return EvaFun(nowSta.situ);
    }

    for (int i = 0; i < PIECE_NUM; i++)
    {
        if (nowSta.situ[i] != BLANK_PIECE) continue;
        Node newNode = {nowSta.situ};
        newNode.situ[i] = AI_PIECE;
        alpha = max(alpha, MIN_LAY(dep + 1, newNode, alpha, beta));
        // ��ǰΪMAX�� ����������һ�㵥��������һ��ֵ����һ�����Сֵ����ô��֦
        if (alpha >= beta) return beta;
    }

    // ��ǰ�㶼��betaС���ҵ�������ֵ
    return alpha;
}

int TicTacToe::MIN_LAY(int dep, Node nowSta, int alpha, int beta)
{
    if (dep == MAX_DEP || nowSta.isFull())
    {
        return EvaFun(nowSta.situ);
    }
    for (int i = 0; i < PIECE_NUM; i++)
    {
        // �ݹ���MAX
        if (nowSta.situ[i] != BLANK_PIECE) continue;
        Node newNode = {nowSta.situ};
        newNode.situ[i] = HUMAN_PIECE;
        // ö�����е�����Сbeta
        beta = min(beta, MAX_LAY(dep + 1, newNode, alpha, beta));
        // ��ǰΪMIN�� �����ݼ�����һ��ֵ����һ���alphaС����һ�㵥������֦
        if (alpha >= beta) return alpha;
    }
    // ����ֵ����alpha���������Сֵ
    return beta;
}

GameRes TicTacToe::getRole(char piecetype)
{
    if (piecetype == BLANK_PIECE)
        return NotOver;
    if (piecetype == HUMAN_PIECE)
        return HumanWin;
    if (piecetype == AI_PIECE)
        return AIWin;
}

GameRes TicTacToe::gameIsOver()
{
    // �����ж�
    for (int r = 0; r < 3; r++)
    {
        bool f1 = piece[r * 3 + 0] == piece[r * 3 + 1];
        bool f2 = piece[r * 3 + 1] == piece[r * 3 + 2];
        bool f3 = piece[r * 3] != BLANK_PIECE;
        if (f1 && f2 && f3) return getRole(piece[r * 3]);
        // getRole �ж���˭Ӯ��
    }

    for (int c = 0; c < 3; c++)
    {
        bool f1 = piece[0 * 3 + c] == piece[1 * 3 + c];
        bool f2 = piece[1 * 3 + c] == piece[2 * 3 + c];
        bool f3 = piece[c] != BLANK_PIECE;
        if (f1 && f2 && f3) return getRole(piece[c]);
    }
    // ��б��
    bool f1 = piece[0] == piece[4] && piece[4] == piece[8];
    bool f2 = piece[0] != BLANK_PIECE;
    if (f1 && f2) return getRole(piece[0]);

    // ��б��
    f1 = piece[2] == piece[4] && piece[4] == piece[6];
    f2 = piece[2] != BLANK_PIECE;
    if (f1 && f2) return getRole(piece[2]);

    for (int i = 0; i < PIECE_NUM; i++)
    {
        if (piece[i] == BLANK_PIECE)
            return NotOver;
    }
    return Draw;
}
// �ж���Ϸ����
bool TicTacToe::checkOver()
{
    GameRes res = gameIsOver();
    if (res == NotOver) return false;

    if (res == HumanWin)
    {
        puts("Congratulations! You win!");
    }
    else if (res == AIWin)
    {
        puts("Really sorry! AI win!");
    }
    else
    {
        puts("It's a tie!");
    }
    return true;
}
// �����������������ǿ����ԣ�����Ϊ2 ����Ϊ1����Ϊ0
int TicTacToe::EvaFun(string situ)
{
    int AI, Human;
    AI = Human = 0;
    for (int i = 0; i < PIECE_NUM; i++)
    {
        int addValue = checkPosType(i) + 1;// ����λ��
        if (situ[i] == AI_PIECE) AI += addValue;
        else if (situ[i] == HUMAN_PIECE) Human += addValue;
    }
    return AI - Human;
}

PosiType TicTacToe::checkPosType(int pos)
{
    if (pos == 4)
        return Central;
    else if (pos == 0 || pos == 2 || pos == 8 || pos == 6)
        return Corner;
    else
        return Side;
}