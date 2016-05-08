#include "mct.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;


Node AllNodes[20000];
int nodeCount=0;
Node::Node(int _M,int _N,int _top[12],int _board[12][12],int _iter):M(_M),N(_N),iter(_iter)
{
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
			board[i][j] = _board[i][j];
	}
	for(int i=0;i<N;i++)
		top[i]=_top[i];
	
}
Node::~Node()
{
	/*
	delete[]childState;
	for (int i = 0; i < childNum; i++)
		if (child[i])
			delete child[i];
	delete[]child;
	*/
}
void Node::initInit(int MM, int NN, int _top[12], int _board[12][12], int _iter)
{
	this->M = MM; this->N = NN; this->iter = _iter;
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
			board[i][j] = _board[i][j];
	}
	for (int i = 0; i<N; i++)
		top[i] = _top[i];
}
void Node::init()
{
	childNum=0;
	for(int i=0;i<N;i++)
		if(top[i]!=0)
			childState[childNum++]=i;
	child=new Node*[childNum];
	for (int i = 0; i < childNum; i++)
		child[i] = NULL;
	activateChild=0;
}
bool Node::notTerminal(){
	return childNum!=0;
}
bool Node::notOver(int x,int y,int& success){

	if(userWin(x,y,M,N,board))
	{
		success=1;
		return false;
	}
	if(machineWin(x,y,M,N,board))
	{
		success=-1;
		return false;
	}
	if (isTie(N, top))
	{
		success = 0;
		return false;
	}
	return true;
}
bool MCT::over(int x, int y, int localTop[12], int localBoard[12][12] ,int& success)
{
	if (userWin(x, y, M, N, localBoard) || machineWin(x, y, M, N, localBoard))
	{
		success = 1;
		return true;
	}
	if (isTie(N, localTop))
	{
		success=0;
		return true;
	}
	return false;
}
bool Node::expandable()
{
	return childNum!=activateChild;
}
bool MCT::timeLimited()
{
	finish=clock();
	double duration=(double)(finish-start)/CLOCKS_PER_SEC;
	//cout << duration << endl;
	return duration>2.5;
}

void MCT::uctSearch()
{
	_root=new Node(M,N,top,board,0);
	_root->init();
	_root->p=NULL;
	//start=clock();
	srand(time(NULL));
	while(!timeLimited())
	{
		int success=-1;
		Node* vl=treePolicy(_root,success);
		float delta;
		if(success==-1)
		{
			if(over(vl->x,vl->y,vl->top,vl->board,success))
				delta=success*100;
			else
				delta=defaultPolicy(vl);
		}
		else
			delta=success*100;
		backUp(vl,delta);
	}
	Node* c = bestChild(_root);
	int i;
	for (i = 0; i < N; i++)
	{
		if (c->top[i] != _root->top[i])
		{
			y = i; break;
		}
	}
	x = c->top[i];
//	cout << x << " " << y << endl;
}
Node* MCT::treePolicy(Node* v0,int& success)
{
	int xx=lastX,yy=lastY;
	success=-1;
	while(!over(xx,yy,v0->top,v0->board,success))
	{
		if(v0->expandable())
			return expand(v0);
		else
		{
			v0=bestChild(v0);
			xx=v0->x;yy=v0->y;
		}
	}
	return v0;
}

Node* MCT::expand(Node* v)
{
	int j=rand()%(v->childNum-v->activateChild);
	for(int i=0;i<v->childNum;i++)
	{
		if(!v->child[i])
			if(j==0)
			{
				//i = v->activateChild;
				//Node* c=(v->child[i]=new Node(M,N,v->top,v->board,v->iter+1));
				Node* c = (v->child[i] = &AllNodes[nodeCount++]);
				c->initInit(M, N, v->top, v->board, v->iter + 1);
				c->top[v->childState[i]]--;
				//cout << c->top[v->childState[i]] << " " << v->childState[i] << endl;
				int xx = c->top[v->childState[i]];
				int yy = v->childState[i];
				c->x = xx;
				c->y = yy;
				v->activateChild++;
				c->board[xx][yy]=(c->iter+1)%2+1;
				count++;
				c->init();
				c->p=v;
				return c;
			}
			else
				j--;
	}
	
	return v;
}

Node* MCT::bestChild(Node* v)
{
	int j=0;float max=-100000;
	for(int i=0;i<v->childNum;i++)
	{
		if (!v->child[i])
			continue;
		float m=score(v->child[i]);
		if(m>max)
		{
			max=m;j=i;
		}
	}
	return v->child[j];
}

float MCT::score(Node* v)
{
	return float(v->winNum)/float(v->totalNum);//+0.1*sqrt(2*log(v->p->totalNum)/float(v->totalNum));
}

float MCT::defaultPolicy(Node*& v)
{
	//cout << count << endl;
	if (v->childNum == 0)
		return 0;
	//int k = rand() % v->childNum;
	int success = 0;
	float value = 0;
	int localBoard[12][12];
	int localTop[20];
	for (int round = 0; round < 100; round++)
	{
		for (int i = 0; i < M; i++)
			for (int j = 0; j < N; j++)
				localBoard[i][j] = v->board[i][j];
		for (int j = 0; j < N; j++)
		{
			localTop[j] = v->top[j];
		}
		int iter = 0;
		int xx=v->x, yy=v->y;
		while (!over(xx, yy, localTop, localBoard, success))
		{
			iter++;
			int k = rand() % N;
			int i = 0;
			while (true)
			{
				if (localTop[i] != 0 && k == 0)
					break;
				if (localTop[i] != 0) k--;
				i = (i + 1) % N;
			}
			localTop[i]--;
			xx = localTop[i]; yy = i;
			localBoard[xx][yy] = (v->iter + iter +1) % 2 + 1;
		}
		if (success != 0)
			value += 1.0 - 2.0 * float(iter % 2);
	}
	return value;
}
//
//float MCT::defaultPolicy(Node* &v)
//{
//	if (v->childNum == 0)
//		return 0;
//	int i=rand()%v->childNum;
//	int success=0;
//	while(true)
//	{
//		Node* c=(v->child[i]=new Node(M,N,v->top,v->board,v->iter+1));
//		int xxx = c->top[v->childState[i]];
//		c->top[v->childState[i]]--;
//		//cout << c->top[v->childState[i]] << " " << v->childState[i] << endl;
//		c->board[c->top[v->childState[i]]][v->childState[i]] = (c->iter) % 2 + 1;
//		c->init();
//		c->p=v;
//		count++;
//		
//		int xx = c->top[v->childState[i]];
//		int yy = v->childState[i];
//		v = c;
//		if (!v->notOver(xx,yy , success))
//			break;
//		i=rand()%v->childNum;
//		
//	}
//	return success;
//}

void MCT::backUp(Node* v,float delta)
 {
	while(v!=NULL)
	{
		v->totalNum+=100;
		v->winNum+=delta;
		delta=-delta;
		v=v->p;
	}
}
