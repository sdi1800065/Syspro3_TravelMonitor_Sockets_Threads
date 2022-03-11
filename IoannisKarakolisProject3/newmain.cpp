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
#include <cstring>
#include <dirent.h>
#include <string>
#include <math.h>
#include<sys/socket.h>
#include<netinet/in.h>/*  internet  sockets  */
#include<netdb.h>/*  gethostbyaddr  */
#include<arpa/inet.h>
#include<time.h>

typedef struct fd_countries{
    char** countries;
    char** countries_path;
    int counter;
}Fd_countries;

typedef struct stat_countries{
    char* country;
    Date**  dates_accepted;
    Date**  dates_rejected;
    int counter_of_datesa;
    int counter_of_datesr;
}Stat_countries;



void logfile(int pid,Fd_countries countries_of_child,Stat_countries*stats,int num_of_allcountries,int element)
{
    std::string filename("log_file.");
    filename=filename+std::to_string(pid)+".txt";
    std::ofstream File(filename);

    for(int i=0;i<countries_of_child.counter;i++)
    {
        File <<countries_of_child.countries[i]<<"\n";
    }
    int totala=0,totalr=0;

    for(int i=0;i<countries_of_child.counter;i++)
    {
        for(int j=0;j<num_of_allcountries;j++)
        {
            if(strcmp(countries_of_child.countries[i],stats[j].country)==0)
            {
               totala=totala+stats[j].counter_of_datesa;
               totalr=totalr+stats[j].counter_of_datesr;
            }
        }
    }
    
    File<<"TOTAL REQUESTS  "<<totala+totalr<<"\n";
    File<<"ACCEPTED  "<<totala<<"\n";
    File<<"REJECTED  "<<totalr<<"\n";

    File.close();
}


template<typename Temp>
void bufferread(int fd,int bufferSize,Temp temp)
{
  int parts;
  if(recv(fd,&parts,sizeof(int),MSG_WAITALL)==-1)
  {
    perror("READ PARTING parts");
    exit(0);
  }
  int partsize;
  if(recv(fd,&partsize,sizeof(int),MSG_WAITALL)==-1)
  {
    perror("READ PARTING partssize");
    exit(0);
  }
  int lastpartsize;
  if(recv(fd,&lastpartsize,sizeof(int),MSG_WAITALL)==-1)
  {
    perror("READ PARTING partssize");
    exit(0);
  }
  int i;
  
 // std::cout<<partsize*parts+lastpartsize<<"\n";
  memset(temp,0,partsize*parts+lastpartsize);
  for(i=0;i<parts;i++)
  {
    if(recv(fd,temp+i*partsize,partsize,MSG_WAITALL)==-1)
    {
      perror("READ PARTING Inside1");
      exit(0);
    }
  }
  if(lastpartsize!=0)
  {
    if(recv(fd,temp+i*partsize,lastpartsize,MSG_WAITALL)==-1)
    {
      perror("READ PARTING Inside2");
      exit(0);
    }
  }
}

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
void must_exit(int numMonitors,Fd_countries* countries_struct,Stat_countries*countries_stats,int countries,int*fdnums,int* forknums,int bufferSize,char*bf,HTHash tables_of_blooms,int bloomscounter,BloomFilter** keep_blooms,HTHash*keep_hash,int hashcounter)
{
    int i;
    for(i=0;i<numMonitors;i++)
    logfile(forknums[i],countries_struct[i],countries_stats,countries,i);
    
    memset(bf,0,100);
    strcpy(bf,"exit");
    for(i=0;i<numMonitors;i++)
    {
        bufferwrite(fdnums[i],bufferSize,sizeof(char)*strlen(bf)+1,bf);
    }
    int ii,a=tables_of_blooms[0].size;
    for(ii=0;ii<a;ii++)
    {
        Hashnode *templist=tables_of_blooms[ii].list;
        Hashnode *temp;
        while(templist!=nullptr)
        {
            temp=templist;
            templist=templist->Link;
            delete  (temp);
        }
    }
    delete[] tables_of_blooms;

    for(i=0;i<numMonitors;i++)
    {
        for(int j=0;j<countries_struct[i].counter;j++){
           delete[] countries_struct[i].countries[j];
        }
        for(int j=0;j<countries_struct[i].counter;j++){
           delete[] countries_struct[i].countries_path[j];
        }
        free(countries_struct[i].countries_path);
        free(countries_struct[i].countries);
    }
    

    for(i=0;i<countries;i++)
    {
        for(int j=0;j<countries_stats[i].counter_of_datesa;j++){
            delete countries_stats[i].dates_accepted[j];
        }
        free(countries_stats[i].dates_accepted);
        for(int j=0;j<countries_stats[i].counter_of_datesr;j++){
            delete countries_stats[i].dates_rejected[j];
        }
        free(countries_stats[i].dates_rejected);
        delete[] countries_stats[i].country;

    }

    for(i=0;i<numMonitors;i++)
    {
        close(fdnums[i]);
    }
    delete[] bf;
    for(i=0;i<bloomscounter;i++)
    {
        delete keep_blooms[i];
    }    
    free(keep_blooms);


    for(i=0;i<hashcounter;i++)
    {
        int ii,a=keep_hash[i][0].size;
        for(ii=0;ii<a;ii++)
        {
            Hashnode *templist=keep_hash[i][ii].list;
            Hashnode *temp;
            while(templist!=nullptr)
            {
                temp=templist;
                templist=templist->Link;
                delete (temp);
            }
        }
        delete[] keep_hash[i];
    }
    free(keep_hash);  
    exit(1);
}


int main(int argc, char** argv)
{
    int i;

    srand( time(NULL) );
    int PORT = rand() % 8000 +2000;
    if(argc!=13)
    {
        std::cout<<"ERROR missing arguments or too many arguments \n";
        return 1;
    }
    std::string input_dir;
    int numMonitors;
    int bufferSize;
    int bloomsize;
    int cyclicBufferSize;
    int numThreads;
    std::string temp;
    for(i=1;i<argc;i++)
    {
        temp=argv[i];
        std::string temp=argv[i];
        if(temp.compare("–t")==0 || temp.compare("-t")==0)
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
        else if(temp.compare("–m")==0 || temp.compare("-m")==0)
        {
            if(argv[i+1]==nullptr)
            {
                std::cout<<"ERROR expected the number of monitors after -m\n";
                return 1;
            }
            std::string temp2=argv[i+1];
            int j;
            for (j=0;j<temp2.length();j++)
            {
                if( temp2[j]<'0' || temp2[j]>'9')
                {
                    std::cout<<"ERROR invalid intiger after -m, characters were detected\n";
                    return 1;
                }
            }
            numMonitors=std::stoi(argv[i+1]);
        }
        else if(temp.compare("-i")==0)
        {
            if(argv[i+1]==nullptr)
            {
                std::cout<<"ERROR Expected name of directory after -i\n";
                return 1;
            }
            input_dir=argv[i+1];
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
            bufferSize=std::stoi(argv[i+1]);
        }
    }
    if(bufferSize<4){
        std::cout<<"ERROR BUFFERSIZE MUST BE GREATER THAN 3\n";
        exit(0);
    }
    
    int forknums[numMonitors];
    int fd;
   
    char Pipe[20];
    char Piper[20];
    char *bf=new char[100];
    int countries=0;


    Fd_countries countries_struct[numMonitors];
    for(i=0;i<numMonitors;i++){
        countries_struct[i].countries=0;
        countries_struct[i].countries_path=0;
        countries_struct[i].counter=0;
    }
    struct dirent *lv;
    DIR *dirp;

    dirp = opendir((char*)input_dir.c_str());
    if (dirp == NULL) {
        perror("Directory Error");
        return -1;
    }
    
    int rb=0;
    while ((lv = readdir(dirp))){
        if(strcmp(lv->d_name,"..")==0 || strcmp(lv->d_name,".")==0  )
        {
            continue;
        }
        if(rb==numMonitors)rb=0;

       countries++;
        countries_struct[rb].countries = (char**) realloc(countries_struct[rb].countries, (countries_struct[rb].counter + 1) * sizeof(*countries_struct[rb].countries));
        countries_struct[rb].countries_path = (char**) realloc(countries_struct[rb].countries_path, (countries_struct[rb].counter + 1) * sizeof(*countries_struct[rb].countries_path));
        countries_struct[rb].countries[countries_struct[rb].counter]=new char[100];
        countries_struct[rb].countries_path[countries_struct[rb].counter]=new char[100];
        strcpy(countries_struct[rb].countries[countries_struct[rb].counter],lv->d_name);
        strcpy(countries_struct[rb].countries_path[countries_struct[rb].counter],(input_dir+"/"+lv->d_name).c_str());
        
        countries_struct[rb].counter++;
        rb++;
    }
    closedir(dirp);
    if(numMonitors>countries)numMonitors=countries;
    Stat_countries countries_stats[countries];
    int k=0;

    for(i=0;i<numMonitors;i++){
        
        for(int j=0;j<countries_struct[i].counter;j++)
        {
            countries_stats[k].country=new char[sizeof(char)*strlen(countries_struct[i].countries[j])+1];
            countries_stats[k].counter_of_datesa=0;
            countries_stats[k].counter_of_datesr=0;
            
            countries_stats[k].dates_accepted=0;
            countries_stats[k].dates_rejected=0;
            strcpy(countries_stats[k].country,countries_struct[i].countries[j]);
            k++;
        }
    }
    int clientsock[numMonitors] , newsock;
    struct sockaddr_in  server , client;
    socklen_t  clientlen;
    struct sockaddr *clientptr =(struct sockaddr  *)&client;
    struct hostent *rem;
    char hbuf[50];
    int hname;
    char hip[50];

    for(i=0;i<numMonitors;i++)
    if((clientsock[i] = socket(AF_INET , SOCK_STREAM , 0)) < 0)
    {
        perror("socket");
        exit(-1);
    }
    
    int opt=1;
    for(i=0;i<numMonitors;i++)
    setsockopt(clientsock[i], SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    hname = gethostname(hbuf, sizeof(hbuf));
    rem = gethostbyname(hbuf);
    strcpy(hip , inet_ntoa(*((struct in_addr*)rem->h_addr_list[0])));
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(hip);

    for(i=0;i<numMonitors;i++)
    {
        int pid=fork();
        if(pid==-1)
        {
            perror("Fork");
            exit(2);
        }
        else if(pid==0)
        {
            char *a[]= {(char*)"./monitorServer",(char*)"-p",(char*)(std::to_string(PORT+i+1)).c_str(),(char*)"-t",(char*)(std::to_string(numThreads)).c_str(),(char*)"-b",(char*)(std::to_string(bufferSize)).c_str(),(char*)"-c",(char*)(std::to_string(cyclicBufferSize)).c_str(),(char*)"-s",(char*)(std::to_string(bloomsize)).c_str(),NULL};
            int j;
            for(j=0;j<countries_struct[i].counter;j++)
            {
                a[j+11]=countries_struct[i].countries_path[j];
            }
            a[j+11]=NULL;
            execvp(a[0],a);
        }
        forknums[i]=pid;
    }

    int c=0;
    for(i=0;i<numMonitors;i++){
        server.sin_port = htons(PORT+i+1);
        while(1){
            c=connect(clientsock[i],(struct sockaddr*)&server,sizeof(server));
            if(c>=0)
            {
                break;
            }
        }
    }    
    
    HTHash tables_of_blooms=HTCreate(23);
    BloomFilter *tempblooms;
    
    int BloomFiltercounter=0;

    BloomFilter** keep_blooms=0;
    int bloomscounter=0;

    HTHash* keep_hash=0;
    int hashcounter=0;
   
    std::cout<<"PLEASE WAIT FOR EVERYTHING TO BE STORED,THERE WILL BE A MESSAGE\n";
    for(i=0;i<numMonitors;i++)
    {
        HTHash temphash=HTCreate(23);
        char *virus=new char[100];

        
        
        while(1){
            memset(virus,0,100);
            //send(clientsock[i],&i,sizeof(int),MSG_WAITALL); //wake monitoraki

            bufferread(clientsock[i],bufferSize,virus);
            if(strcmp(virus,"DONEREADINGVIRUSES")==0)break;
            tempblooms=new BloomFilter(bloomsize);
            unsigned int * test=new unsigned int[tempblooms->getbms()]();
            keep_blooms = (BloomFilter**) realloc(keep_blooms, (bloomscounter + 1) * sizeof(*keep_blooms));
                int t=1;
            
            for(int j=0;j<tempblooms->getbms();j++)
                {bufferread(clientsock[i],bufferSize,&test[j]);}
            tempblooms->setnewbitm(test);
            keep_blooms[bloomscounter]=tempblooms;
            bloomscounter++;
            std::string tempstr(virus);
            HTInsert(temphash,tempstr,(HTItem)(tempblooms));
        }    
        keep_hash = (HTHash*) realloc(keep_hash, (hashcounter + 1) * sizeof(*keep_hash));
        keep_hash[hashcounter]=temphash;
        hashcounter++;
        for(int j=0;j<countries_struct[i].counter;j++)
        {
            std::string tempstr(countries_struct[i].countries[j]);
            HTInsert(tables_of_blooms,tempstr,(HTItem)(temphash));
        }
        delete[] virus;
    }


    memset(bf,0,100);
    std::cout<<"Everything is stored give user INPUT\n";    
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string firstelement,id,virusname,countryfrom,countryto,date;
        if(!(iss>>firstelement)){std::cout<<"Wrong Input\n";continue;};
        if(firstelement=="/travelRequest")
        {

            if (!(iss>>id>>date>>countryfrom>>countryto>>virusname)) { std::cout<<"ERROR: IN USER INPUT\n";continue; } // error
            
            if((!is_number(id))){std::cout<<"ERROR:given id is not a number\n";continue;};
            Date d;
            if(!d.setdate(date)){std::cout<<"ERROR:INVALID DATE GIVEN\n";continue;}
            void* gethash;
            if(!HTGet(tables_of_blooms,countryto,&gethash)){std::cout<<"ERROR:INVALID COUNTRYTO GIVEN\n";continue;}
            if(HTGet(tables_of_blooms,countryfrom,&gethash))
            {
                HTHash geth=(HTHash)gethash;
                void*getbloom;
                if(HTGet(geth,virusname,&getbloom))
                {
                    BloomFilter *bloom=(BloomFilter*)getbloom;
                    if(bloom->lookup(id))
                    {
                        
                        for(i=0;i<numMonitors;i++)
                            for(int j=0;j<countries_struct[i].counter;j++)
                            {
                                if(strcmp(countries_struct[i].countries[j],countryfrom.c_str())==0)
                                {
                                    memset(bf,0,100);
                                    strcpy(bf,"CHECKPOSITIVE");
                                    bufferwrite(clientsock[i],bufferSize,sizeof(char)*strlen(bf)+1,bf);
                                    bufferwrite(clientsock[i],bufferSize,sizeof(char)*strlen(id.c_str())+1,id.c_str());
                                    bufferwrite(clientsock[i],bufferSize,sizeof(char)*strlen(virusname.c_str())+1,virusname.c_str());
                                    memset(bf,0,100);
                                    bufferread(clientsock[i],bufferSize,bf);
                                    if(strcmp(bf,"NO")==0)
                                    {
                                        for(int t=0;t<countries;t++)
                                        {
                                            if(strcmp(countries_stats[t].country,countryto.c_str())==0){
                                            countries_stats[t].dates_rejected=(Date**)realloc(countries_stats[t].dates_rejected, (countries_stats[t].counter_of_datesr + 1) * sizeof(*countries_stats[t].dates_rejected));
                                            countries_stats[t].dates_rejected[countries_stats[t].counter_of_datesr]=new Date;
                                            if(!countries_stats[t].dates_rejected[countries_stats[t].counter_of_datesr]->setdate(date)){ std::cout<<"ERROR: IN USER INPUT INVALID DATE FORMAT\n";continue; } // error;
                                            countries_stats[t].counter_of_datesr++;
                                            }
                                        }
                                        std::cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED\n";
                                    }
                                    else
                                    {
                                        memset(bf,0,100);
                                        bufferread(clientsock[i],bufferSize,bf);
                                        Date d_vaccination;
                                        std::string tempstr(bf);
                                        d_vaccination.setdate(tempstr);
                                        Date d_sixlater;
                                        d_sixlater.setdate_sixmonthslater(d_vaccination);;
                                        if(checkifbetweendates(d,d_vaccination,d_sixlater))
                                        {
            
                                            for(int t=0;t<countries;t++)
                                            {
                                                if(strcmp(countries_stats[t].country,countryto.c_str())==0){
                                                countries_stats[t].dates_accepted=(Date**)realloc(countries_stats[t].dates_accepted, (countries_stats[t].counter_of_datesa + 1) * sizeof(*countries_stats[t].dates_accepted));
                                                countries_stats[t].dates_accepted[countries_stats[t].counter_of_datesa]=new Date;   
                                                if(!countries_stats[t].dates_accepted[countries_stats[t].counter_of_datesa]->setdate(date)){ std::cout<<"ERROR: IN USER INPUT INVALID DATE FORMAT\n";continue; } // error;
                                                countries_stats[t].counter_of_datesa++;
                                            }
                                        }
                                            std::cout<<"REQUEST ACCEPTED – HAPPY TRAVELS\n";
                                            continue;
                                        }
                                        else
                                        {
                                            for(int t=0;t<countries;t++)
                                            {
                                                if(strcmp(countries_stats[t].country,countryto.c_str())==0){
                                                countries_stats[t].dates_rejected=(Date**)realloc(countries_stats[t].dates_rejected, (countries_stats[t].counter_of_datesr + 1) * sizeof(*countries_stats[t].dates_rejected));
                                                countries_stats[t].dates_rejected[countries_stats[t].counter_of_datesr]=new Date;
                                                if(!countries_stats[t].dates_rejected[countries_stats[t].counter_of_datesr]->setdate(date)){ std::cout<<"ERROR: IN USER INPUT INVALID DATE FORMAT\n";continue; } // error;
                                                countries_stats[t].counter_of_datesr++;
                                                }
                                            }
                                            std::cout<<"REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n";
                                            continue;
                                        }
                                    }
                                    continue;
                                }
                            }
                    }
                    else
                    {
                        for(int t=0;t<countries;t++)
                        {
                            if(strcmp(countries_stats[t].country,countryto.c_str())==0){
                                countries_stats[t].dates_rejected=(Date**)realloc(countries_stats[t].dates_rejected, (countries_stats[t].counter_of_datesr + 1) * sizeof(*countries_stats[t].dates_rejected));
                                countries_stats[t].dates_rejected[countries_stats[t].counter_of_datesr]=new Date;
                                if(!countries_stats[t].dates_rejected[countries_stats[t].counter_of_datesr]->setdate(date)){ std::cout<<"ERROR: IN USER INPUT INVALID DATE FORMAT\n";continue; } // error;
                                countries_stats[t].counter_of_datesr++;
                            }
                        }
                        std::cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED\n";
                    }
                }
                else
                {
                    for(int t=0;t<countries;t++)
                    {
                        if(strcmp(countries_stats[t].country,countryto.c_str())==0){
                            countries_stats[t].dates_rejected=(Date**)realloc(countries_stats[t].dates_rejected, (countries_stats[t].counter_of_datesr + 1) * sizeof(*countries_stats[t].dates_rejected));
                            countries_stats[t].dates_rejected[countries_stats[t].counter_of_datesr]=new Date;
                            if(!countries_stats[t].dates_rejected[countries_stats[t].counter_of_datesr]->setdate(date)){ std::cout<<"ERROR: IN USER INPUT INVALID DATE FORMAT\n";continue; } // error;
                            countries_stats[t].counter_of_datesr++;
                        }
                    }
                    std::cout<<"REQUEST REJECTED – YOU ARE NOT VACCINATED\n";
                }
            }else std::cout<<"THERE IS NO SUCH COUNTRY IN THE DATABASE\n";
        }
        else if(firstelement=="/travelStats")
        {
            std::string date1;
            if (!(iss>>virusname>>date>>date1)) { std::cout<<"ERROR: IN USER INPUT\n";continue; } // error
            Date d1,d2;
            if(!d1.setdate(date) || !d2.setdate(date1)){ std::cout<<"ERROR: IN USER INPUT INVALID DATE FORMAT\n";continue; } // error
            if(iss>>countryfrom)
            {
                int totalr=0;
                int totala=0;
                for(i=0;i<countries;i++)
                {
                    if(strcmp(countries_stats[i].country,countryfrom.c_str())==0)
                    {
                        for(int j=0;j<countries_stats[i].counter_of_datesa;j++)
                        {
                            if(checkifbetweendates(*(countries_stats[i].dates_accepted[j]),d1,d2))
                            {
                                totala++;
                            }
                        }
                        for(int j=0;j<countries_stats[i].counter_of_datesr;j++)
                        {
                            if(checkifbetweendates(*(countries_stats[i].dates_rejected[j]),d1,d2))
                            {
                                totalr++;
                            }
                        }
                        break;                
                    }
                }
                    std::cout<<"TOTAL REQUESTS  "<<totala+totalr<<"\n";
                    std::cout<<"ACCEPTED  "<<totala<<"\n";
                    std::cout<<"REJECTED  "<<totalr<<"\n";
            }
            else
            {
                int totala=0;
                int totalr=0;
                for(i=0;i<countries;i++)
                {
                    
                    for(int j=0;j<countries_stats[i].counter_of_datesa;j++)
                    {
                        if(checkifbetweendates(*(countries_stats[i].dates_accepted[j]),d1,d2))
                        {
                            totala++;
                        }
                    }
                    
                    for(int j=0;j<countries_stats[i].counter_of_datesr;j++)
                    {
                        if(checkifbetweendates(*(countries_stats[i].dates_rejected[j]),d1,d2))
                        {
                            totalr++;
                        }
                    }           
                }
                std::cout<<"TOTAL REQUESTS  "<<totala+totalr<<"\n";
                std::cout<<"ACCEPTED  "<<totala<<"\n";
                std::cout<<"REJECTED  "<<totalr<<"\n";
                continue;              
            }
        }
        else if(firstelement=="/searchVaccinationStatus")
        {
            if (!(iss>>id)){ std::cout<<"ERROR: IN USER INPUT\n";continue; } // error
            char name_lastname[200];
            char age[10];
            int flag=0;
            if(!is_number(id)){ std::cout<<"ERROR: IN USER INPUT, INVALID ID NOT A NUMBER\n";continue; } // error
            for(i=0;i<numMonitors;i++)
            {
                memset(bf,0,100);
                strcpy(bf,"EVERYTHING");
                bufferwrite(clientsock[i],bufferSize,sizeof(char)*strlen(bf)+1,bf);
                
                memset(bf,0,100);
                strcpy(bf,id.c_str());
                bufferwrite(clientsock[i],bufferSize,sizeof(char)*strlen(bf)+1,bf);
                while(1)
                {
                    memset(bf,0,100);
                    bufferread(clientsock[i],bufferSize,bf);
                
                    if(strcmp(bf,"NOSUCHID")==0)break;
                    if(flag==0)
                    {
                        memset(name_lastname,0,100);
                        bufferread(clientsock[i],bufferSize,name_lastname);
                        std::cout<<id<<" "<<name_lastname<<std::endl;
                        memset(name_lastname,0,100);
                        bufferread(clientsock[i],bufferSize,name_lastname);
                        std::cout<<"AGE "<<name_lastname<<std::endl;
                        flag=1;
                    }
                      std::cout<<bf<<std::endl;
                }
            }
        }
        else if(firstelement=="/addVaccinationRecords")
        {
            if (!(iss>>countryto)){ std::cout<<"ERROR: IN USER INPUT\n";continue; } // error
            int flag=0;
            for(i=0;i<numMonitors;i++)
            {
                for(int j=0;j<countries_struct[i].counter;j++)
                {
                    if(strcmp(countryto.c_str(),countries_struct[i].countries[j])==0)
                    {
                        memset(bf,0,100);
                        strcpy(bf,"addrecord");
                        bufferwrite(clientsock[i],bufferSize,sizeof(char)*strlen(bf)+1,bf);
                        
                        flag=1;
                        
                        HTHash temphash=HTCreate(23);
                        char *virus=new char[100];

                        while(1){
                            memset(virus,0,100);
                            bufferread(clientsock[i],bufferSize,virus);
                            if(strcmp(virus,"DONEREADINGVIRUSES")==0)break;
                
                            tempblooms=new BloomFilter(bloomsize);
                            unsigned int * test=new unsigned int[tempblooms->getbms()]();
                            keep_blooms = (BloomFilter**) realloc(keep_blooms, (bloomscounter + 1) * sizeof(*keep_blooms));

                            for(int j=0;j<tempblooms->getbms();j++)
                                bufferread(clientsock[i],bufferSize,&test[j]);
                            tempblooms->setnewbitm(test);
                            keep_blooms[bloomscounter]=tempblooms;
                            bloomscounter++;
                            std::string tempstr(virus);
                            HTInsert(temphash,tempstr,(HTItem)(tempblooms));

                        }    
                        keep_hash = (HTHash*) realloc(keep_hash, (hashcounter + 1) * sizeof(*keep_hash));
                        keep_hash[hashcounter]=temphash;
                        hashcounter++;
                        for(int j=0;j<countries_struct[i].counter;j++)
                        {
                            std::string tempstr(countries_struct[i].countries[j]);
                            HTInsert(tables_of_blooms,tempstr,(HTItem)temphash);
                        }
                        delete[] virus;
                        break;
                    }
                }
                if(flag==1)break;
            }
        }
        else if(firstelement=="/exit")
        {  
            must_exit(numMonitors,countries_struct,countries_stats,countries,clientsock,forknums,bufferSize,bf,tables_of_blooms,bloomscounter,keep_blooms,keep_hash,hashcounter);
        }
        else
        {
            std::cout<<"WRONG USER INPUT\n";
        }
    }
   
}
