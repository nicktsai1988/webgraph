#include "utils.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "graph.h"
bool encode_bitvector(const WebNode& node,const WebNode& reference_node,EnCode & code)
	//以reference为参考节点编码node，结果存于code之中
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
			if(j==reference_node.linkscount)//参考节点游标已经到末尾
			{
				for(k=i;k<node.linkscount;k++)//将node中多余的连接加到difference部分
					code.differences.push_back(node.links[k]);
				break;
			}
			if(node.links[i]==reference_node.links[j]) //如果相等
				bitvector.set(j++);						//则将该位置1
			else
				code.differences.push_back(node.links[i]); //小于，则加入difference
		}
		for(k=j;k<reference_node.linkscount;k++)//参考节点还未到末尾
			bitvector.clr(k);
		code.bitvector=bitvector;
		return true;
	}	
}

const BitVector* find(const vector<HuffNode> &huffcode,unsigned int key)//可以修改提高效率
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

unsigned int difference(const WebNode& node1,const WebNode& node2)//存在于node1却不存在于node2中的链接个数
{
	unsigned int count=0;
	unsigned int i,j;
	for(i=0,j=0;i<node1.linkscount;i++)
	{
		while((j<node2.linkscount)&&(node1.links[i]>node2.links[j]))
			j++;
		if(j==node2.linkscount) //node2长度比较短，已到尾部
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
}//node2作为参考编码编码node1的开销
void write_graph_to_file(const char* filename,const WebGraph& graph)
{
	unsigned int i;
	std::ofstream fout;
	unsigned int count=graph.nodecount-1;//剔除根节点,节点ID从1开始到count
    fout.open(filename,std::ios_base::out|std::ios_base::binary);
    if(!fout.is_open())
    {
		std::cerr<<"open file "<<filename<<" error\n";
        exit(1);
    }
    fout.write((char*)&(count),sizeof(unsigned int));  //保存顶点个数
    for(i=1;i<graph.nodecount;++i)
    {
        //按照顺序保存
        fout.write((char*)&(graph.nodes[i].linkscount),sizeof(unsigned int)); //保存顶点的链接个数
        fout.write((char*)(graph.nodes[i].links),sizeof(unsigned int)*(graph.nodes[i].linkscount));  //保存顶点的链接
    }
    fout.close();
}
double similarity(const std::vector<unsigned int>& links,const std::vector<unsigned int>& reference)
{//links与reference中同时存在的节点个数除以reference中的节点总数
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
		if(links[i] == reference[j])//等于
		{
			j++;
			count++;
		}
	}
	return count/(double)reference.size();
}




