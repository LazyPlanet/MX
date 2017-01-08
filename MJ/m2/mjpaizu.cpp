#include "mjpaizu.h"
#include <algorithm>

mjpaizu::mjpaizu(mjpai *pmj_arr,int i_num)
{
        for(int i=0;i<i_num;i++)
            m_mjzu.push_back(pmj_arr[i]);
}



mjpai mjpaizu::getmj(int i) const
{
    assert(m_mjzu.size()&&unsigned(i)<m_mjzu.size());

    return  m_mjzu[i];              //operator[](i);
}

mjpai mjpaizu::takemj(int i)
{
    vector<mjpai>::iterator p=m_mjzu.begin();

    p+=i;

    mjpai mj=*p;

    m_mjzu.erase(p);
    
    return mj;
}

int mjpaizu::find(mjpai mj) const
{
    vector<mjpai>::const_iterator p;

    p=::find(m_mjzu.begin(),m_mjzu.end(),mj);

    if(p!=m_mjzu.end()) return p-m_mjzu.begin();

    return 100;
}

void  mjpaizu::insertmj(mjpai mj)
{
    m_mjzu.push_back(mj);
}

void mjpaizu::sort()
{
    ::sort(m_mjzu.begin(),m_mjzu.end());
}

void mjpaizu::clear(){
    m_mjzu.clear();
}


/*
mjpaizu::mjpaizu(mjpai *pmj_arr,int i_num):count(i_num),count_memory(i_num)
{
        mja_mj=new mjpai[i_num];
        assert(mja_mj);
        for(int i=0;i<i_num;i++)
            mja_mj[i]=pmj_arr[i];
}
mjpaizu::mjpaizu(mjpaizu &s)
{
    count_memory=count=s.getcount();

    if(count==0) return;

    mja_mj=new mjpai[count];
    assert(mja_mj);

    for(int i=0;i<count;i++)
        mja_mj[i]=s.mja_mj[i];
}


mjpai mjpaizu::getmj(int i) const
{
    assert(count&&i<count);

    return  mja_mj[i];
}

mjpai mjpaizu::takemj(int i)
{
    assert(count&&i<count);
    assert(count<=count_memory);
    if(i>=count)return 0;
    mjpai mj=mja_mj[i];

    for(;i<count-1;i++)
         mja_mj[i]=mja_mj[i+1];
    count--;

    return mj;
}

int mjpaizu::find(mjpai mj) const
{
    assert(count);
    int i;
    for(i=0;i<count;i++)
        if(mj==mja_mj[i])
            break;
    if(i==count)return 100;
    return i;
}

void  mjpaizu::insertmj(mjpai mj)
{
    assert(count<=count_memory);
    int i;
    mjpai * pmj_temp;
    bool flagnew=false;
    if(count==count_memory){    //插入时,如果预留空间用完,则再多分配5个,备用
        if(count_memory!=0){
            pmj_temp=new mjpai[count];
            assert(pmj_temp);
            for(i=0;i<count;i++)
                pmj_temp[i]=mja_mj[i];
            delete [] mja_mj;

            flagnew=true;
        }
        mja_mj=new mjpai[count+15];
        assert(mja_mj);
        count_memory=count+15;

        if(flagnew){
            for(i=0;i<count;i++)
                mja_mj[i]=pmj_temp[i];

            delete [] pmj_temp;
        }
    }

    mja_mj[count]=mj;
    count++;
}

mjpaizu & mjpaizu::operator =(mjpaizu &s)
{
    assert(count<=count_memory);
    if(this==&s) return *this;

    if(s.count>count_memory)
    {
        if(count_memory!=0) delete [] mja_mj;
        count_memory=s.count;
        mja_mj=new mjpai[count_memory];
        assert(mja_mj);
    }

    count=s.count;
    for(int i=0;i<count;i++)
         mja_mj[i]=s.mja_mj[i];

    return *this;
}

void mjpaizu::sort()
{
    assert(count);      //assert(0);
    mishel(mja_mj,count);
}

void mjpaizu::clear(){
    assert(count<=count_memory);
    if(count_memory==0)return;

    delete [] mja_mj;
    count= count_memory=0;
}
*/
//===========================
mjpaizu ting(const mjpai *pmj_arr,int i_num)
{
    int i;
    mjpaizu ret;

    for(i=mjpai::wan1;i<=mjpai::wan9;i++)
    {
        if(bukao(pmj_arr,i_num,i)) continue;
        if(hu(pmj_arr,i_num,i)) ret.insertmj(i);
    }
    for(i=mjpai::tiao1;i<=mjpai::tiao9;i++)
    {
        if(bukao(pmj_arr,i_num,i)) continue;
        if(hu(pmj_arr,i_num,i)) ret.insertmj(i);
    }
    for(i=mjpai::bing1;i<=mjpai::bing9;i++)
    {
        if(bukao(pmj_arr,i_num,i)) continue;
        if(hu(pmj_arr,i_num,i)) ret.insertmj(i);
    }
    for(i=mjpai::zhong;i<=mjpai::bai;i+=20)
    {
        if(bukao(pmj_arr,i_num,i)) continue;
        if(hu(pmj_arr,i_num,i)) ret.insertmj(i);
    }

    return ret;
}

