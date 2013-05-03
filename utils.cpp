#include "utils.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "graph.h"
bool encode_bitvector(const WebNode& node,const WebNode& reference_node,EnCode & code)
	//��referenceΪ�ο��ڵ����node���������code֮��
{
	if(node.pageid!=code.nodeid)
		return false;
	else
	{
		unsigned int i,j,k;
		code.differences.clear();
		BitVector bitvector(reference_node.linkscount);
		code.reference=reference_node.pageid;
		code.nodeid=node.pageid;

		for(i=0,j=0;i<node.linkscount;i++)
		{
			while((j<reference_node.linkscount)&&(node.links[i]>reference_node.links[j]))
				bitvector.clr(j++);
			if(j==reference_node.linkscount)//�ο��ڵ��α��Ѿ���ĩβ
			{
				for(k=i;k<node.linkscount;k++)//��node�ж�������Ӽӵ�difference����
					code.differences.push_back(node.links[k]);
				break;
			}
			if(node.links[i]==reference_node.links[j]) //������
				bitvector.set(j++);						//�򽫸�λ��1
			else
				code.differences.push_back(node.links[i]); //С�ڣ������difference
		}
		for(k=j;k<reference_node.linkscount;k++)//�ο��ڵ㻹δ��ĩβ
			bitvector.clr(k);
		code.bitvector=bitvector;
		return true;
	}	
}

const BitVector* find(const vector<HuffNode> &huffcode,unsigned int key)//�����޸����Ч��
{
	for(unsigned int i=0;i<huffcode.size();i++)
	{
		if(huffcode[i].info == key)
			return &(huffcode[i].vector);
	}
		return NULL;
}

void encode_diffvector(const vector<HuffNode>& huffcode,EnCode &code)
{
	unsigned int i;
	code.diffvector.reset();
	for(i=0;i<code.differences.size();i++)
	{
		const BitVector* bits=find(huffcode,code.differences[i]);
		if(bits != NULL)
			code.diffvector.append(*bits);
		else
		{
			std::cerr<<"encode_diffvector error"<<std::endl;
			exit(1);
		}
	}
}

unsigned int difference(const WebNode& node1,const WebNode& node2)//������node1ȴ��������node2�е����Ӹ���
{
	unsigned int count=0;
	unsigned int i,j;
	for(i=0,j=0;i<node1.linkscount;i++)
	{
		while((j<node2.linkscount)&&(node1.links[i]>node2.links[j]))
			j++;
		if(j==node2.linkscount) //node2���ȱȽ϶̣��ѵ�β��
		{
			count+=(node1.linkscount-i);
			break;
		}
		if(node1.links[i]<node2.links[j])
			count++;
	}
	return count;
}
/*
double log2(double x)
{
	return log(x)/log(2.0);
}
*/
unsigned int cost(const WebNode& node1,const WebNode& node2,unsigned int nodecount)
{
	 return node2.linkscount+((unsigned int)(ceil(log2((double)nodecount))))*(difference(node1,node2)+1);
}//node2��Ϊ�ο��������node1�Ŀ���
void write_graph_to_file(const char* filename,const WebGraph& graph)
{
	unsigned int i;
	std::ofstream fout;
	unsigned int count=graph.nodecount-1;//�޳����ڵ�,�ڵ�ID��1��ʼ��count
    fout.open(filename,std::ios_base::out|std::ios_base::binary);
    if(!fout.is_open())
    {
		std::cerr<<"open file "<<filename<<" error\n";
        exit(1);
    }
    fout.write((char*)&(count),sizeof(unsigned int));  //���涥�����
    for(i=1;i<graph.nodecount;++i)
    {
        //����˳�򱣴�
        fout.write((char*)&(graph.nodes[i].linkscount),sizeof(unsigned int)); //���涥������Ӹ���
        fout.write((char*)(graph.nodes[i].links),sizeof(unsigned int)*(graph.nodes[i].linkscount));  //���涥�������
    }
    fout.close();
}
double similarity(const std::vector<unsigned int>& links,const std::vector<unsigned int>& reference)
{//links��reference��ͬʱ���ڵĽڵ��������reference�еĽڵ�����
	unsigned int i,j;
	double count=0;
	for(i=0,j=0;i<links.size();++i)
	{
		while(j<reference.size() && links[i] > reference[j])
		{
			j++;
		}
		if(j == reference.size())
		{
			break;
		}
		if(links[i] == reference[j])//����
		{
			j++;
			count++;
		}
	}
	return count/(double)reference.size();
}




