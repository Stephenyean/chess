#ifndef MCT_H
#define MCT_H
#include <iostream>
#include "judge.h"
#include "time.h"
using namespace std;
class Node{
public:
	Node() {}
	Node(int _M,int _N,int top[12],int _board[12][12],int _iter);
	~Node();
	void initInit(int _M, int _N, int top[12], int _board[12][12], int _iter);
	void init();
	int board[12][12];
	int top[12];
	int M;
	int N;
	int iter;
	int x;
	int y;
	float winNum;
	int totalNum;
	float score;
	Node* p;
	Node** child;
	int childState[12];
	int childNum;
	int activateChild;
	bool notTerminal();
	bool notOver(int ,int,int&);
	bool expandable();
	
};
class MCT{
public:
	int x;
	int y;
	int count;
	int lastX;
	int lastY;
	int M;
	int N;
	clock_t start;
	clock_t finish;
	int *top;
	int board[12][12];
	Node* _root;
	float c;
	bool timeLimited();
	float score(Node* v);
	MCT(int _x,int _y,int _lastX, int _lastY, int _M, int _N, int* _top, int** _board, clock_t _s):x(_x),y(_y),lastX(_lastX),lastY(_lastY),M(_M),N(_N),top(_top),start(_s)
	{
		count = 0;
		//board = new int*[M];
		for (int i = 0; i < M; i++)
		{
			//board[i] = new int[N];
			for (int j = 0; j < N; j++)
				board[i][j] = _board[i][j];
		}
	}
	~MCT(){}
	bool over(int, int, int lt[12], int lb[12][12], int&);
	void uctSearch();
	Node* treePolicy(Node*,int&);
	Node* expand(Node* );
	Node* bestChild(Node*);
	float defaultPolicy(Node* &);
	void backUp(Node*, float);
	//float evaluate(Node* v);
};


#endif
