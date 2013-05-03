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
	write_out_degrees(name);//���������
	write_node_vector(name);//����ڵ�����
	write_position(name);//����λ����Ϣ
	return write_result(name);//����ѹ�����WEBͼ
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
		Matrix<unsigned int> matrix(size,size,UINT_MAX);//�б�ʾԴ����߲ο��ڵ㣬�б�ʾĿ�����߱�����ڵ�

		for(unsigned int j=0;j<_graph.nodecount;j++)
		{
			unsigned int limit=cost(_graph.nodes[j],root,_graph.nodecount); //��root����ڵ�j�Ŀ���
			matrix[_graph.nodecount][j]=limit;
			for(unsigned int i=0;i<_graph.nodecount;i++)
			{	
				if(i==j)
					continue;
				else
				{
					unsigned int temp=cost(_graph.nodes[j],_graph.nodes[i],_graph.nodecount);//�Խڵ�i����ڵ�j
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
	vector<unsigned int> limit(_graph.nodecount);//������Ƶ�����
	for(unsigned int i=1;i<_graph.nodecount;i++)
	{
		limit[i]=cost(_graph.nodes[i],root,_graph.nodecount-1);//��root�ڵ����ڵ�i
	}
	Edmonds obj(size);
	{
		vector<RefLink> affinity;
		RefLink newlink;
		for(unsigned int i=1;i<_graph.nodecount;i++)//�Ը��ڵ����node[i]
		{		
			newlink.source=0;//���ڵ�
			newlink.target=i;
			newlink.weight=limit[i];
			affinity.push_back(newlink);
		}
		for(unsigned int i=1;i<_graph.nodecount;i++)//��node[i]������ڵ�
		{
			newlink.source=i;
			newlink.target=0;//���ڵ�
			newlink.weight=cost(root,_graph.nodes[i],_graph.nodecount-1);
			affinity.push_back(newlink);
		}
		for(unsigned int j=1;j<_graph.nodecount;j++)//��node[i]����node[j]
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
void Compress::encode_outdegree()//Ϊÿ�����ȼ��������������
{
	typedef map<unsigned int,unsigned int> OutDegreeCount;
	OutDegreeCount outdegrees;
	OutDegreeCount::iterator it;
	for(unsigned int i=1;i<_graph.nodecount;i++)
	{
		it=outdegrees.find(_graph.nodes[i].linkscount);
			if(it!=outdegrees.end())//�ҵ�
			{
				it->second++;
			}
			else//û�ҵ�
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

void Compress::encode_nodesvector()//Ϊÿ�����Ӽ��������������
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
		if(_reference[i].source==0)//�ο��ڵ�Ϊ���ڵ�
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
	for(unsigned int i=1;i<_encodes.size();i++)//ͳ�Ʋο��ڵ�
	{
		it=nodecount.find(_encodes[i].reference);
		if(it==nodecount.end())//û�ҵ�
		{
			nodecount.insert(NodeCount::value_type(_encodes[i].reference,1));
		}
		else //�ҵ�
			it->second++;
	}
	for(unsigned int i=1;i<_encodes.size();i++)//ͳ�Ʋ���ڵ�
		for(unsigned int j=0;j<_encodes[i].differences.size();j++)
		{
			it=nodecount.find(_encodes[i].differences[j]);
			if(it==nodecount.end())//û�ҵ�
			{
				nodecount.insert(NodeCount::value_type(_encodes[i].differences[j],1));
			}
			else//�ҵ�
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
	HuffmanTree obj(nodes);//���ɹ���������
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
		encode_diffvector(_nodesvector,_encodes[i]);//����diffvector
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
		//����+�ο��ڵ�+λ����+���첿������
/*
#ifdef DEBUG
		if(i == 1)
			cerr<<"i==1,result.length= "<<result.length()<<endl;
#endif
*/
	}
}

void Compress::write_out_degrees(const char* name)//����ڵ���ȵ�λ����
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
	fout.write((char*)&size,sizeof(size));//��¼�³��ȵĸ���
	unsigned short length;
	for(unsigned int i=0;i<_outdegrees.size();i++)
	{
		fout.write((char*)&(_outdegrees[i].info),sizeof(unsigned int)); //��¼�³���
		length=_outdegrees[i].vector.length();
		fout.write((char*)&length,sizeof(length));//��¼��λ��������
		//_outdegrees[i].vector.write(fout);  //��¼��λ����;
		fout<<_outdegrees[i].vector;
	}
	fout.close();
}
void Compress::write_node_vector(const char* name)//����ڵ��λ����
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
	fout.write((char*)&size,sizeof(size));//��¼�½ڵ����
	unsigned short length;
	for(unsigned int i=0;i<_nodesvector.size();i++)
	{
		fout.write((char*)&_nodesvector[i].info,sizeof(unsigned int)); //��¼�½ڵ�id
		length=_nodesvector[i].vector.length();   //λ��������
		fout.write((char*)&length,sizeof(length));//��¼��λ��������
		fout<<_nodesvector[i].vector;        //��¼��λ����
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
	unsigned int nodecount=_graph.nodecount-1;//�޳����ڵ�
	fout.write((char*)&nodecount,sizeof(nodecount));//��¼�ڵ����
	unsigned int length=result.length();       
	fout.write((char*)&length,sizeof(length)); //��¼��λ������λ����
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

