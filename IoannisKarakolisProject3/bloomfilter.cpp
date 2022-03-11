#include <iostream>
#include <cstring>
#include "modules.h"

unsigned long djb2(unsigned char *str) 
{
	unsigned long hash = 5381;
	int c; 
	while (c = *str++) {
		hash = ((hash << 5) + hash) + c; 
	}
	return hash;
}

unsigned long sdbm(unsigned char *str) 
{
	unsigned long hash = 0;
	int c;

	while (c = *str++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

unsigned long hash_i(unsigned char *str, unsigned int i)
{
	return djb2(str) + i*sdbm(str) + i*i;
}

void BloomFilter::insert(std::string& str)
{
    char *s= new char[str.length()+1];
    strcpy(s,str.c_str());
    int i = 0;
    for (i = 0 ; i < this->K ; i++) {
     	array.setbit(hash_i((unsigned char *)s, i)%array.size());
    }
    delete[] s;
}
int BloomFilter:: lookup(std::string& str)
{
    char *s= new char[str.length()+1];
    strcpy(s,str.c_str());
    int i = 0;

   for (i = 0 ; i < this->K ; i++) {
       	if(array[hash_i((unsigned char *)s, i)%array.size()]!=1)
        {
            delete[] s;
            return 0;
        }
	}
    delete[] s;
    return 1;
}

void BloomFilter:: print()
{
    int i = 0;
    std::cout<<array.size()<<" "<<array.getbitmatrixsize()<<"\n";
   for (i = 0 ; i < array.size(); i++) {
       std::cout<<array[i]<<"\n";
	}
}