#define DEBUG
#include "Compress.h"
#include "edmonds.h"
#include <climits>
#include "utils.h"
#include "HuffmanTree.h"
#include <map>
#include <string>
#include <iostream>
#include <fstream>
using std::map;
using std::string;
using std::ofstream;
using std::cerr;
using std::ios_base;
Compress::Compress(const WebGraph& graph):_graph(graph)
{
	find_reference();
#ifdef DEBUG
	cerr<<"find_reference complete"<<endl;
#endif
	encode_outdegree();
#ifdef DEBUG
	cerr<<"encode_outdegree complete"<<endl;
#endif
	encode_nodesvector();
#ifdef DEBUG
	cerr<<"encode_nodevector complete"<<endl;
#endif
	create_result();
#ifdef DEBUG
	cerr<<"create_result complete"<<endl;
#endif
}


Compress::~Compress(void)
{
}

unsigned int Compress::write_to_file(const char* name)
{
	write_out_degrees(name);//保存出度数
	write_node_vector(name);//保存节点向量
	write_position(name);//保存位置信息
	return write_result(name);//保存压缩后的WEB图
}
/*
void Compress::find_reference()
{
	WebNode root;
	root.pageid=_graph.nodecount;
	root.links=NULL;
	root.linkscount=0;
	unsigned int size=_graph.nodecount+1;
	Edmonds obj(size);
	{
		Matrix<unsigned int> matrix(size,size,UINT_MAX);//行表示源点或者参考节点，列表示目标点或者被编码节点

		for(unsigned int j=0;j<_graph.nodecount;j++)
		{
			unsigned int limit=cost(_graph.nodes[j],root,_graph.nodecount); //以root编码节点j的开销
			matrix[_graph.nodecount][j]=limit;
			for(unsigned int i=0;i<_graph.nodecount;i++)
			{	
				if(i==j)
					continue;
				else
				{
					unsigned int temp=cost(_graph.nodes[j],_graph.nodes[i],_graph.nodecount);//以节点i编码节点j
					if(temp<limit)
						matrix[i][j]=temp;
				}
			}
		}
		for(unsigned int i=0;i<_graph.nodecount;i++)
			matrix[i][_graph.nodecount]=cost(root,_graph.nodes[i],_graph.nodecount);
#ifdef DEBUG
		cerr<<"cost matrix computer finished"<<endl;
#endif
		obj.init(matrix);
	}
	unsigned int rootid=_graph.nodecount;
	obj.find(&rootid);
	obj.getresult(&_reference);
}
*/
void Compress::find_reference()
{
	//WebNode root;
	//root.pageid=_graph.nodecount;
	//root.links=NULL;
	//root.linkscount=0;
	WebNode& root=_graph.nodes[0];
	unsigned int size=_graph.nodecount;
	vector<unsigned int> limit(_graph.nodecount);//存放限制的数组
	for(unsigned int i=1;i<_graph.nodecount;i++)
	{
		limit[i]=cost(_graph.nodes[i],root,_graph.nodecount-1);//以root节点编码节点i
	}
	Edmonds obj(size);
	{
		vector<RefLink> affinity;
		RefLink newlink;
		for(unsigned int i=1;i<_graph.nodecount;i++)//以根节点编码node[i]
		{		
			newlink.source=0;//根节点
			newlink.target=i;
			newlink.weight=limit[i];
			affinity.push_back(newlink);
		}
		for(unsigned int i=1;i<_graph.nodecount;i++)//以node[i]编码根节点
		{
			newlink.source=i;
			newlink.target=0;//根节点
			newlink.weight=cost(root,_graph.nodes[i],_graph.nodecount-1);
			affinity.push_back(newlink);
		}
		for(unsigned int j=1;j<_graph.nodecount;j++)//以node[i]编码node[j]
			for(unsigned int i=1;i<_graph.nodecount;i++)
			{
				if(i==j)
					continue;
				else
				{
					unsigned int temp=cost(_graph.nodes[j],_graph.nodes[i],_graph.nodecount-1);
					if(temp<limit[j])
					{
						newlink.source=i;
						newlink.target=j;
						newlink.weight=temp;
						affinity.push_back(newlink);
					}
				}
			}
#ifdef DEBUG
			cerr<<"affnity size= "<<affinity.size()<<endl;
			cerr<<"affnity links per node "<<affinity.size()*1.0/_graph.nodecount<<endl;
			cerr<<"cost matrix computer finished"<<endl;
#endif
		obj.init(affinity);
	}
	unsigned int rootid=0;
	obj.find(&rootid);
	obj.getresult(&_reference);
/*
#ifdef DEBUG
	cerr<<"Compress::find_reference()"<<endl;
	cerr<<"reference[1].source=  "<<_reference[1].source<<",referewnce[1].target= "<<_reference[1].target<<endl;
#endif
*/
}
void Compress::encode_outdegree()//为每个出度计算出哈夫曼编码
{
	typedef map<unsigned int,unsigned int> OutDegreeCount;
	OutDegreeCount outdegrees;
	OutDegreeCount::iterator it;
	for(unsigned int i=1;i<_graph.nodecount;i++)
	{
		it=outdegrees.find(_graph.nodes[i].linkscount);
			if(it!=outdegrees.end())//找到
			{
				it->second++;
			}
			else//没找到
				outdegrees.insert(OutDegreeCount::value_type(_graph.nodes[i].linkscount,1));
	}
	vector<InfoNode> degrees(outdegrees.size());
	unsigned int index=0;
	for(it=outdegrees.begin();it!=outdegrees.end();it++)
	{
		degrees[index].id=it->first;
		degrees[index].weight=it->second;
		index++;
	}
	cerr<<"degrees size()"<<degrees.size()<<endl;
	HuffmanTree obj(degrees);
	obj.saveresult(_outdegrees);
}

void Compress::encode_nodesvector()//为每个链接计算出哈夫曼编码
{
	//WebNode root;
	//root.pageid=_graph.nodecount;
	//root.linkscount=0;
	//root.links=NULL;
	WebNode& root=_graph.nodes[0];
	_encodes.resize(_graph.nodecount);
	for(unsigned int i=1;i<_encodes.size();i++)
		_encodes[i].nodeid=i;
	for(unsigned int i=0;i<_reference.size();i++)
	{
		if(_reference[i].source==0)//参考节点为根节点
		{
			bool state=encode_bitvector(_graph.nodes[_reference[i].target],root,_encodes[_reference[i].target]);
			if(!state)
			{
				cerr<<"encode_bitvector error"<<endl;
			}
		}
		else
		{
			bool state=encode_bitvector(_graph.nodes[_reference[i].target],_graph.nodes[_reference[i].source],_encodes[_reference[i].target]);
			if(!state)
			{
				cerr<<"encode_bitvector error"<<endl;
			}
		}
	}
	typedef map<unsigned int,unsigned int> NodeCount;
	NodeCount nodecount;
	NodeCount::iterator it;
	for(unsigned int i=1;i<_encodes.size();i++)//统计参考节点
	{
		it=nodecount.find(_encodes[i].reference);
		if(it==nodecount.end())//没找到
		{
			nodecount.insert(NodeCount::value_type(_encodes[i].reference,1));
		}
		else //找到
			it->second++;
	}
	for(unsigned int i=1;i<_encodes.size();i++)//统计差异节点
		for(unsigned int j=0;j<_encodes[i].differences.size();j++)
		{
			it=nodecount.find(_encodes[i].differences[j]);
			if(it==nodecount.end())//没找到
			{
				nodecount.insert(NodeCount::value_type(_encodes[i].differences[j],1));
			}
			else//找到
				it->second++;

		}
#ifdef DEBUG
	cerr<<"nodecount= "<<nodecount.size()<<endl;
#endif
	vector<InfoNode> nodes(nodecount.size());
	unsigned int index=0;
	for(it=nodecount.begin();it!=nodecount.end();it++)
	{
		nodes[index].id=it->first;
		nodes[index].weight=it->second;
		index++;
	}
	HuffmanTree obj(nodes);//生成哈夫曼编码
	obj.saveresult(_nodesvector);
}
void Compress::create_result()
{
	unsigned int linkscount;
	_position.clear();
	_position.resize(_graph.nodecount);
	_position[0].id=0;
	_position[0].position=0;
	for(unsigned int i=1;i<_encodes.size();i++)
	{
		encode_diffvector(_nodesvector,_encodes[i]);//编码diffvector
	}
	for(unsigned int i=1;i<_encodes.size();i++)
	{
		linkscount=_graph.nodes[_encodes[i].nodeid].linkscount;
		if(_encodes[i].nodeid != i)
		{
			cerr<<"_encodes[i].nodeid != i"<<endl;
			exit(1);
		}
		const BitVector *degree=find(_outdegrees,linkscount);
		const BitVector *ref=find(_nodesvector,_encodes[i].reference);

		if(degree == NULL || ref == NULL)
		{
			cerr<<"create_result error"<<endl;
			exit(1);
		}
		/*
#ifdef DEBUG
		if(i==1)
		{
			cerr<<"encodes[1].reference= "<<_encodes[1].reference<<endl;
			cerr<<"node[1].degree.length= "<<degree->length()<<endl;
			cerr<<"ref.length= "<<ref->length()<<endl;
			cerr<<"bitvector.length= "<<_encodes[1].bitvector.length()<<endl;
			cerr<<"diffvector.length= "<<_encodes[1].diffvector.length()<<endl;
			cerr<<"difference.length= "<<_encodes[1].differences.size()<<endl;
			cerr<<"difference is: "<<endl;
			for(unsigned int i=0;i<_encodes[1].differences.size();i++)
				cerr<<_encodes[1].differences[i]<<",";
			cerr<<endl;
		}
#endif
*/
		_position[_encodes[i].nodeid].id=_encodes[i].nodeid;
		_position[_encodes[i].nodeid].position=result.length();
		result.append(*degree).append(*ref).append(_encodes[i].bitvector).append(_encodes[i].diffvector);
		//出度+参考节点+位向量+差异部分向量
/*
#ifdef DEBUG
		if(i == 1)
			cerr<<"i==1,result.length= "<<result.length()<<endl;
#endif
*/
	}
}

void Compress::write_out_degrees(const char* name)//保存节点出度的位向量
{
	string filename(name);
	filename.append(".degree");
	ofstream fout;
	fout.open(filename.c_str(),ios_base::out|ios_base::binary);
	if(!fout.is_open())
	{
		cerr<<"write_out_degrees() error"<<endl;
		exit(1);
	}
	unsigned int size=_outdegrees.size();
	fout.write((char*)&size,sizeof(size));//记录下出度的个数
	unsigned short length;
	for(unsigned int i=0;i<_outdegrees.size();i++)
	{
		fout.write((char*)&(_outdegrees[i].info),sizeof(unsigned int)); //记录下出度
		length=_outdegrees[i].vector.length();
		fout.write((char*)&length,sizeof(length));//记录下位向量长度
		//_outdegrees[i].vector.write(fout);  //记录下位向量;
		fout<<_outdegrees[i].vector;
	}
	fout.close();
}
void Compress::write_node_vector(const char* name)//保存节点的位向量
{
	string filename(name);
	filename.append(".node");
	ofstream fout;
	fout.open(filename.c_str(),ios_base::out|ios_base::binary);
	if(!fout.is_open())
	{
		cerr<<"write_out_degrees() error"<<endl;
		exit(1);
	}
	unsigned int size=_nodesvector.size();
	fout.write((char*)&size,sizeof(size));//记录下节点个数
	unsigned short length;
	for(unsigned int i=0;i<_nodesvector.size();i++)
	{
		fout.write((char*)&_nodesvector[i].info,sizeof(unsigned int)); //记录下节点id
		length=_nodesvector[i].vector.length();   //位向量长度
		fout.write((char*)&length,sizeof(length));//记录下位向量长度
		fout<<_nodesvector[i].vector;        //记录下位向量
	}
	fout.close();
}
unsigned int Compress::write_result(const char* name)
{
	string filename(name);
	filename.append(".comp");
	ofstream fout;
	fout.open(filename.c_str(),ios_base::out|ios_base::binary);
	if(!fout.is_open())
	{
		cerr<<"write_result() error"<<endl;
		exit(1);
	}
	unsigned int nodecount=_graph.nodecount-1;//剔除跟节点
	fout.write((char*)&nodecount,sizeof(nodecount));//记录节点个数
	unsigned int length=result.length();       
	fout.write((char*)&length,sizeof(length)); //记录下位向量的位长度
	fout<<result;
	fout.close();
	return result.length();
}
void Compress::write_position(const char* name)
{
	unsigned int size;
	string filename(name);
	filename.append(".pos");
	ofstream fout;
	fout.open(filename.c_str(),ios_base::out|ios_base::binary);
	if(!fout.is_open())
	{
		cerr<<"write_position error"<<endl;
		exit(1);
	}
	size=_position.size()-1;
	fout.write((char*)&size,sizeof(size));
	for(unsigned int i=1;i<_position.size();i++)
	{
		fout.write((char*)&_position[i].id,sizeof(unsigned int));
		fout.write((char*)&_position[i].position,sizeof(unsigned int));
	}
	fout.close();
}

