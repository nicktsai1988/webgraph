#ifndef EDMONDS_H
#define EDMONDS_H
#include <vector>
#include <climits>
#include "matrix.h"
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "./src/edmonds_optimum_branching.hpp"
using std::vector;
#include "utils.h"

bool is_cyclic(const vector<RefLink>& result,unsigned int *root);
class Edmonds
{
public:
	typedef boost::property<boost::edge_weight_t, unsigned int>  EdgeProperty;
	typedef boost::adjacency_list<boost::vecS,
								boost::vecS,
								boost::directedS,
								boost::no_property,
								EdgeProperty>              Graph;
	typedef boost::graph_traits<Graph>::vertex_descriptor       Vertex;
	typedef boost::graph_traits<Graph>::vertex_iterator			Vertex_iterator;
	typedef boost::graph_traits<Graph>::edge_descriptor         Edge;
	
	Edmonds(unsigned int nodes);
	//void init(const vector<vector<unsigned int> >& matrix);  //���ڽӾ����ʼ��ͼ
	void init(const Matrix<unsigned int>& matrix);   //�þ����ʼ��ͼ
	void init(const vector<RefLink>& links);        //���ڽ����г�ʼ��ͼ
	void find(unsigned int *root); //�����ڵ��edmonds�㷨
	const unsigned int minicount();   //������С����ͼ��Ȩֵ
	void getresult(std::vector<RefLink>* links);   //��links�����ҵ�����С����ͼ
private:
	std::vector<Edge> result;  //��С����ͼ�Ľ��
	unsigned int nodes; //�ڵ����
	Graph G;         //ͼ
	boost::property_map<Graph, boost::edge_weight_t>::type weights;
	boost::property_map<Graph, boost::vertex_index_t>::type vertex_indices;
	unsigned int count;
};
#endif
