#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <set>
#include <sstream>
#include <cmath>
#include <chrono>
using namespace std;
using namespace std::chrono;

const int N = 9;
const int targetSta[N] = {1, 2, 3, 8, 0, 4, 7, 6, 5};
const int targetPos[N] = {4, 0, 1, 2, 5, 8, 7, 6, 3};
// const int N = 16;
// // ����Ԫ�ص�����
// const int targetSta[N] = {1, 2, 3, 4,
//                           12, 13, 14, 5,
//                           11, 0, 15, 6,
//                           10, 9, 8, 7};
// // Ŀ��״̬
// const int targetPos[N] = {9, 0, 1, 2,
//                           3, 7, 11, 15,
//                           14, 13, 12, 8,
//                           4, 5, 6, 10};
//pos[i] ����i ���±�Ӧ��Ϊ ʲô such 0 index is 9
const int dim = sqrt(N);
//dim �߳�
const int LEFT = 1, RIGHT = 2, UP = 3, DOWN = 4;
//���巽��
struct Node
{
    int *status;
    Node *father;
    int weight, deepNum;
    // weight Ϊ h deepNum Ϊ g
};
// �ڵ� ���� 1 ״̬ 2 ���ڵ�ָ�� 3 Ȩ�� 4 �Ѿ����������
typedef pair<int, Node *> PII;
// pair Ȩ�� ���ָ��
// 

int creaNum, closeNum;
// open ���������� close ��������
queue<Node *> closeQueue;
// close �����
set<string> vis;
// ����vis string
priority_queue<PII, vector<PII>, greater<>> openQueue;
// ���ȶ��� open��
Node *getStart();
// ��ȡ��ʼ״̬
bool checkVised(const int *test);
// ������״̬��û�б�������������з��ط���TRUE else add set return false
Node *A_star();
bool isSolve(int *start);
Node *move(Node *father, int dis);
// ���ڵ� Ŀ�ĵأ���
int getZeroPos(const int *arr);
bool isEqual(const int *test1, const int *test2);
string toStr(const int *arr);
void deleteNode(Node *node);
void delAllNode();
void printAns(Node *state);
int heuFun(Node *newSta);
// ����·�����

int main()
{
    Node *initState = getStart();
    creaNum++, openQueue.push({initState->weight, initState});
    checkVised(initState->status);

    /* Time Test */
    auto start = high_resolution_clock::now();
    /*      A_star Function      */
    Node *endState = A_star();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken by function: "
         << duration.count() << " milliseconds" << endl;

    if (endState == NULL)
    {
        puts("No Solution!");
    }
    else
    {
        printf("Number of extension nodes: %d\n", closeNum);
        printf("Number of generated nodes: %d\n", creaNum);
        printAns(endState);
    }

    delAllNode();
    system("pause");
    return 0;
}

/**
 * Get the Initial state
*/
Node *getStart()
{
    Node *initState = new Node;
    initState->status = new int[N];
    // ��������Ԫ�ص�һά����
    cout << "Please enter the initial state: \n";
    for (int i = 0; i < N; i++)
        cin >> initState->status[i];
    //����һά����
    initState->deepNum = 0;
    initState->father = NULL;
    initState->weight = heuFun(initState);
    return initState;
}

/**
 * If the state has visted, return true,
 * otherwise, return false and mark the state
 * @param test
 * @return true or fasle
*/
bool checkVised(const int *test)
{
    string s = toStr(test);
    // statue to string
    if (vis.find(s) != vis.end())
        return true;
    vis.insert(s);
    return false;
}

/**
 * A_star algorithm to solve the problem
*/
Node *A_star()
{
    // The initial state and the target state cannot reach each other
    if (!isSolve(openQueue.top().second->status))
        return NULL;

    // loop while open table is not empty
    while (!openQueue.empty())
    {
        Node *father = openQueue.top().second;
        //get f minest to father for explain child nodes
        closeNum++;
        // Move elements from the open table to the close table
        openQueue.pop();
        closeQueue.push(father);

        // Four directions of movement
        for (int i = 1; i <= 4; i++)
        {
            Node *newSta = move(father, i);
            if (newSta == NULL) continue;
            // can not 
            openQueue.push({newSta->weight, newSta});
            creaNum++;

            if (isEqual(newSta->status, targetSta))
            {
                // Find the target state
                return newSta;
            }
        }
    }
    return NULL;
}

/**
 * Determine whether the problem can be solved
 * @param start
 * @param targetSta
*/
bool isSolve(int *start)
{
    int startInver, targetInver;
    // ���������������
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            // ����0 because  if start[j] == 0 ����
            if (start[i] && start[j] && start[i] > start[j])
                startInver++;
            if (targetSta[i] && targetSta[j] && targetSta[i] > targetSta[j])
                targetInver++;
        }
    }

    if ((N - 1) % 2 == 0)
    {
        // if dim is odd �ո������ƶ���Ӱ����������Ե���ż��
        return startInver % 2 == targetInver % 2;
    }
    else
    {
        int startZero = getZeroPos(start) / dim;
        int targetZero = getZeroPos(targetSta) / dim;
        int k = abs(startZero - targetZero);
        // 0�Ĳ�ֵ
        if (startInver % 2 == targetInver % 2)
        {
            // �൱��0 �����ƶ��Ĵ���
            // if inver num ��ͬ ��������ƶ�ż���Σ��ɽ�
            return k % 2 == 0;
        }
        else
        {
            // if inver num ��ͬ if up or down odd times can solve
            return k % 2;
        }
    }
}

/**
 * Move the blank block to the given direction
 * @param father Status to be moved
 * @param dis Moving direction
 * @return If the movement is legal, return to the state after the movement, 
 *         otherwise return a null pointer
*/
Node *move(Node *father, int dis)
{
    int zeroPos = getZeroPos(father->status), newPos;
    // define newPos 
    // Initialize the new state
    Node *newSta = new Node;
    newSta->father = father;
    newSta->deepNum = father->deepNum + 1;
    newSta->status = new int[N];
    memcpy(newSta->status, father->status, sizeof(int) * N);

    // Determine the index of the grid to be swapped,
    // and check whether the swap is legal
    switch (dis)
    {
    case LEFT:
        newPos = zeroPos - 1;
        // turn left pos -1
        if (newPos < 0 || zeroPos % dim == 0)
        {
            // if origin is leftest => illegal
            delete (newSta);
            return NULL;
        }
        break;
    case RIGHT:
        newPos = zeroPos + 1;
        if (newPos >= N || newPos % dim == 0)
        {
            // if newpos is next line leftest => illegal
            // origin is rightest
            delete (newSta);
            return NULL;
        }
        break;
    case UP:
        newPos = zeroPos - dim;
        if (newPos < 0)
        {
            delete (newSta);
            return NULL;
        }
        break;
    case DOWN:
        newPos = zeroPos + dim;
        if (newPos >= N)
        {
            delete (newSta);
            return NULL;
        }
        break;
    default:
        break;
    }
    // legal
    // Swap two grid
    swap(newSta->status[zeroPos], newSta->status[newPos]);
    // Check if the status has been expanded
    if (checkVised(newSta->status))
    {
        deleteNode(newSta);
        return NULL;
    }
    newSta->weight = heuFun(newSta); // Obtain the heuristic function value
    return newSta;
}

/**
 * Find the position of the blank space
 * @param arr
 * @return
*/
int getZeroPos(const int *arr)
{
    //arr is statue
    for (int i = 0; i < N; i++)
    {
        if (arr[i] == 0)
            return i;
    }
    return -1;
}

/**
 * Return True if the two states are the same
 * @param test1
 * @param test2
 * @return true or false
*/
bool isEqual(const int *test1, const int *test2)
{
    return memcmp(test1, test2, sizeof(int) * N) == 0;
}

/**
 * Convert an array of integers to a string
 * @param arr
 * @return A string
*/
string toStr(const int *arr)
{
    stringstream ss;
    // �൱��sprintf ���������뵽������
    for (int i = 0; i < N; i++)
        ss << arr[i] << ',';
        // ƴ������Ԫ�أ�ÿ��Ԫ�غ���ӣ�
    return ss.str();
    // ת��string
}

/**
 * Release the space pointed by the pointer
 * @param node 
*/
void deleteNode(Node *node)
{
    delete (node->status);
    delete (node);
    node = NULL;
}

/**
 * Backtracking output the movement process
 * @param state
*/
void printAns(Node *state)
{
    if (state == NULL)
        return;
    printAns(state->father);

    printf("The %d step: \n", state->deepNum);
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            printf("%3d ", state->status[i * dim + j]);
        }
        puts("");
    }
}

/**
 * Delete all node
*/
void delAllNode()
{
    while (!openQueue.empty())
    {
        deleteNode(openQueue.top().second);
        openQueue.pop();
    }

    while (!closeQueue.empty())
    {
        deleteNode(closeQueue.front());
        closeQueue.pop();
    }
}

/**
 * Heuristic function 1: 
 * The sum of the number of grid that are not at the target position and the path depth 
 * @param newSta 
*/
// int heuFun(Node *newSta)
// {
//     int weight = newSta->deepNum;
//     for (int i = 0; i < N; i++)
//         weight += newSta->status[i] != targetSta[i];
//     return weight;
// }

/**
 * Heuristic function 2:
 * The sum of the distance between all grid and their target positions and the path depth
 * @param newSta
*/
// int heuFun(Node *newSta)
// {
//     int weight = newSta->deepNum;
//     for (int i = 0; i < N; i++)
//     {
//         int num = newSta->status[i];
//         int expePos = targetPos[num];
//         weight += abs(i / dim - expePos / dim) + abs(i % dim - expePos % dim);
//     }
//     return weight;
// }

/**
 * Heuristic function 3: The path depth
 * @param newSta
*/
int heuFun(Node *newSta)
{
    return newSta->deepNum;
}

/*
Test Data 1:
6 8 0
5 2 3
1 4 7
*/

/*
Test Data 2:
2 8 3
1 6 4
7 0 5
*/

/*
Test Data 3:
1 2 3
0 8 4��
6 7 5
*/

/*
Test Data 4:
1 2 3 4
11 13 5 6
10 12 0 8
9 14 15 7
*/