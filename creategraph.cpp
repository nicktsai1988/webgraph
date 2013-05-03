#include "creategraph.h"
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<ctime>
using std::cerr;
using std::endl;
using std::ofstream;
using std::ios_base;
using std::ifstream;
CreateGraph::CreateGraph(unsigned int nodes,unsigned int exist)
{
    graph.nodecount=nodes+1; //���еĽڵ���ϸ��ڵ�,�ڵ�0Ϊ���ڵ�
    if(exist==0)
        this->exist=(nodes/10);
    else
        this->exist=exist;
    srand(time(0));  //�������������
    init();
    create();
    sort();
}//ָ���ڵ�����Ϳ�ʼʱ���ڵ�ҳ�������Ĭ�Ͽ�ʼ���ڵ�Ϊ�ڵ������1/10

CreateGraph::~CreateGraph()
{

}
//��ʼ��WEBͼ

CreateGraph::CreateGraph(const char *filename,Format format)
{
	/*
    ifstream fin;
    unsigned int nodecount,i;
    fin.open(filename,ios_base::in|ios_base::binary);
    if(!fin.is_open())
    {
        cerr<<"CreateGraph() error,cannot open "<<filename<<endl;
        exit(1);
    }
    fin.read((char*)&nodecount,sizeof(nodecount));
    graph.nodecount=nodecount+1;
    if((graph.nodes=new WebNode[graph.nodecount])==NULL)
    {
        cerr<<"new webnode error\n";
        exit(1);
    }
	graph.nodes[0].pageid=0;
	graph.nodes[0].linkscount=0;
	graph.nodes[0].links=NULL;
    for(i=1;i<=(graph.nodecount-1);i++)
    {
        unsigned int linkscount;
        graph.nodes[i].pageid=i;
        fin.read((char*)&linkscount,sizeof(unsigned int));
        graph.nodes[i].linkscount=linkscount;
        if((graph.nodes[i].links=new unsigned int[linkscount])==NULL)
        {
            cerr<<"new links error\n";
            exit(1);
        }
        fin.read((char*)(graph.nodes[i].links),linkscount*sizeof(unsigned int));
    }
    fin.close();
	*/
	if(format == ORIGIN)
		load_origin_graph(filename);
	else
		if(format == CONVERT)
			load_convert_graph(filename);
		else
		{
			cerr<<"format argument error"<<endl;
			exit(1);
		}
}

void CreateGraph::load_origin_graph(const char* filename)
{
	ifstream fin;
	unsigned int nodecount,i;
	fin.open(filename,ios_base::in|ios_base::binary);
	if(!fin.is_open())
	{
		cerr<<"CreateGraph() error,cannot open "<<filename<<endl;
		exit(1);
	}
	fin.read((char*)&nodecount,sizeof(nodecount));
	graph.nodecount=nodecount+1;
	if((graph.nodes=new WebNode[graph.nodecount])==NULL)
	{
		cerr<<"new webnode error\n";
		exit(1);
	}
	graph.nodes[0].pageid=0;
	graph.nodes[0].linkscount=0;
	graph.nodes[0].links=NULL;
	for(i=1;i<=(graph.nodecount-1);i++)
	{
		unsigned int linkscount;
		graph.nodes[i].pageid=i;
		fin.read((char*)&linkscount,sizeof(unsigned int));
		graph.nodes[i].linkscount=linkscount;
		if((graph.nodes[i].links=new unsigned int[linkscount])==NULL)
		{
			cerr<<"new links error\n";
			exit(1);
		}
		fin.read((char*)(graph.nodes[i].links),linkscount*sizeof(unsigned int));
	}
	fin.close();
}
void CreateGraph::load_convert_graph(const char* filename)
{
	ifstream fin;
	unsigned int from,to;
	unsigned int nodecount,edgecount;
	char buf[256];
	NodeLink *nodelink;
	fin.open(filename);
	if(!fin.is_open())
	{
		cerr<<"CreateGraph::load_convert_graph() error,cannot open "<<filename<<endl;
		exit(1);
	}
	fin.getline(buf,256);//# Directed graph (each unordered pair of nodes is saved once): web-Stanford.txt 
	fin.getline(buf,256);//# Stanford web graph from 2002
	fin>>buf;//#
	fin>>buf;//Nodes
	fin>>nodecount;//nodecount
	fin>>buf;//Edges
	fin>>edgecount;//edgecount
	fin.getline(buf,256);//'\n'
	fin.getline(buf,256);//# FromNodeId	ToNodeId
	if((nodelink = new NodeLink[nodecount+1]) == NULL)
	{
		cerr<<"new NodeLink error"<<endl;
		exit(1);
	}
	unsigned int temp=edgecount;
	while(temp--)
	{
		fin>>from>>to;
		nodelink[from].links.push_back(to);
	}
	//cerr<<"form= "<<from<<" to="<<to<<endl;
	fin.close();
	cerr<<"read text graph completed"<<endl;
	graph.nodecount=nodecount+1;
	if((graph.nodes=new WebNode[graph.nodecount])==NULL)
	{
		cerr<<"new webnode error\n";
		exit(1);
	}
	//unsigned int count=0;
	graph.nodes[0].pageid=0;
	graph.nodes[0].linkscount=0;
	graph.nodes[0].links=NULL;
	for(unsigned int i=1;i<graph.nodecount;++i)
	{
		graph.nodes[i].pageid=i;
		graph.nodes[i].linkscount=nodelink[i].links.size();
		//count+=nodelink[i].links.size();
		if((graph.nodes[i].links = new unsigned int[graph.nodes[i].linkscount]) == NULL)
		{
			cerr<<"new links error"<<endl;
			exit(1);
		}
		for(unsigned int j=0;j<graph.nodes[i].linkscount;++j)
		{
			graph.nodes[i].links[j]=nodelink[i].links[j];
		}
	}
	delete[] nodelink;
	sort();
	std::cerr<<"nodecount= "<<nodecount<<" edgecount= "<<edgecount<<std::endl;
	//cerr<<"count= "<<count<<endl;
}

void CreateGraph::init() //����ռ�ͳ�ʼ��
{
    unsigned int i;
    if((graph.nodes=new WebNode[graph.nodecount])==NULL)
    {
        cerr<<"CreateGraph init() error\n";
        exit(1);
    }
    for(i=0;i<graph.nodecount;i++)
        graph.nodes[i].links=NULL;
	graph.nodes[0].pageid=0;
	graph.nodes[0].linkscount=0;
}
//ĳ�������Ƿ��Ѿ�����

bool CreateGraph::have_existed(const unsigned int *links, const unsigned int size,const unsigned int element)
{
    unsigned int i;
    for(i=0;i<size;i++)
    {
        if(links[i]==element)
            return true;
    }
    return false;
}
/*
void CreateGraph::write_to_file(const char *filename)
{
    unsigned int i;
    ofstream fout;
	unsigned int count=graph.nodecount-1;//�޳����ڵ�,�ڵ�ID��1��ʼ��count
    fout.open(filename,ios_base::out|ios_base::binary);
    if(!fout.is_open())
    {
        cerr<<"open file "<<filename<<" error\n";
        exit(1);
    }
    fout.write((char*)&(count),sizeof(unsigned int));  //���涥�����
    for(i=1;i<graph.nodecount;i++)
    {
        //����˳�򱣴�
        fout.write((char*)&(graph.nodes[i].linkscount),sizeof(unsigned int)); //���涥������Ӹ���
        fout.write((char*)(graph.nodes[i].links),sizeof(unsigned int)*(graph.nodes[i].linkscount));  //���涥�������
    }
    fout.close();
}
*/
void CreateGraph::create()
{

    unsigned int i,j;
    for(i=1;i<=exist;i++)//�ȳ�ʼ�����ڵ�ҳ�棬Ȼ���ٸ��ݴ��ڵ�ҳ����������ҳ��
    {
        graph.nodes[i].pageid=i;//����ҳ��ID
        graph.nodes[i].linkscount=link_count();//���Ӹ���Ϊ50~150
        if((graph.nodes[i].links=new unsigned int[graph.nodes[i].linkscount])==NULL)
        {
            std::cerr<<"creategraph create() error\n";
            exit(1);
        }//Ϊÿ��ҳ������ӷ���ռ�
        for(j=0;j<graph.nodes[i].linkscount;j++)//Ϊÿ��ҳ���������
        {
            graph.nodes[i].links[j]=new_link(graph.nodes[i].links,j);
        }
    }
    for(i=exist+1;i <= (graph.nodecount-1);i++)//Ϊ�����ҳ�����洢�ռ������
    {
        graph.nodes[i].pageid=i;
        graph.nodes[i].linkscount=link_count();
        if((graph.nodes[i].links=new unsigned int[graph.nodes[i].linkscount])==NULL)
        {
            std::cerr<<"creategraph create() error\n";
            exit(1);
        }//Ϊÿ��ҳ������ӷ���ռ�
        copy_links(graph.nodes[i],i);
    }
}
unsigned int CreateGraph::link_count()
{
    return (rand()%150+50);
}
unsigned int CreateGraph::new_link(const unsigned int *links, const unsigned int size)
{
    unsigned int temp=(rand()%(graph.nodecount-1))+1; //����IDΪ1~(nodecount-1)������
    while(have_existed(links,size,temp))  //��������ĳ�������򷵻أ��������ѡ��
    {
        temp=(rand()%graph.nodecount);
    }
    return temp;
}
unsigned int CreateGraph::new_link(const unsigned int *links, const unsigned int size, unsigned int seed)
{

    int temp1=rand()%100; //��80%�Ļ�����ԭ�������ӣ�20%������仯
    if(temp1<80)
        return seed;
    else
    {
        unsigned int temp2=(rand()%(graph.nodecount-1))+1; //ȡ��ȥ���ڵ�IDΪ���Ľڵ�
        while(have_existed(links,size,temp2))
        {
            temp2=(rand()%(graph.nodecount-1)+1);
        }
        return temp2;
    }
}

void CreateGraph::copy_links(WebNode &node, const unsigned int limit)
{
    unsigned int pageid,linkscount;
    unsigned int subcount;
    unsigned int count=0;
    unsigned int current;
    while(1)
    {
        pageid=(rand()%(limit-1))+1; //���Ѿ����ڵ�ҳ�������µ�ҳ��,�����޳�IDΪ0��ҳ��
        linkscount=graph.nodes[pageid].linkscount;
        subcount=(rand()%linkscount+1); //������Ҫ���Ƶ����Ӹ���
        if(subcount>(node.linkscount-count))
            subcount=(node.linkscount-count);
        if(subcount==0)
        {
            break;
        }
        for(unsigned int i=0;i<subcount;i++)
        {
            current=graph.nodes[pageid].links[rand()%linkscount];
            unsigned int temp=0;
            while(temp<100&&have_existed(node.links,count,current))
            {
                  current=graph.nodes[pageid].links[rand()%linkscount];
                  temp++;
            }
            if(temp==100)//�������100����һ���ο��Ľڵ�
                break;
            else       //���򽫸�������ӵ��µ�ҳ����
            {
                node.links[count]=new_link(node.links,count,current);
                count++;
            }
        }
    }
}
void CreateGraph::sort()
{
    unsigned int i;
    for(i=1;i<graph.nodecount;i++)
    {
        qsort(graph.nodes[i].links,graph.nodes[i].linkscount,sizeof(unsigned int),cmp);
    }
}

int CreateGraph::cmp(const void* x, const void* y)
{
    return *(int *)x-*(int *)y;
}

WebGraph& CreateGraph::instance()
{
    return graph;
}






