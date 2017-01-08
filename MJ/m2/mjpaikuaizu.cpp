#include "mjpaikuaizu.h"

int mjpaikuaizu::whatcolor(int i_ikuai)const
{
    assert(getcount()>0&&i_ikuai<i_numofkuai);
    int first;
    first = i_beginofkuai[i_ikuai];

    if(getmj(first)<=19&&getmj(first)>=11)return 1;
    if(getmj(first)<=39&&getmj(first)>=31)return 2;
    if(getmj(first)<=59&&getmj(first)>=51)return 3;
    if(getmj(first)==71||getmj(first)==91)return 4;
    if(getmj(first)==81)return 5;
    return 0;
}


bool mjpaikuaizu::iske(int i_ikuai)const
{
    assert(getcount()>0&&i_ikuai<i_numofkuai);

    //if(getcount()-i_beginofkuai[i_ikuai]<3)return false ;

    int end = (i_ikuai==i_numofkuai-1)?getcount():i_beginofkuai[i_ikuai+1];

    int first;
    first = i_beginofkuai[i_ikuai];

    if(end-first!=3) return false;

    if(getmj(first)==getmj(first+1)&&getmj(first)==getmj(first+2))
        return true;
    return false;
}

bool mjpaikuaizu::isshun(int i_ikuai)const
{
    assert(getcount()>0&&i_ikuai<i_numofkuai);

    int end = (i_ikuai==i_numofkuai-1)?getcount():i_beginofkuai[i_ikuai+1];

    int first;
    first = i_beginofkuai[i_ikuai];

    if(end-first!=3) return false;

    int max=0,min=100;
    for(int i=first;i<first+3;i++){
        if( getmj(i)>max) max=getmj(i);
        if(getmj(i)<min)min=getmj(i);
    }

    if(max-min==2&&((min+1==getmj(first))||(min+1==getmj(first+1))||(min+1==getmj(first+2))))
         return true;
    return false;

}

int mjpaikuaizu::isgang(int i_ikuai)const
{
    assert(getcount()>0&&i_ikuai<i_numofkuai);

    int end = (i_ikuai==i_numofkuai-1)?getcount():i_beginofkuai[i_ikuai+1];

    int first;
    first = i_beginofkuai[i_ikuai];

    if(end-first!=4) return false;

    if(getmj(first)==getmj(first+1)&&getmj(first)==getmj(first+2)&&getmj(first)==getmj(first+3))
        if(isangang[i_ikuai])
            return 2;
        else return 1;
    return 0;
}

void mjpaikuaizu::insertmjkuai(mjpai firstmj,int type,bool isangang)
{
    assert(i_numofkuai<8);
    assert(firstmj<=91&&firstmj>=11);
    assert(type>=0&&type<4);


    switch(type)
    {
        case 0:
            assert(firstmj!=81);
            assert((firstmj>10&&firstmj<18)||(firstmj>30&&firstmj<38)||
                (firstmj>50&&firstmj<58));
                
            i_beginofkuai[i_numofkuai]=getcount();
            i_numofkuai++;

            insertmj(firstmj);
            insertmj(firstmj+1);
            insertmj(firstmj+2);
            
            break;
        case 1:
        case 2:
            assert(firstmj!=81);
            i_beginofkuai[i_numofkuai]=getcount();
            mjpaikuaizu::isangang[i_numofkuai]=isangang;
            i_numofkuai++;

            for(int i=0;i<type+2;i++)
                insertmj(firstmj);


            break;
        case 3:
            assert(firstmj==81);
            i_beginofkuai[i_numofkuai]=getcount();
            i_numofkuai++;

            insertmj(firstmj);

            break;
    }

}

void mjpaikuaizu::deletemjkuai(int i)
{
    assert(i<i_numofkuai);
   // int count=getcount();

    int i_todelete=i_beginofkuai[i];
    int i_next;

    if(i==i_numofkuai-1)
        i_next=getcount();
    else
        i_next=i_beginofkuai[i+1];

    for(int i_i=i_todelete;i_i<i_next;i_i++){
        takemj(i_todelete);//一张一张删除
    }

    for(int i_i=i;i_i<i_numofkuai-1;i_i++)
    {
        i_beginofkuai[i_i]=i_beginofkuai[i_i+1]-(i_next-i_todelete);
        //标志位 重新组合
        isangang[i_i]=isangang[i_i+1];
    }

    i_numofkuai--; //少了一组
}

mjpaikuaizu & mjpaikuaizu::operator =(mjpaikuaizu &s)
{
    if(this==&s) return *this ;
    
    mjpaizu::operator =(s);

    i_numofkuai=s.i_numofkuai;

    for(int i=0;i<8;i++){
        i_beginofkuai[i]=s.i_beginofkuai[i];
        isangang[i]=s.isangang[i];
    }

    return *this;
}   

mjpaikuaizu::mjpaikuaizu(mjpaikuaizu &s):mjpaizu(s) //调用父类的拷贝构造
{
    //mjpaizu::mjpaizu(s); //不能放在这,呵呵.实际上此处调用并非对父类初始化.
    //而是生成了一临时对象(构造函数调用相当于生成临时对象)

    i_numofkuai=s.i_numofkuai;

    for(int i=0;i<8;i++){
        i_beginofkuai[i]=s.i_beginofkuai[i];
        isangang[i]=s.isangang[i];
    }

}

int mjpaikuaizu::havezikeorgang()const
{
    int i, i_sumke=0, i_sumgang=0   ,i_sumangang=0;
    for(i=0;i<i_numofkuai;i++)
    {
        if(whatcolor(i)==4)
        {
            if(iske(i)) i_sumke++;
            if(isgang(i)==1) i_sumgang++;
            if(isgang(i)==2) i_sumangang++;
        }
    }
    return i_sumke+10*i_sumgang+100*i_sumangang;
}

int mjpaikuaizu::havegang()const
{
    int i, i_sumgang=0   ,i_sumangang=0;
    for(i=0;i<i_numofkuai;i++)
    {
        if(whatcolor(i)!=4&&whatcolor(i)!=5)
        {
            //if(iske(i)) i_sumke++;
            if(isgang(i)==1) i_sumgang++;
            if(isgang(i)==2) i_sumangang++;
        }
    }
    return i_sumgang+10*i_sumangang;
}

int mjpaikuaizu::havefa()const
{
    int i, i_sum=0;
    for(i=0;i<i_numofkuai;i++)
    {
        if(whatcolor(i)==5)
        {
            i_sum++;
        }
    }
    return i_sum;
}

void mjpaikuaizu::bugang(mjpai mj,bool isangang)
{
    int i;

    for(i=0;i<i_numofkuai;i++)
    {
        if(iske(i))
        {
            if(mj==getmj(i_beginofkuai[i]))
            {
                deletemjkuai(i);
                insertmjkuai(mj,2,isangang);
                break;
            }

        }
    }
    assert(i!=i_numofkuai);;
}


int mjpaikuaizu::findke(mjpai mj)const
{
    assert(mj>10&&mj<92);

    for(int i=0;i<i_numofkuai;i++)
        if(iske(i)&&getmj(i_beginofkuai[i])==mj)
        //{
            return i;
            //break;
        //}

    return 100;
}


mjpai mjpaikuaizu::getfirstofkuai(int kuai)const
{
    return mjpaizu::getmj(i_beginofkuai[kuai]);
}

