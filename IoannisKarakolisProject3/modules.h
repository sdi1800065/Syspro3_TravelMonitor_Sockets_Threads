#include <iostream>
typedef void* HTItem;
struct Hashnode
{
  std::string key;
  HTItem item;
  Hashnode *Link;
};

struct Hashtable
{
  int size;
  int num_elements;
  Hashnode *list;
};

typedef Hashtable *HTHash;


HTHash HTCreate(int);
void HTInsert(HTHash &,std::string&,HTItem);
int HTGet(HTHash,std::string& ,HTItem*);
int h(char* ,int );
int HTsearch(HTHash table,std::string& key);

class Bitarray
{
  int m;
  unsigned int * bitmatrix;
  int bitmatrixsize;

  const unsigned int sizeofbuckets= (8 * sizeof (unsigned int));
public:
  Bitarray(int m)
  {
    this->m=m;
    bitmatrixsize=((m + sizeofbuckets - 1) / sizeofbuckets);
    this->bitmatrix=new unsigned int[((m + sizeofbuckets - 1) / sizeofbuckets)]();
  }

  ~Bitarray(){delete[] this->bitmatrix;}
  int size(){return this->m;}

  void setbit(int a)
  {
    int pos=a/( 8 * sizeof (unsigned int));
    unsigned int mask=(unsigned int)1 << (a % sizeofbuckets);
    bitmatrix[pos]=bitmatrix[pos]|mask;
  }

  bool operator[](int a)
  {
    int pos=a/sizeofbuckets;
    unsigned int mask=(unsigned int)1 << (a % sizeofbuckets);
    return bitmatrix[pos]&mask ;
  }
  unsigned int * getbitmatrix()
  {
    return bitmatrix;
  }
  int getbitmatrixsize()
  {
    return bitmatrixsize;
  }
  void setnewbm(unsigned int* nbm)
  {
    delete[] bitmatrix;
    bitmatrix=nbm;
  }
};



class BloomFilter
{
    Bitarray array;
    const int K;
public:
  BloomFilter(int m,const int K=16):array(8*m),K{K}{};
  ~BloomFilter(){};
  void insert(std::string &str);
  int lookup(std::string &str);
  void print();
  unsigned int * getbm()
  {
    return array.getbitmatrix();
  }
  int getbms()
  {
    return array.getbitmatrixsize();
  }
  void setnewbitm(unsigned int * newbitm)
  {
    array.setnewbm(newbitm);
  }
};

class records
{
  int citizenID;
  std::string firstName;
  std::string lastName;
  std::string country;
  int age;
public:
  records(int id,std::string &name,std::string &surname,std::string &cntry,int ag)
  {
    citizenID=id;
    firstName=name;
    lastName=surname;
    country=cntry;
    age=ag;
  };
  ~records(){};
  std::string getgeneralinfo(){return firstName+lastName+country;}
  std::string getnamelastname(){return firstName+" "+lastName+" "+country ;}
  int getage(){return age;}
  int getcitizenid(){return citizenID;}
  void records_Print(){std::cout<<citizenID<<" "<<firstName<<" "<<lastName<<" "<<country<<" "<<age<<"\n";}
  std::string getcountry(){return country;}


};

unsigned long djb2(unsigned char *str);
unsigned long sdbm(unsigned char *str) ;
unsigned long hash_i(unsigned char *str, unsigned int i);


//Deletes a node from a list.
template<typename T>
void Delete1(Hashnode **list, Hashnode *node)
{
  if(node==nullptr)return;

  Hashnode *PreviousNode, *CurrentNode;

  if ((*list) ==node)
  {
    CurrentNode=*list;
    (*list)=(*list)->Link;
    if(CurrentNode->item!=nullptr)
    delete (T)CurrentNode->item;
    delete CurrentNode;
  }
  else
  {
    PreviousNode=*list;
    CurrentNode=(*list)->Link;

    while (CurrentNode->Link != node->Link)
    {
      PreviousNode=CurrentNode;
      CurrentNode=CurrentNode->Link;
    }
    PreviousNode->Link=CurrentNode->Link;
    if(CurrentNode->item!=nullptr)
    delete (T)CurrentNode->item;
    delete CurrentNode;
   }
}
#include <cstring>
// removes a key from a HashTable.
template<typename T>
void HTRemove(HTHash table,std::string &key)
{
  int pos=h((char*)key.c_str(),table[0].size);
  if(table[pos].list!=nullptr)
  {
    Hashnode *templist=table[pos].list;
    while(templist!=nullptr)
    {
        if (templist->key==key)
        {

          Delete1<T>(&(table[pos].list),templist);
           return;
        }
        templist=templist->Link;
    }
  }
  return;
}
//Destroys a HashTable.
template <typename T>
void HTDestroy(HTHash table)
{

  int i,a=table[0].size;
  for(i=0;i<a;i++)
  {
      Hashnode *templist=table[i].list;
      Hashnode *temp;
      while(templist!=nullptr)
      {

        temp=templist;
        templist=templist->Link;
          if(temp->item!=nullptr)
        delete (T)(temp->item);
        delete (Hashnode*)temp;
    }
  }
  delete[] table;
}
struct listnode
{
    int key;
    std::string data;
    listnode * next;
    listnode *bellow;
    listnode()
    {
        key=-99999;
        //*data="";
        next=nullptr;
        bellow=nullptr;
    }
    void set(int key,std::string &data,listnode*next,listnode*bellow)
    {
        this->key=key;
        this->data=data;
        this->next=next;
        this->bellow=bellow;
    }
};
class skiplist {
    listnode **table_of_lists;
    int maxlevel;

public:
  skiplist(int maxlevel=33);
  ~skiplist();
  listnode** get_pointers_vector(int,int);
void insert(int key,std::string &data);
void deletedata(int key);
listnode* search(int key);
bool searchbool(int key);
listnode *getbase()
{
  return table_of_lists[0];
}
int basesize_country(HTHash,std::string&);
};

typedef struct virus_structs
{
  skiplist *vaccinated;
  skiplist *not_vaccinated;
  BloomFilter *bloom;
  ~virus_structs()
  {
    delete vaccinated;
    delete not_vaccinated;
    delete bloom;
  };
}Virus_structs;

typedef struct dat{
int day;
int month;
int year;
int setdate(std::string &);
void setdate_sixmonthslater(struct dat d);
}Date;

bool checkifbetweendates(Date &date,Date &date1,Date &date2);
void printeveything(HTHash table,std::string& citizenID);
void print_country_statistics(HTHash filetable,Date date_1,Date date_2,std::string &secondelement,void*getv);
void print_country_statistics_age(HTHash filetable,Date date_1,Date date_2,std::string &secondelement,void*getv);
bool is_number(const std::string& s);
void insert_records_file(HTHash &filetable,HTHash &virustable,HTHash &,int bloomsize,std::string&id,std::string &irstname,std::string &lastname,std::string &country,std::string &age,std::string &virus,std::string &vacinated,std::string &date);
void insert_records_user(HTHash &filetable,HTHash &virustable,HTHash &,int bloomsize,std::string&id,std::string &irstname,std::string &lastname,std::string &country,std::string &age,std::string &virus,std::string &vacinated,std::string &date);
void get_todays_date(std::string &);
void print_all_nonvaccinated(HTHash filetable,void * getvoid);
