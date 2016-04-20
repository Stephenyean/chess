#include "mct.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;
Node::Node(int _M,int _N,int* _top,int** _board,int _iter):M(_M),N(_N),iter(_iter)
{
	board = new int*[M];
	for (int i = 0; i < M; i++)
	{
		board[i] = new int[N];
		for (int j = 0; j < N; j++)
			board[i][j] = _board[i][j];
	}
	top = new int[N];
	for(int i=0;i<N;i++)
		top[i]=_top[i];
	
}
Node::~Node()
{
	delete[]childState;
	for (int i = 0; i < childNum; i++)
		if (child[i])
			delete child[i];
	delete[]child;
	for (int i = 0; i < M; i++)
		delete[]board[i];
	delete[]board;
	delete[]top;
}
void Node::init()
{
	childState=new int[12];
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
	//for (int i = 0; i < M; i++)
	//{
	//	for (int j = 0; j < N; j++)
	//	{
	//		cout << board[i][j] << " ";
	//	}
	//	cout << endl;
	//}
	//cout << x << " " << y << endl;
	if(userWin(x,y,M,N,(int**)board))
	{
		success=1;
		return false;
	}
	if(machineWin(x,y,M,N,(int**)board))
	{
		success=-1;
		return false;
	}
	if(isTie(N,top))
		return false;
	return true;
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
	return duration>4.0;
}

void MCT::uctSearch()
{
	_root=new Node(M,N,top,board,0);
	_root->init();
	_root->p=NULL;
	start=clock();
	while(!timeLimited())
	{
		Node* vl=treePolicy(_root);
		float delta=defaultPolicy(vl);
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
	cout << x << " " << y << endl;
}
Node* MCT::treePolicy(Node* v0)
{
	while(v0->notTerminal())
	{
		if(v0->expandable())
			return expand(v0);
		else
			v0=bestChild(v0);
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
				Node* c=(v->child[i]=new Node(M,N,(int*)v->top,(int**)v->board,v->iter+1));
				c->top[v->childState[i]]--;
				//cout << c->top[v->childState[i]] << " " << v->childState[i] << endl;
				c->board[c->top[v->childState[i]]][v->childState[i]]=(c->iter)%2+1;
				count++;
				c->init();
				c->p=v;
				return c;
			}
			else
				j--;
	}
	v->activateChild++;
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
	return float(v->winNum)/float(v->totalNum)+c*sqrt(2*log(v->p->totalNum)/float(v->totalNum));
}

float MCT::defaultPolicy(Node* &v)
{
	int i=rand()%v->childNum;
	int success=0;
	while(true)
	{
		Node* c=(v->child[i]=new Node(M,N,(int*)v->top,(int**)v->board,v->iter+1));
		int xxx = c->top[v->childState[i]];
		c->top[v->childState[i]]--;
		//cout << c->top[v->childState[i]] << " " << v->childState[i] << endl;
		c->board[c->top[v->childState[i]]][v->childState[i]] = (c->iter) % 2 + 1;
		c->init();
		c->p=v;
		count++;
		
		int xx = c->top[v->childState[i]];
		int yy = v->childState[i];
		v = c;
		if (!v->notOver(xx,yy , success))
			break;
		i=rand()%v->childNum;
		
	}
	return success;
}

void MCT::backUp(Node* v,float delta)
{
	while(v!=NULL)
	{
		v->totalNum++;
		v->winNum+=delta;
		delta=-delta;
		v=v->p;
	}
}
