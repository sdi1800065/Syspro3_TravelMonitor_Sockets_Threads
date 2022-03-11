#include "modules.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <math.h>
#include<sys/types.h>/*  sockets  */ 
#include<sys/socket.h>/*  sockets  */
#include<netinet/in.h>/*  internet  sockets  */
#include<netdb.h>/*  gethostbyaddr  */
#include<pthread.h>
#include<arpa/inet.h>


  pthread_mutex_t  mtx1=PTHREAD_MUTEX_INITIALIZER;/*  Mutex  for  synchronization  */
  pthread_mutex_t  mtx2=PTHREAD_MUTEX_INITIALIZER;/*  Mutex  for  synchronization  */
  pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

  static char** countries=0;
  static int countries_counter=0;

  static char** already_inserted=0;
  static int already_inserted_counter=0;
  static int numThreads;
  static int nomorefiles;
  static int buffersize;
  static int bloomsize;
  static int cyclicBufferSize;
  char**buf;
  static int buf_init;
  static HTHash filetable=HTCreate(7867);//Initialize with a size /prime numbers are better as a size/the hashtable is dynamic so if it need more it will allocate by itself.
  static HTHash virustable=HTCreate(23);
  static HTHash countriestable=HTCreate(23);
  static std::string id,firstname,lastname,country,age,virus,vacinated,date;
  static std::string line;
  static int bufpoint;

  typedef struct args{
    int argc;
    char** argv;
  }Args;


template <typename Temp>    
void bufferwrite(int fd,int bufferSize,int sizeofelement,Temp send_element)
{
  int parts=0;
  if(bufferSize>=sizeofelement)parts=1;
  else parts=ceil((float)sizeofelement/(float)bufferSize);
  int i;
  if(send(fd,&parts,sizeof(int),MSG_WAITALL)==-1)
  {
    perror("Parting Write parts");
    exit(0);
  }

  int t=(sizeofelement/parts);
  if(send(fd,&t,sizeof(int),MSG_WAITALL)==-1)
  {
    perror("Parting Write parts");
    exit(0);
  }
  
  int lastpart=sizeofelement%parts;
  if(send(fd,&lastpart,sizeof(int),MSG_WAITALL)==-1)
  {
    perror("Parting Write parts");
    exit(0);
  }
 
  

  for(i=0;i<parts;i++)
  {
    if(send(fd,send_element+i*t,t,MSG_WAITALL)==-1)
    {
      perror("Parting Write");
      exit(0);
    }
  }
  if(lastpart!=0)
  {
    if(send(fd,send_element+i*t,lastpart,MSG_WAITALL)==-1)
    {
      perror("Parting Write");
      exit(0);
    }
  }
}

template<typename Temp>
void bufferread(int fd,int bufferSize,Temp temp)
{
  int parts;
  while(recv(fd,&parts,sizeof(int),MSG_WAITALL)==-1)
  {
  //  perror("READ PARTING parts");
    //exit(0);
  }
  int partsize;
  while(recv(fd,&partsize,sizeof(int),MSG_WAITALL)==-1)
  {
    //perror("READ PARTING partssize");
    //exit(0);
  }
  int lastpartsize;
  while(recv(fd,&lastpartsize,sizeof(int),MSG_WAITALL)==-1)
  {
    //perror("READ PARTING partssize");
    //exit(0);
  }
  int i;
  memset(temp,0,partsize*parts+lastpartsize);
  for(i=0;i<parts;i++)
  {
    while(recv(fd,temp+i*partsize,partsize,MSG_WAITALL)==-1)
    {
      //perror("READ PARTING Inside");
      //exit(0);
    }
  }
  if(lastpartsize!=0)
  {
    if(recv(fd,temp+i*partsize,lastpartsize,MSG_WAITALL)==-1)
    {
      perror("READ PARTING Inside");
      exit(0);
    }
  }
}

inline bool exists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}
int is_bufempty()
{
  int counter=0;
  for(int i=0;i<cyclicBufferSize;i++)
    if(strcmp(buf[i],"")!=0)return 0;

  return 1;
}
int is_buffull()
{
  int counter=-1;
  for(int i=0;i<cyclicBufferSize;i++)
    if(strcmp(buf[i],"")==0){counter=i;break;}
  if(counter!=-1)return counter;
  else return 1;
}


void* ParentThread(void* arguments)
{
  Args* args=(Args*)arguments;
  int i;
  
  for(i=0;i<cyclicBufferSize;i++)
  {
    buf[i]=new char[100];
    memset(buf[i],0,100);
    strcpy(buf[i],"");
  }
  buf_init=1;
  int k=0;
  for(i=args->argc-(args->argc-11);i<args->argc;i++){

    struct dirent *lv;
    DIR *dirp;
    dirp = opendir(args->argv[i]);
    if (dirp == NULL) {
        perror("Directory Error");
        exit(-2);
    }
    countries=(char**) realloc(countries, (countries_counter + 1) * sizeof(*countries));
    countries[countries_counter]=new char[100];
    strcpy(countries[countries_counter],args->argv[i]);
    countries_counter++;
    while ((lv = readdir(dirp))){
        if(strcmp(lv->d_name,"..")==0 || strcmp(lv->d_name,".")==0)
        {
            continue;
        }
        std::string inputfile(args->argv[i]);
        inputfile=inputfile+"/"+lv->d_name;
        
        already_inserted=(char**) realloc(already_inserted, (already_inserted_counter + 1) * sizeof(*already_inserted));
        already_inserted[already_inserted_counter]=new char[100];
        strcpy(already_inserted[already_inserted_counter],lv->d_name);
        already_inserted_counter++;
        

        while(k=is_buffull()){}

        memset(buf[k],0,100);
        strcpy(buf[k],inputfile.c_str());
        //k++;
    }

    closedir(dirp);
  };
    nomorefiles=1;
  return arguments;
}

void* ThreadFun(void * dummy) {
  int i,j;
  int flag=0;
  while(buf_init!=1){};
 
  while(1){
    if(nomorefiles==1){
      if(is_bufempty())
      {
        return dummy;
      }
    }
  
    pthread_mutex_lock(&mtx1);
    std::string inputfile(buf[bufpoint]);
    memset(buf[bufpoint],0,100);
    strcpy(buf[bufpoint],"");
    std::ifstream myfile(inputfile);
    bufpoint++;
    if(bufpoint==cyclicBufferSize)bufpoint=0;
    pthread_mutex_unlock(&mtx1);
    if(exists(inputfile)==0)
    {
      continue;
    }
    pthread_mutex_lock(&mtx2);
    while (std::getline(myfile, line))
    {
      std::istringstream iss(line);
      if (!(iss >>id>>firstname>>lastname>>country>>age>>virus>>vacinated)) { continue; } // error
        if(vacinated=="NO")
        {
          if(iss>>date)
          {
            std::cout<<"ERROR IN RECORD "<<line<<" Date after NO\n";
            continue;
          }else date=" ";
        }else if(!(iss>>date)){std::cout<<"ERROR:EXPECTED DATE AFTER YES\n";continue;};
      insert_records_file(filetable,virustable,countriestable,bloomsize,id,firstname,lastname,country,age,virus,vacinated,date);
    }
    myfile.close();
    pthread_mutex_unlock(&mtx2);
    if(flag==1)return dummy;
  }
}

void addvac(int fd2) {

  buf_init=0;
  nomorefiles=0;
  bufpoint=0;
  int i,j;
  pthread_t *threads;
  threads =(pthread_t*)malloc(sizeof(pthread_t)*numThreads);

  void* dummy;
  for(i=0;i<numThreads;i++)
    pthread_create(&threads[i],NULL,ThreadFun,(void*)&threads[i]);

  for(int i=0;i<cyclicBufferSize;i++)
  {
    memset(buf[i],0,100);
    strcpy(buf[i],"");
  }

  buf_init=1;
  int k=0;

  char*bf2=new char[200];
  for(i=0;i<countries_counter;i++)
  {
    memset(bf2,0,200);
    strcpy(bf2,countries[i]);
    struct dirent *lv;
    DIR *dirp;
    
    dirp = opendir(bf2);
    if (dirp == NULL) {
        perror("Directory Error2");
        exit(-2);
    }
    
    while ((lv = readdir(dirp))){
        if(strcmp(lv->d_name,"..")==0 || strcmp(lv->d_name,".")==0  )
        {
            continue;
        }
        int flaga=0;
        for(j=0;j<already_inserted_counter;j++)
        {
          if(strcmp(already_inserted[j],lv->d_name)==0)
          {
            flaga=1;
            break;
          }
        }
        if(flaga==1)continue;
        std::string inputfile(bf2);
        inputfile=inputfile+"/"+lv->d_name;
        std::ifstream myfile(inputfile);
        
        already_inserted=(char**) realloc(already_inserted, (already_inserted_counter + 1) * sizeof(*already_inserted));
        already_inserted[already_inserted_counter]=new char[100];
        strcpy(already_inserted[already_inserted_counter],lv->d_name);
        already_inserted_counter++;

        while(k=is_buffull()){}

        memset(buf[k],0,100);
        strcpy(buf[k],inputfile.c_str());
        k++;
        
    }
    closedir(dirp);
  }
  nomorefiles=1;
 
  for(i=0;i<numThreads;i++)
  {
    pthread_join(threads[i],NULL);
  }
  free(threads);

  int a=virustable->size;
  for(i=0;i<a;i++)
  {
     Hashnode *templist=virustable[i].list;
        while(templist!=NULL)
        {
            bufferwrite(fd2,buffersize,sizeof(char)*strlen((templist->key).c_str()),(templist->key).c_str());

            Virus_structs* getbloom;
            getbloom=(Virus_structs*)templist->item;
            unsigned int* t=getbloom->bloom->getbm();
            for(int j=0;j<getbloom->bloom->getbms();j++){
                bufferwrite(fd2,buffersize,sizeof(unsigned int),&t[j]);
            }
            templist=templist->Link;
        }
    }
    memset(bf2,0,100);
    strcpy(bf2,"DONEREADINGVIRUSES");
    bufferwrite(fd2,buffersize,sizeof(char)*strlen(bf2)+1,bf2);
  delete[] bf2;
}

int main(int argc, char** argv)
{
  int i;
  if(argc<12)
  {
    std::cout<<"ERROR missing arguments or too many arguments \n";
    exit(-1);
  }
  std::string temp;
  int port;
  buf_init=0;
  nomorefiles=0;
  bufpoint=0;
  for(i=1;i<argc-(argc-10);i++)
  {
    temp=argv[i];
    std::string temp=argv[i];
    if(temp.compare("–p")==0 || temp.compare("-p")==0)
    {
      if(argv[i+1]==nullptr)
      {
        std::cout<<"ERROR expected the number of port after -p\n";
        return 1;
      }
      std::string temp2=argv[i+1];
      int j;
      for (j=0;j<temp2.length();j++)
      {
        if( temp2[j]<'0' || temp2[j]>'9')
        {
          std::cout<<"ERROR invalid intiger after -p, characters were detected\n";
          return 1;
        }
      }
      port=std::stoi(argv[i+1]);
    }
    else if(temp.compare("–t")==0 || temp.compare("-t")==0)
    {
      if(argv[i+1]==nullptr)
      {
        std::cout<<"ERROR expected the number of threads after -t\n";
        return 1;
      }
      std::string temp2=argv[i+1];
      int j;
      for (j=0;j<temp2.length();j++)
      {
        if( temp2[j]<'0' || temp2[j]>'9')
        {
          std::cout<<"ERROR invalid intiger after -t, characters were detected\n";
          return 1;
        }
      }
      numThreads=std::stoi(argv[i+1]);
    }
    else if(temp.compare("-s")==0)
    {
      if(argv[i+1]==nullptr)
      {
        std::cout<<"ERROR expected size of bloomfilter after -s\n";
        return 1;
      }
      std::string temp2=argv[i+1];
      int j;
      for (j=0;j<temp2.length();j++)
      {
        if( temp2[j]<'0' || temp2[j]>'9')
        {
          std::cout<<"ERROR invalid intiger after -s, characters were detected\n";
          return 1;
        }
      }
      bloomsize=std::stoi(argv[i+1]);
     }
     else if(temp.compare("-c")==0)
     {
        if(argv[i+1]==nullptr)
        {
          std::cout<<"ERROR expected cyclicBufferSize after -s\n";
          return 1;
        }
        std::string temp2=argv[i+1];
        int j;
        for (j=0;j<temp2.length();j++)
        {
          if( temp2[j]<'0' || temp2[j]>'9')
          {
            std::cout<<"ERROR invalid intiger after -c, characters were detected\n";
            return 1;
          }
        }
        cyclicBufferSize=std::stoi(argv[i+1]);
      }
      else if(temp.compare("-b")==0)
      {
        if(argv[i+1]==nullptr)
        {
          std::cout<<"ERROR expected size of buffer after -b\n";
          return 1;
        }
        std::string temp2=argv[i+1];
        int j;
        for (j=0;j<temp2.length();j++)
        {
          if( temp2[j]<'0' || temp2[j]>'9')
          {
            std::cout<<"ERROR invalid intiger after -b, characters were detected\n";
            return 1;
          }
        }
        buffersize=std::stoi(argv[i+1]);
      }
  }
  int sock,bnd,newsocket;
  struct sockaddr_in  server;
  struct sockaddr *serverptr =(struct sockaddr  *)&server;
  struct hostent *rem;
 
  char hbuf[50];
  int hname;
  char hip[50];

  buf=new char*[cyclicBufferSize];
  pthread_t *threads;
  threads =(pthread_t*)malloc(sizeof(pthread_t)*numThreads);
  Args *args=new Args;
  args->argc=argc;
  args->argv=argv;

  void* dummy;
  for(i=0;i<numThreads;i++)
    pthread_create(&threads[i],NULL,ThreadFun,(void*)&threads[i]);
  ParentThread((void*)args);

  for(i=0;i<numThreads;i++)
  {
    pthread_join(threads[i],NULL);
  }
  free(threads);

  if((sock = socket(AF_INET , SOCK_STREAM , 0)) < 0)
  {
    perror("socket");
    exit(-1);
  }

  hname = gethostname(hbuf, sizeof(hbuf));
  rem = gethostbyname(hbuf);
  strcpy(hip , inet_ntoa(*((struct in_addr*)rem->h_addr_list[0])));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
    
  server.sin_addr.s_addr = inet_addr(hip);
  int opt=1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
  if((bnd = bind(sock,serverptr,sizeof(server))) < 0)
  {
    perror("bind");
    exit(-1);
  }
  if(listen(sock,1)<0)
  {
    perror("listen");
    exit(-1);
  }

  socklen_t  serverlen=sizeof server;
  newsocket=accept(sock,serverptr,&serverlen);
  if(newsocket < 0)
  {
    perror("newsocket");
    exit(-1);
  }

  int t=1;



  char bf[100];
  int a=virustable->size;
  for(i=0;i<a;i++)
  {
    Hashnode *templist=virustable[i].list;
    while(templist!=NULL)
    {
      //recv(newsocket,&t,sizeof(int),MSG_WAITALL);   //wake monitoraki
      bufferwrite(newsocket,buffersize,sizeof(char)*strlen((templist->key).c_str()),(templist->key).c_str());
      Virus_structs* getbloom;
      getbloom=(Virus_structs*)templist->item;
      unsigned int* t=getbloom->bloom->getbm();
      for(int j=0;j<getbloom->bloom->getbms();j++){
        bufferwrite(newsocket,buffersize,sizeof(unsigned int),&t[j]);}
      templist=templist->Link;
    }
  }
  //read(newsocket,&t,sizeof(int));
  memset(bf,0,100);
  strcpy(bf,"DONEREADINGVIRUSES");
  bufferwrite(newsocket,buffersize,sizeof(char)*strlen(bf)+1,bf);

  while(1)
  {    
    memset(bf,0,100);

    bufferread(newsocket,buffersize,bf);
      
    if(strcmp(bf,"exit")==0) //prepares the monitor to be killed , its not really neaded but valgrind gives errors withought it.
    {       
      close(newsocket);
      close(sock);
      HTDestroy<records *>(filetable);
      HTDestroy<Virus_structs *>(virustable);
      HTDestroy<int*>(countriestable);//Doesnt matter what template you give cause item->data will be null for this case just need to delete pointers and key.
      delete args;
      for(i=0;i<cyclicBufferSize;i++)
      {
        delete[] buf[i];
      }
        for(i=0;i<countries_counter;i++)
        {
            delete[] countries[i];
        }
        free(countries);
        for(i=0;i<already_inserted_counter;i++)
        {
            delete[] already_inserted[i];
        }
        free(already_inserted);
        delete[] buf;
        close(sock);
        close(newsocket);
        exit(0);
    }
    else if(strcmp(bf,"addrecord")==0)
    {
      addvac(newsocket);
      continue;
    }
    else if(strcmp(bf,"EVERYTHING")==0)
    {
      memset(bf,0,100);
      bufferread(newsocket,buffersize,bf);
           
      std::string citizenID(bf);
      void*recv;
      if(!HTGet(filetable,citizenID,&recv)){memset(bf,0,100);strcpy(bf,"NOSUCHID");bufferwrite(newsocket,buffersize,sizeof(char)*strlen(bf),bf);continue;};
          
      records*rec=(records*)recv;
      int flaga=0;
      int i,a=virustable->size;
      int id=std::stoi(citizenID);
      for(i=0;i<a;i++)
      {
        Hashnode *templist=virustable[i].list;
        while(templist!=NULL)
        {   
          Virus_structs* getvirus;
          getvirus=(Virus_structs*)templist->item;
          listnode *temp;
                    
          if(temp=getvirus->vaccinated->search(id))
          {
            std::string sendstr(" VACCINATED ON ");
            sendstr=templist->key+sendstr+temp->data;
            memset(bf,0,100);
            strcpy(bf,sendstr.c_str());
            bufferwrite(newsocket,buffersize,sizeof(char)*strlen(bf),bf);
            if(flaga==0)
            {
              char namelast[100];
              memset(namelast,0,100);
              strcpy(namelast,(rec->getnamelastname()).c_str());
              bufferwrite(newsocket,buffersize,sizeof(char)*strlen(namelast),namelast);
              memset(namelast,0,100);
              strcpy(namelast,(std::to_string(rec->getage())).c_str());
              bufferwrite(newsocket,buffersize,sizeof(char)*strlen(namelast)+1,namelast);
              flaga=1;
            }
          }
          else if(temp=getvirus->not_vaccinated->search(id))
          {
            std::string sendstr(" NOT YET VACCINATED");
            sendstr=templist->key+sendstr;
            memset(bf,0,100);
            strcpy(bf,sendstr.c_str());
            bufferwrite(newsocket,buffersize,sizeof(char)*strlen(bf),bf);
            if(flaga==0)
            {
              char namelast[100];
              memset(namelast,0,100);
              strcpy(namelast,(rec->getnamelastname()).c_str());
              bufferwrite(newsocket,buffersize,sizeof(char)*strlen(namelast),namelast);
              memset(namelast,0,100);
              strcpy(namelast,(std::to_string(rec->getage())).c_str());
              bufferwrite(newsocket,buffersize,sizeof(char)*strlen(namelast)+1,namelast);
              flaga=1;
            }
          }
          templist=templist->Link;
        }
      }
      memset(bf,0,100);
      strcpy(bf,"NOSUCHID");
      bufferwrite(newsocket,buffersize,sizeof(char)*strlen(bf),bf);
    }
    else if(strcmp(bf,"CHECKPOSITIVE")==0){
      memset(bf,0,100);
      bufferread(newsocket,buffersize,bf);
      std::string id(bf);
      memset(bf,0,100);
      bufferread(newsocket,buffersize,bf);
      std::string virusname(bf);
      void*getvoid;
      Virus_structs*getvirus;
      HTGet(virustable,virusname,&getvoid);      
      getvirus=(Virus_structs*)getvoid;
      listnode*temp;
      if(temp=getvirus->vaccinated->search(std::stoi(id))){
        memset(bf,0,100);
        strcpy(bf,"YES");
        bufferwrite(newsocket,buffersize,sizeof(char*)*strlen(bf),bf);
        memset(bf,0,100);
        strcpy(bf,(temp->data).c_str());
        bufferwrite(newsocket,buffersize,sizeof(char*)*strlen(bf),bf);
        continue;
      }
      else
      {
        memset(bf,0,100);
        strcpy(bf,"NO");
        bufferwrite(newsocket,buffersize,sizeof(char*)*strlen(bf),bf);
        continue;
      }
      continue;
    }
    else
    {
      std::cout<<"WRONG INPUT GIVEN TO MONITORAKI\n";
    }
  }
}
