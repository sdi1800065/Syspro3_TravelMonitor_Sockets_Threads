#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "modules.h"

//Hash function.
int h(char* K,int TABLESIZE)
{
  int h=0, a=127;
  for (; *K!='\0'; K++)
    h=(a*h + *K) % TABLESIZE;

  return h;
}

//Inserts a node at the end of a list.
void Insertatstart(Hashnode *&list,std::string & key ,HTItem A)
{
  Hashnode *N;
  N=new Hashnode;

  N->item=A;
  N->key=key;

  N->Link = list; // Link address part
  list=N;          // Make newNode as first node
 
}

//Secondary function to rehash.
void NewTable(HTHash& table1)
{
  HTHash table=table1;
  int Nsize = 2*(table[0].size)+1;

  HTHash table2=HTCreate(Nsize);
  int i,a=table[0].size;
  for(i=0;i<a;i++)
  {
    if(table[i].list!=NULL)
    {
      Hashnode *templist=table[i].list;
      while(templist!=NULL)
      {
        Hashnode *temp=templist;
        HTInsert(table2,templist->key,templist->item);
        templist=templist->Link;
        delete temp;
      }
    }
  }
HTHash temp=table1;
table1=table2;
delete[] temp;
}

//Inserts a key (with its item) in the hash (replace if exists).
void HTInsert(HTHash &table1,std::string & key,HTItem item)
{
  
  float loadfactor=((float)table1[0].num_elements/table1[0].size);

  table1[0].num_elements++;
  if(0.9<loadfactor)
  {
    HTHash table=table1;
    NewTable(table);
    int pos=h((char*)key.c_str(),table[0].size);
    Insertatstart(table[pos].list,key,item);
    table1=table;
    return;
  }
  
  int pos=h((char*)key.c_str(),table1[0].size);
  Insertatstart(table1[pos].list,key,item);

}

//Creates a HashTable.
HTHash HTCreate(int size)
{
  HTHash table;
  int i;
  table=new Hashtable[size];
  if(table==NULL)
  {
    printf("ERROR in malloc\n");
    return NULL;
    
  }
  for(i=0;i<size;i++)
  {
    table[i].size=size;
    table[i].num_elements=0;
    table[i].list=NULL;
  }
 return table;
}

//Searches for key in a hash. If found, returns true and copies the item in the item pointer passed as argument.
int HTGet(HTHash table,std::string & key,HTItem* item)
{
  Hashnode *templist;
  int i,a=table[0].size;
  i=h((char*)key.c_str(),table[0].size);
  
  Hashnode *N;
  N=table[i].list;
  while (N != NULL)
  {
    if (N->key==key)
    {
      *item=(N->item);
      return 1;
    }
    else
    {
      N=N->Link;
    }
  }
  *item=nullptr;
  return 0;
}

int HTsearch(HTHash table,std::string & key)
{
  Hashnode *templist;
  int i,a=table[0].size;
  i=h((char*)key.c_str(),table[0].size);

    if(table[i].list!=NULL)
    {
      Hashnode *N;
      N=table[i].list;

      while (N != NULL)
      { 
        if (N->key==key)
        {
         return 1;
        }
        else
        {
                 
          N=N->Link;
        }
      }
    }
  return 0;
}


