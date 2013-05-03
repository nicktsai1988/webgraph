#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include "bitvector.h"
class WebGraph;
struct WebNode     //WEBͼ�еĽڵ�
{
    unsigned int pageid;
    unsigned int linkscount;
    unsigned int *links;
};
struct RefLink //�ο��ڵ� 
{
	unsigned int source;
	unsigned int target;
	unsigned int weight;
};
struct EnCode   //�ο�����
{
	unsigned int nodeid;   //������ڵ�
	unsigned int reference; //�ο��ڵ�
	BitVector bitvector;   //λ����
	std::vector<unsigned int> differences;  //���첿��
	BitVector diffvector;   //���첿�ֵ�����
};

struct HuffNode       //�ڵ��Ӧ�Ĺ���������
{
	unsigned int info; //�˹����������Ӧ����Ϣ
	BitVector vector;  //����������
};

struct InfoNode    //��Ϣ�ڵ�
{
	unsigned int id;
	unsigned int weight;
};
struct RefPosition//�ο��ڵ��λ��
{
	unsigned int id;
	unsigned int position;
};
struct CacheNode
{
	unsigned int nodeid;
	std::vector<unsigned int> links;
};
//�����ȵ���encode_bitvector()���ڵ���encode_diffvector()��
bool encode_bitvector(const WebNode& node,const WebNode& reference_node,EnCode &code);
const BitVector* find(const vector<HuffNode> &huffcode,unsigned int key);
void encode_diffvector(const vector<HuffNode>& huffcode,EnCode &code);
unsigned int difference(const WebNode& node1,const WebNode& node2);
//double log2(double x);
unsigned int cost(const WebNode& node1,const WebNode& node2,unsigned int nodecount);
void write_graph_to_file(const char* filename,const WebGraph& graph);
double similarity(const std::vector<unsigned int>& links,const std::vector<unsigned int>& reference);//node��reference��ͬʱ���ڵĽڵ��������reference�еĽڵ�����,�ڼ���cache��ʹ��
#endif // UTILS_H
