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

struct Pair
{
	unsigned int multiplier;
	unsigned int remainer;
};


struct Interval
{
	Pair start;
	unsigned int length;
};


struct NodeEncoding
{
	unsigned int nodeid;
	unsigned int reference;
	unsigned int outdegree;
	//bool reference_flag;
	bool block_flag;//��ǿ�ĵ�һ��bitΪ0������Ϊ1
	bool interval_flag;//����Ƿ���interval
	bool start_flag;//��ǵ�һ��start������
	bool rest_flag;//�������ֵ
	bool rest_first_flag;//������ֵ
	std::vector<unsigned int> blocks;
	std::vector<Interval> intervals;
	std::vector<unsigned int> temp_rest_links;
	std::vector<unsigned int> rest_links_length;
	std::vector<unsigned int> rest_links;
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
double similarity(const CacheNode& node,const CacheNode& reference);//node��reference��ͬʱ���ڵĽڵ��������reference�еĽڵ�����
void print_graph_info(const WebGraph& graph);
int gap_encoding(EnCode& encode);
int find_ref(const WebGraph& graph,const unsigned int win,std::vector<RefLink>& result);
BitVector gamma_encoding(unsigned int x);
int block_encoding(const BitVector& vector,NodeEncoding &result);
int interval_encoding(const std::vector<unsigned int>& lists,NodeEncoding& result);
int rest_links_encoding(NodeEncoding& encoding);
unsigned int pair_to_number(const Pair &p);
Pair number_to_pair(unsigned int x);
int big_to_little_encoding(std::vector<unsigned int>& result,std::vector<unsigned int>&length,const std::vector<unsigned int>& lists);
#endif // UTILS_H
