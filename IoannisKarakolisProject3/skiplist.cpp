#include <iostream>
#include <cstdlib>     /* srand, rand */
#include <ctime> 
#include "modules.h"

int geth(int maxlevel)
{
    int sum=0;
    while (rand()%2==1 && sum<maxlevel)
    {
        sum=sum+1;
    }
    return sum;
}

skiplist::skiplist(int mlevel)
{
    srand (time(nullptr));
    maxlevel=mlevel;
    int i;
    table_of_lists=new listnode*[mlevel+1];//+1 to create an empty top
    table_of_lists[0]=new listnode;
    for (i=1; i<mlevel+1; i++)
    {   
        table_of_lists[i]=new listnode;
        table_of_lists[i]->bellow=table_of_lists[i-1];
    }   
}
    
void skiplist::insert(int key,std::string& data)
{  
    int height=geth(this->maxlevel);
    if(skiplist::searchbool(key)){std::cout<<"Already inserted\n";return;}
    listnode *top=table_of_lists[maxlevel],*prev,*above;
    int hlevel=maxlevel;
    int flag=0;
    while(top->bellow!=nullptr)
    {
        while(top->next!=nullptr)
        {
                if(top->next->key>=key) break;
                top=top->next;   
        }
        if(hlevel<=height)
        {         
            listnode * newlistnode=new listnode;
            newlistnode->next=top->next;
            newlistnode->key=key;
            if(flag==1)
                above->bellow=newlistnode;        
            flag=1;
            above=newlistnode;
            top->next=newlistnode;

        }
        prev=top;
        top=top->bellow;
        hlevel--;
    }
    while(top->next!=nullptr)
    {
        if(top->next->key>=key) break;
           top=top->next;   
    }
    listnode * newlistnode_zero=new listnode;
    newlistnode_zero->set(key,data,top->next,nullptr);
    top->next=newlistnode_zero;
    if(flag==1){above->bellow=newlistnode_zero;}

}

void skiplist::deletedata(int key)
{
    listnode *temp1;
    if(!(temp1=skiplist::search(key))){std::cout<<"Already not inserted\n";return;};
    listnode *top=table_of_lists[maxlevel],*temp,*temp2;
    int flag=0;
    while(top->bellow!=nullptr)
    {
        top=top->bellow;

            while(top->next!=nullptr)
            {
                if(top->next->key>=key) break;
                top=top->next;   
            }
            if(top->next!=nullptr)
            {
                if(top->next->key==key)
                {   
                    temp=top->next;
                    top->next=top->next->next;
                    delete temp;
                }
            }
    }
}
    
listnode* skiplist::search(int key)
{
    listnode *top=table_of_lists[maxlevel];
    while(top->bellow!=nullptr)
    {
        top=top->bellow;
            while(top->next!=nullptr)
            {
                if(top->next->key>key)break;
                top=top->next;   
            }
    }
    if(top->key==key)
        return top;
    return nullptr;      
}
bool skiplist::searchbool(int key)
{
    listnode *top=table_of_lists[maxlevel];
    while(top->bellow!=nullptr)
    {
        top=top->bellow;
            while(top->next!=nullptr)
            {
                if(top->next->key>key)break;
                top=top->next;   
            }
        if(top->key==key)return 1;
    }
    return 0;      
}
skiplist::~skiplist()
{
    int i;
    for (i=0;i<=maxlevel;i++)
    {
        listnode* currentNode = table_of_lists[i]; 
        while (currentNode!=nullptr)
        {
            listnode* nextNode = currentNode->next;    
            delete currentNode;                        
            currentNode = nextNode;        
                         
        }
    }
    delete[] table_of_lists;       
}

int skiplist:: basesize_country(HTHash filetable,std::string &secondelement)
{
    listnode *templist=table_of_lists[0];
    int size=0;
    std::string tempstr;
    records* nrec;
    void *getrec;
    while(templist!=NULL)
    {
        tempstr=std::to_string(templist->key);
        if(HTGet(filetable,tempstr,&getrec))
        {
            nrec=(records*)getrec;

            if(nrec->getcountry()==secondelement)
            {
                size++;
           }
        }
        templist=templist->next;
    }
    return size;
}