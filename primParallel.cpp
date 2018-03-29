#include <iostream>       
#include <thread>
#include <ctime>
#include <stdlib.h>
#include <chrono>
#include <fstream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <omp.h>
#define V 6
#define p 2
using namespace std;

int graph[V][V] = { { 0, 2, 0, 6, 0 ,2},
			    	{ 2, 0, 3, 8, 5 ,2},
			    	{ 0, 3, 0, 0, 7 ,0},
			    	{ 6, 8, 0, 0, 9 ,3},
			    	{ 0, 5, 7, 9, 0 ,2},
			    	{ 2, 4, 0, 3, 2 , 0},
			    	};

int distanc[V];
int parent[V];
bool visited[V];
struct node
{
	int value;
	int index;
}feed[V];

int minkey(int p1, int p2)
{
	int min = INT_MAX, index;
	int num_threads;
	//#pragma omp parallel num_threads(V)
	//{
		//num_threads = omp_get_num_threads();
		//#pragma omp for 
			for (int i = p1; i <= p2; ++i)
				if(visited[i] == false && distanc[i] < min)
		//#pragma omp critical
					min = distanc[i], index = i;
	//}
	return index;
}

void primParallel(int p1, int p2, int i)
{
	//cout << "hi1"<<endl;
    int ptr = minkey(p1, p2);
    cout << " i = " << i << " :: " << distanc[ptr] << " :: " << ptr << endl;
    feed[i].value = distanc[ptr];

    feed[i].index = ptr;
}

void joining(int p1, int p2, int i)
{
    for(int v = p1; v <= p2;++v)
    {
    	if(graph[i][v] && visited[v] == false && graph[i][v] < distanc[v])
    	{
    		parent[v] = i, distanc[v] = graph[i][v];
    	}
    }
}

void printMST(int from[], int n, int graph[V][V])
    {
    	printf("Edge   Weight\n");
    	for (int i = 1; i < V; i++)
    		printf("%d - %d    %d \n", parent[i], i, graph[i][parent[i]]);
    }

int main()
{
	for(int i = 0;i<V;i++)
	{
		if(i % (V/p) == 0)
			distanc[i] = 0;
		else
			distanc[i] = INT_MAX;
		visited[i] = false;
		parent[i] = -1;
		feed[i].value = INT_MAX;
		feed[i].index = -1;
	}
	thread th[p];

	for(int count = 0; count < V -1; count++)
	{	
		for(int i = 0;i<p;i++){
			
			th[i] = thread(primParallel,i*(V/p), i*(V/p)+p, i);
		}
		cout << "hi" << endl;
		for(int i = 0;i<p;i++)
			th[i].join();

		cout << "hi1" << endl;
		struct node min;
		min.value = INT_MAX;

		int check = 0;
		for(int i = 0;i<V;i++)
		{
			if(feed[i].value < min.value)
			{
				min = feed[i];
				check = 1;
			}
			if(count == 0 && check == 1 && i % (V/p) == 0)              // note
				distanc[i] = INT_MAX;

			feed[i].value = INT_MAX;
			feed[i].index = -1;
		}
		visited[min.index] = true;

		cout << "adding "<< min.value << " index " << min.index << endl;

		for(int i = 0;i<p;i++)
			th[i] = thread(joining,i*(V/p), i*(V/p)+p, min.index);

		for(int i = 0;i<p;i++)
			th[i].join();

		printMST(parent, V, graph);
		cout << " ----------------------------------- " << endl;

	}

	printMST(parent, V, graph);

	return 0;
}