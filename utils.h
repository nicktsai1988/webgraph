#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include "bitvector.h"
class WebGraph;
struct WebNode     //WEB图中的节点
{
    unsigned int pageid;
    unsigned int linkscount;
    unsigned int *links;
};
struct RefLink //参考节点 
{
	unsigned int source;
	unsigned int target;
	unsigned int weight;
};
struct EnCode   //参考编码
{
	unsigned int nodeid;   //被编码节点
	unsigned int reference; //参考节点
	BitVector bitvector;   //位向量
	std::vector<unsigned int> differences;  //差异部分
	BitVector diffvector;   //差异部分的向量
};

struct HuffNode       //节点对应的哈夫曼编码
{
	unsigned int info; //此哈夫曼编码对应的信息
	BitVector vector;  //哈夫曼编码
};

struct InfoNode    //信息节点
{
	unsigned int id;
	unsigned int weight;
};
struct RefPosition//参考节点的位置
{
	unsigned int id;
	unsigned int position;
};
struct CacheNode
{
	unsigned int nodeid;
	std::vector<unsigned int> links;
};
//必须先调用encode_bitvector()，在调用encode_diffvector()；
bool encode_bitvector(const WebNode& node,const WebNode& reference_node,EnCode &code);
const BitVector* find(const vector<HuffNode> &huffcode,unsigned int key);
void encode_diffvector(const vector<HuffNode>& huffcode,EnCode &code);
unsigned int difference(const WebNode& node1,const WebNode& node2);
//double log2(double x);
unsigned int cost(const WebNode& node1,const WebNode& node2,unsigned int nodecount);
void write_graph_to_file(const char* filename,const WebGraph& graph);
double similarity(const std::vector<unsigned int>& links,const std::vector<unsigned int>& reference);//node与reference中同时存在的节点个数除以reference中的节点总数,在计算cache中使用
#endif // UTILS_H
