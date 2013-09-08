#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H
#include "utils.h"

class HuffmanTree
{
private:
	struct HTNode
	{
		unsigned int id;
		unsigned int weight;
		unsigned int parent,lchild,rchild;
	};
	size_t nodesize;
	size_t treesize;
	HTNode * data;
	vector<HuffNode> _result;
	void select(unsigned int upbound,unsigned int *s1,unsigned int *s2);//��1-upbound��Ѱ��parentΪ0����weight��С�������ڵ㣬s1,s1���ؽ��
	unsigned int max_index(unsigned int array[],size_t size);
	unsigned int max(unsigned int x,unsigned int y);
public:
	HuffmanTree(const InfoNode* nodes,size_t size); //nodesΪ�ڵ����Ϣ��sizeΪ�ڵ����
	HuffmanTree(const vector<InfoNode>& nodes);
	bool getcode(unsigned int id,BitVector& code); //code�����Ӧ��id�ı��룬����ȡ�����״̬
	size_t saveresult(vector<HuffNode>& result);
	~HuffmanTree(void);
};
#endif //HUFFMANTREE_H
