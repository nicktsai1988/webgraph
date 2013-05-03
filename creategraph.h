#ifndef CREATEGRAPH_H
#define CREATEGRAPH_H
#include "graph.h"
class CreateGraph
{
public:
	enum Format{ORIGIN,CONVERT};
    CreateGraph(unsigned int,unsigned int exist=0);
    CreateGraph(const char* filename,Format format);
	~CreateGraph();
    WebGraph& instance();
   // void write_to_file(const char* filename);
private:
	struct NodeLink
	{
		vector<unsigned int> links;
	};
	void load_origin_graph(const char* filename);
	void load_convert_graph(const char* filename);
    void init();
    void create();
    bool have_existed(const unsigned int *links,const unsigned int size,const unsigned int element);
    unsigned int new_link(const unsigned int* links,const unsigned int size);
    unsigned int new_link(const unsigned int* links,const unsigned int size,unsigned int seed); //����seed����
    unsigned int link_count();
    void copy_links(WebNode& node,const unsigned int limit); //limitΪ�Ѿ����ڵ�ҳ��ĸ���
    void sort();
    static int cmp(const void* x,const void* y);
    unsigned int exist;
    WebGraph graph;
};

#endif // CREATEGRAPH_H
