#include "utils.h"
#include<iostream>
using namespace std;
int main()
{
	CacheNode node1,node2;
	//node1.linkscount=4;
	node1.links=new unsigned int[4];
	node1.links[0]=3;
	node1.links[1]=4;
	node1.links[2]=6;
	node1.links[3]=7;
	node2.linkscount=5;
	node2.links=new unsigned int[5];
	node2.links[0]=1;
	node2.links[1]=2;
	node2.links[2]=4;
	node2.links[3]=5;
	node2.links[4]=6;

	cout<<similarity(node1,node2)<<endl;
	delete[] node1.links;
	delete[] node2.links;
	return 0;
}
