#ifndef BITVECTOR_H
#define BITVECTOR_H
#include<vector>
#include <iostream>
using std::vector;
class BitVector
{
public:
	BitVector(unsigned int length=0);               //Ĭ�Ϲ��캯��
//	BitVector(const char* filename);
	//BitVector(const unsigned char *data,size_t bitlength);
    BitVector(const vector<char> &list);//����Ϊ�ַ�1����0������
	BitVector(const char *list,size_t size);
    BitVector(const BitVector& bits);
    BitVector& operator=(const BitVector& bits);
    ~BitVector();
    BitVector& append(const vector<char>& list);
	BitVector& append(const char*,size_t size);
    BitVector& append(const BitVector& bits);
	BitVector& append(bool bit);
	int fill(std::ifstream& fin,size_t bitlength);
	void reset();
	BitVector get_sub(unsigned int position,unsigned int bitlength);
    void set(unsigned int index);
    void clr(unsigned int index);
    bool test(unsigned int index)const;
    unsigned int length() const;
	unsigned int write_to_file(const char*filename);
	//std::ostream& write(std::ostream& out);
	friend std::ostream& operator<<(std::ostream& out,const BitVector& vector);
private:
    void adjust_size(const unsigned int size);
    unsigned int bit_length;
    unsigned int char_length;
    unsigned char *bit_vector;
};

#endif // BITVECTOR_H
