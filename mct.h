#ifndef MCT_H
#define MCT_H
#include <iostream>
#include "judge.h"
#include "time.h"
using namespace std;
class Node{
public:
	Node(int _M,int _N,int* top,int** _board,int _iter);
	~Node();
	void init();
	int **board;
	int *top;
	int M;
	int N;
	int iter;
	
	float winNum;
	int totalNum;
	float score;
	Node* p;
	Node** child;
	int* childState;
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
	int* top;
	int** board;
	Node* _root;
	float c;
	bool timeLimited();
	float score(Node* v);
	MCT(int _x,int _y,int _lastX, int _lastY, int _M, int _N, int* _top, int** _board):x(_x),y(_y),lastX(_lastX),lastY(_lastY),M(_M),N(_N),top(_top)
	{
		count = 0;
		board = new int*[M];
		for (int i = 0; i < M; i++)
		{
			board[i] = new int[N];
			for (int j = 0; j < N; j++)
				board[i][j] = _board[i][j];
		}
	}
	~MCT(){}
	
	void uctSearch();
	Node* treePolicy(Node*);
	Node* expand(Node* );
	Node* bestChild(Node*);
	float defaultPolicy(Node* &);
	void backUp(Node*, float);
	//float evaluate(Node* v);
};


#endif
