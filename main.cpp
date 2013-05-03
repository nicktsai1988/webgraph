#include <iostream>
#include <cstdlib>
#include <climits>
#include <ctime>
#include "bitvector.h"
#include "DecodeTree.h"
#include "Decompress.h"
#include "graph.h"
#include "HuffmanTree.h"
#include "Compress.h"
#include "creategraph.h"
using namespace std;
void print_node_info(const WebNode& node)
{
	cerr<<"print_node_info():nodeid= "<<node.pageid<<" linkscount= "<<node.linkscount<<endl;
}
void print_graph_info(const WebGraph &graph)
{
	unsigned int nodecount,edgecount,maxcount,minicount;
	nodecount=graph.nodecount-1;
	maxcount=edgecount=0;
	minicount=UINT_MAX;
	unsigned int i,j;
	for(i=1;i<graph.nodecount;i++)
	{
		edgecount+=graph.nodes[i].linkscount;
		if(graph.nodes[i].linkscount>maxcount)
			maxcount=graph.nodes[i].linkscount;
		if(graph.nodes[i].linkscount<minicount)
			minicount=graph.nodes[i].linkscount;
		for(j=1;j<graph.nodes[i].linkscount;j++)
		{
			if(graph.nodes[i].links[j-1]>graph.nodes[i].links[j])
				cerr<<"link error"<<endl;
		}
	}
	cerr<<"print_graph_info"<<endl;
	cerr<<"nodecout= "<<nodecount<<",edgecount= "<<edgecount<<endl;
	cerr<<"maxcount= "<<maxcount<<",minicount= "<<minicount<<endl;
	cerr<<"per-edge count= "<<edgecount/nodecount<<endl;
}
void check_encode_bitvector(const WebNode& node,const WebNode& reference)
{
	EnCode encode;
	encode.nodeid=node.pageid;
	encode.reference=reference.pageid;
	bool state=encode_bitvector(node,reference,encode);
	if(!state)
	{
		cerr<<"check_encode_bitvector() error"<<endl;
		exit(1);
	}
	unsigned int count=0;
	for(unsigned int i=0;i<encode.bitvector.length();++i)
	{
		if(encode.bitvector.test(i))
			++count;
	}
	cerr<<"count= "<<count<<endl;
	cerr<<"difference size= "<<encode.differences.size()<<endl;
	cout<<"outdegree of node "<<node.pageid<<" = "<<node.linkscount<<endl;
	cout<<"outdegree of reference "<<reference.pageid<<" = "<<reference.linkscount<<endl;
}
int main(int argc,char* argv[])
{
	using namespace std;
	unsigned int nodes;
	clock_t begin,end;
	if(argc==1)
		nodes=1000;
	else
		nodes=atoi(argv[1]);
	//CreateGraph BerkStanGraph("web-BerkStan.txt",CreateGraph::CONVERT);
	//BerkStanGraph.write_to_file("BerkStan.graph");
	//CreateGraph BerkStanGraph("BerkStan.graph",CreateGraph::ORIGIN);
	//print_graph_info(BerkStanGraph.instance());
	//CreateGraph graph("web-Stanford.txt",CreateGraph::CONVERT);
	//graph.write_to_file("Standford.graph");
	//CreateGraph graph2("Standford.graph",CreateGraph::ORIGIN);
	//graph2.write_to_file("Standford2.graph");
	
	const char* filename="test.graph";
//	cout<<"nodes= "<<nodes<<endl;
//	CreateGraph creator(nodes);
	//cerr<<"test.graph completed"<<endl;
	CreateGraph creator(filename,CreateGraph::ORIGIN);
	
	WebGraph& graph=creator.instance();
//	write_graph_to_file(filename,graph);
/*	
	cerr<<"write test.graph completed"<<endl;
	begin=clock();
	Compress test(graph);
	end=clock();
	cerr<<"time of compress graph= "<<(end-begin)<<endl;
	unsigned int bitcount=test.write_to_file("test");
	unsigned int linkscount=0;
	for(unsigned int i=0;i<graph.nodecount;i++)
	{
		linkscount+=graph.nodes[i].linkscount;
	}
	cout<<"per edge "<<bitcount*1.0/linkscount<<endl;
*/
	DeCompress decompress("test.comp");
	decompress.init("test.degree","test.node","test.pos");
	WebGraph graph2;
	graph2.nodes=new WebNode[decompress.size()+1];
	graph2.nodecount=decompress.size()+1;
	graph2.nodes[0].pageid=0;
	graph2.nodes[0].links=NULL;
	graph2.nodes[0].linkscount=0;
	std::vector<unsigned int> links;
	
	unsigned int pageid =1;
	begin=clock();
	decompress.setparameter(100,0.2);
	cerr<<"start decode node"<<endl;
	while(pageid<=decompress.size())
	{
		decompress.getlinks(pageid,links);
	//	print_node_info(graph.nodes[pageid]);
		if(links.size() != graph.nodes[pageid].linkscount)
		{
			cerr<<"decode node ="<<pageid<<" error!"<<endl;
			cerr<<"links.size()="<<links.size()<<",node["<<pageid<<"].linkscount=" <<graph.nodes[pageid].linkscount<<endl;;
			exit(1);
		}
		graph2.nodes[pageid].pageid=pageid;
		graph2.nodes[pageid].linkscount=links.size();
		graph2.nodes[pageid].links=new unsigned int[links.size()];
		for(unsigned int i=0;i<links.size();i++)
		{
			if(links[i] != graph.nodes[pageid].links[i])
			{	
				cerr<<"pageid="<<pageid<<",link can't match"<<endl;
				exit(1);
			}
			graph2.nodes[pageid].links[i]=links[i];
		}
		pageid++;
	}
	cerr<<"decompress.nodecount= "<<decompress.size()<<endl;
	cerr<<"decode right"<<endl;
	end=clock();
	cerr<<"time of decompress graph= "<<(end-begin)<<endl;
	write_graph_to_file("test2.graph",graph2);
	cerr<<"write decompressed graph completed"<<endl;
	/*
	cerr<<"*******************************************"<<endl;
	decompress.getlinks(2,links);
	cerr<<"********************************************"<<endl;
	decompress.getlinks(700,links);
	print_node_info(graph.nodes[700]);
	check_encode_bitvector(graph.nodes[1],graph.nodes[700]);
	/*
	WebNode node1,node2,root;
	node1.pageid=1;
	node1.linkscount=4;
	node1.links=new unsigned int[node1.linkscount];
	node1.links[0]=1;
	node1.links[1]=31;
	node1.links[2]=4;
	node1.links[3]=12;
	node2.pageid=2;
	node2.linkscount=7;
	node2.links=new unsigned int[node2.linkscount];
	node2.links[0]=1;
	node2.links[1]=2;
	node2.links[2]=3;
	node2.links[3]=5;
	node2.links[4]=7;
	node2.links[5]=8;
	node2.links[6]=9;
	root.pageid=3;
	root.linkscount=0;
	root.links=NULL;
	unsigned int nodecount=9;
	cout<<"difference(node1,node2)="<<difference(node1,node2)<<endl;
	cout<<"difference(node1,root)="<<difference(node1,root)<<endl;
	cout<<"cost(node1,node2)="<<cost(node1,node2,nodecount)<<endl;
	cout<<"cost(node1,root)="<<cost(node1,root,nodecount)<<endl;
	
	vector<char> links1,links2,links3,links4;
	links1.push_back('0');
	links2.push_back('1');
	links2.push_back('0');
	links3.push_back('1');
	links3.push_back('1');
	links3.push_back('0');
	links4.push_back('1');
	links4.push_back('1');
	links4.push_back('1');
	HuffNode node1,node2,node3,node4;
	node1.info='A';
	node1.vector=BitVector(links1);
	node2.info='B';
	node2.vector=BitVector(links2);
	node3.info='C';
	node3.vector=BitVector(links3);
	node4.info='D';
	node4.vector=BitVector(links4);
	vector<HuffNode> nodes;
	nodes.push_back(node1);
	nodes.push_back(node2);
	nodes.push_back(node3);
	nodes.push_back(node4);
	DecodeTree tree(nodes);
	unsigned int position=0;
	cout<<(char)tree.decode(BitVector(links1),&position);
	cout<<" position = "<<position<<endl;
	position=0;
	cout<<(char)tree.decode(BitVector(links2),&position);
	cout<<" position= "<<position<<endl;
	position=0;
	cout<<(char)tree.decode(BitVector(links3),&position);
	cout<<" position= "<<position<<endl;
	position=0;
	cout<<(char)tree.decode(BitVector(links4),&position);
	cout<<" position= "<<position<<endl;
	*/
	cout<<"completed"<<endl;
//	cin.get();
	return 0;
}

