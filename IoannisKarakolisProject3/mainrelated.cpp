#include <iostream>
#include <string>
#include <ctime>
#include "modules.h"

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void Date::setdate_sixmonthslater(Date d)
{
     
    if(d.month<=6)
    {
        this->day=d.day;
        this->month=d.month+6;
        this->year=d.year;
    }
    else
    {
        this->day=d.day;
        this->month=(6-d.month)*(-1);
        this->year=d.year+1;
    }

}

int Date::setdate(std::string& date)
{
    std::string checky=date.substr(date.length()-4,date.length());
    if(!is_number(checky))return 0;
    std::string checkm="",checkd="",temp;

    checkd=checkd+date.substr(0,1);
    temp=date.substr(1,1);
    if(temp!="-")checkd=checkd+temp;
    if(!is_number(checkd))return 0;

    checkm=date.substr(checkd.length()+1,1);
    temp=date.substr(checkd.length()+2,1);
    if(temp!="-")checkm=checkm+temp;

    if(!is_number(checkm))return 0;
    if(checkd+"-"+checkm+"-"+checky!=date)return 0;

    this->day=std::stoi(checkd);
    this->month=std::stoi(checkm);
    this->year=std::stoi(checky);
    if(day>0&&day<=31&&month>0&&month<=12&&year>999&&year<9999)
    return 1;
    else
    return 0;
}
bool checkifbetweendates(Date &x,Date &y,Date &z)
{
    int flag1=0;
    if(x.year > y.year)
    {
       flag1=1;
    }
    else if(x.year < y.year) return 0;
    else
    {
        if(x.month > y.month)
        {
            flag1=1;
        }
        else  if(x.month<y.month) return 0;
        else
        {
            if(x.day>=y.day)
            {
                flag1=1;
            }
            else return 0;
        }
    }

    if(z.year > x.year)
    {
        return 1;
    }
    else if(z.year < x.year) return 0;
    else
    {
      if(z.month > x.month)
      {
        return 1;
      }
      else  if(z.month<x.month) return 0;
      else
      {
          if(z.day>=x.day)
          {
            return 1;
          }
          else return 0;
      }
    }
}

void printeveything(HTHash table,std::string &citizenID)
{
    int i,a=table->size;
    int id=std::stoi(citizenID);
    for(i=0;i<a;i++)
    {
        Hashnode *templist=table[i].list;
        while(templist!=NULL)
        {
            Virus_structs* getvirus;
            getvirus=(Virus_structs*)templist->item;
            listnode *temp;
            if(temp=getvirus->vaccinated->search(id))
            {
                std::cout<<templist->key<<" YES "<<temp->data<<"\n";
            }
            else if(temp=getvirus->not_vaccinated->search(id))
            {
                std::cout<<templist->key<<" NO\n";
            }
            templist=templist->Link;
        }
    }
}
void print_country_statistics(HTHash filetable,Date date_1,Date date_2,std::string &secondelement,void*getv)
{
    int vac_between=0,size,i;
    Date date_between;
    Virus_structs*getvirus=(Virus_structs*)getv;
    void*getrec;
    listnode *templist=getvirus->vaccinated->getbase();
    std::string tempstr;
    records *nrec;

    while(templist!=NULL)
    {
        tempstr=std::to_string(templist->key);
        if(HTGet(filetable,tempstr,&getrec))
        {
            nrec=(records*)getrec;
            if(nrec->getcountry()==secondelement)
            {
                date_between.setdate(templist->data);
                if(checkifbetweendates(date_between,date_1,date_2))
                {
                    vac_between++;
                }
           }
        }
        templist=templist->next;
    }
    int not_vac=(getvirus->not_vaccinated->basesize_country(filetable,secondelement));
    int vac=(getvirus->vaccinated->basesize_country(filetable,secondelement));
    std::cout<<secondelement<<": "<<vac_between<<" "<<((double)vac_between/(vac+not_vac))*100<<"%\n";
}

void print_country_statistics_age(HTHash filetable,Date date_1,Date date_2,std::string &secondelement,void*getv)
{

    int allvaccinated=0,vac_between=0,size,i;
    Date date_between;
    Virus_structs*getvirus=(Virus_structs*)getv;
    void*getrec;
    listnode *templist=getvirus->vaccinated->getbase();
    int ptw=0,pfy=0,psy=0,pabvsy=0,intage;
    std::string tempstr;
    records *nrec;

    while(templist!=NULL)
    {
        tempstr=std::to_string(templist->key);
        if(HTGet(filetable,tempstr,&getrec))
        {
            nrec=(records*)getrec;
            if(nrec->getcountry()==secondelement)
            {
                date_between.setdate(templist->data);
                intage=nrec->getage();

                if(checkifbetweendates(date_between,date_1,date_2))
                {
                    if(intage<=20)ptw++;
                    else if(intage<=40)pfy++;
                    else if(intage<=60)psy++;
                    else pabvsy++;
                }
           }
        }
        templist=templist->next;
    }
    int not_vac=(getvirus->not_vaccinated->basesize_country(filetable,secondelement));
    int vac=(getvirus->vaccinated->basesize_country(filetable,secondelement));
    std::cout<<secondelement<<": \n"
    <<"0-20 "<<ptw<<" "<<((double)ptw/(vac+not_vac))*100<<"%\n"
    <<"20-40 "<<pfy<<" "<<((double)pfy/(vac+not_vac))*100<<"%\n"
    <<"40-60 "<<psy<<" "<<((double)psy/(vac+not_vac))*100<<"%\n"
    <<"60+ "<<pabvsy<<" "<<((double)pabvsy/(vac+not_vac))*100<<"%\n";
}
void insert_records_file(HTHash &filetable,HTHash &virustable,HTHash &countriestable,int bloomsize,std::string&id,std::string &firstname,std::string &lastname,std::string &country,std::string &age,std::string &virus,std::string &vacinated,std::string &date)
{
    if(!(is_number(id)&&is_number(age))){std::cout<<"ERROR IN RECORD "<<id<<" "<<firstname<<" "<<lastname<<" "<<country<<""<<age<<" "<<virus<<" "<<vacinated<<" "<<date<<" invalid id/age \n";return;};
    records* get;
    void* getvoid;
    void *getvoidp;
    Virus_structs *getvirus;
    int intid=std::stoi(id);

    if(HTGet(filetable,id,&getvoid))
    {
        get=(records*)getvoid;
        if(get->getgeneralinfo()!=(firstname+lastname+country)||get->getage()!=std::stoi(age))
        {
            std::cout<<"ERROR IN RECORD "<<id<<" "<<firstname<<" "<<lastname<<" "<<country<<" "<<age<<" "<<virus<<" "<<vacinated<<" "<<date<<" Same id Different name/lastname/country/age\n";
            return;
        }
    }
    else
    {

        if(!HTGet(countriestable,country,&getvoid))
        {
            HTInsert(countriestable,country,nullptr);
        }

        records *newrec;
        newrec=new records(intid,firstname,lastname,country,std::stoi(age));
        HTInsert(filetable,id,(HTItem)(newrec));
    }

    if(HTGet(virustable,virus,&getvoid))
    {
        getvirus=(Virus_structs*)getvoid;

        if(getvirus->not_vaccinated->searchbool(std::stoi(id)) || getvirus->vaccinated->searchbool(std::stoi(id)))
        {
            std::cout<<"ERROR CITIZEN "<<id<<" ALREADY VACCINATED.\n";
            return;
        }

        if(vacinated=="YES")
        {
            getvirus->vaccinated->insert(intid,date);
            getvirus->bloom->insert(id);
            return;
        }


        getvirus->not_vaccinated->insert(intid,date);
        return;
    }
    else
    {
        Virus_structs *newrec;
        newrec=new virus_structs;
        newrec->bloom=new BloomFilter(bloomsize);
        newrec->not_vaccinated=new skiplist;
        newrec->vaccinated=new skiplist;
        getvirus=(Virus_structs*)getvoid;

        if(vacinated=="YES")
        {
            newrec->bloom->insert(id);
            newrec->vaccinated->insert(intid,date);
            HTInsert(virustable,virus,(HTItem)(newrec));
            return;
        }
        newrec->not_vaccinated->insert(intid,date);
        HTInsert(virustable,virus,(HTItem)(newrec));
        return;
    }

}

void insert_records_user(HTHash &filetable,HTHash &virustable,HTHash &countriestable,int bloomsize,std::string&id,std::string &firstname,std::string &lastname,std::string &country,std::string &age,std::string &virus,std::string &vacinated,std::string &date)
{
    if(!(is_number(id)&&is_number(age))){std::cout<<"ERROR IN RECORD "<<id<<" "<<firstname<<" "<<lastname<<" "<<country<<""<<age<<" "<<virus<<" "<<vacinated<<" "<<date<<" invalid id/age \n";return;};
    records* get;
    void* getvoid;
    Virus_structs *getvirus;

    int intid=std::stoi(id);

    if(HTGet(filetable,id,&getvoid))
    {
        get=(records*)getvoid;
        if(get->getgeneralinfo()!=(firstname+lastname+country)||get->getage()!=std::stoi(age))
        {
            std::cout<<"ERROR IN RECORD "<<id<<" "<<firstname<<" "<<lastname<<" "<<country<<" "<<age<<" "<<virus<<" "<<vacinated<<" "<<date<<" Same id Different name/lastname/country/age\n";
            return;
        }
    }
    else
    {

        if(!HTGet(countriestable,country,&getvoid))
        {
            HTInsert(countriestable,country,nullptr);
        }

        records *newrec;
        newrec=new records(intid,firstname,lastname,country,std::stoi(age));
        HTInsert(filetable,id,(HTItem)(newrec));
    }

    if(HTGet(virustable,virus,&getvoid))
    {
        getvirus=(Virus_structs*)getvoid;
        listnode*temp;
        if(temp=getvirus->vaccinated->search(std::stoi(id)))
        {
            std::cout<<"ERROR CITIZEN "<<id<<" ALREADY VACCINATED ON "<<temp->data<<"\n";
            return;
        }


        if(vacinated=="YES")
        {
            if(getvirus->not_vaccinated->searchbool(std::stoi(id)))getvirus->not_vaccinated->deletedata(std::stoi(id));
            getvirus->vaccinated->insert(intid,date);
            getvirus->bloom->insert(id);
            return;
        }
        if(getvirus->not_vaccinated->searchbool(std::stoi(id)))
        {
            std::cout<<"ERROR CITIZEN "<<id<<" ALREADY RECORDED AS NOT VACCINATED "<<"\n";
            return;
        }
        getvirus->not_vaccinated->insert(intid,date);
        return;
    }
    else
    {
        Virus_structs *newrec;
        newrec=new virus_structs;
        newrec->bloom=new BloomFilter(bloomsize);
        newrec->not_vaccinated=new skiplist;
        newrec->vaccinated=new skiplist;
        getvirus=(Virus_structs*)getvoid;

        if(vacinated=="YES")
        {
            newrec->bloom->insert(id);
            newrec->vaccinated->insert(intid,date);
            HTInsert(virustable,virus,(HTItem)(newrec));
            return;
        }
        newrec->not_vaccinated->insert(intid,date);
        HTInsert(virustable,virus,(HTItem)(newrec));
        return;
    }

}

void get_todays_date(std::string &date)
{
    time_t     now = time(0);
    struct tm  tstruct;
    tstruct = *localtime(&now);

    strftime((char*)date.c_str(), sizeof(date), "%d-%m-%Y", &tstruct);
}


void print_all_nonvaccinated(HTHash filetable,void * getvoid)
{
    Virus_structs*temp=(virus_structs*)getvoid;
    listnode*nonvac=temp->not_vaccinated->getbase();
    void *getrec;
    records*rec;
    std::string tempstr;
    while(nonvac!=nullptr)
    {
        tempstr=std::to_string(nonvac->key);
        if(HTGet(filetable,tempstr,&getrec))
        {
            rec=(records*)getrec;
            rec->records_Print();
        }
        nonvac=nonvac->next;
    }
    return;
}
