#include "man.h"
#include "unit1.h"
//HTTP://WWW.CODEFANS.NET

bool man::hu()const
{
    mjpai mjzu[14];

    int count=zuhand.getcount();
    int i;
    for(i=0;i<count;i++)
        mjzu[i]=zuhand.getmj(i);

    mjzu[i]=painewget;
    assert(i<14);
    return hupai(mjzu,count+1);
}

bool man::duting()const
{
    mjpai mjzu[14];

    int count=zuhand.getcount();
    int i;
    for(i=0;i<count;i++)
        mjzu[i]=zuhand.getmj(i);
    assert(i<=14);
    return ::duting(mjzu,count);
}

bool man::qingyise()const
{
    mjpai mjzu[22];

    int count=zuhand.getcount();
    int i;
    for(i=0;i<count;i++)
        mjzu[i]=zuhand.getmj(i);

    int j;
    count=zushowed.getcount();
    for(j=0;j<count;j++)
        mjzu[i++]=zushowed.getmj(j);

    mjzu[i]=painewget;

    assert(i<22);
    return isonecolor(mjzu,i+1);

}

bool man::hunyise()const
{
    mjpai mjzu[22];

    int count=zuhand.getcount();
    int i;
    for(i=0;i<count;i++)
        mjzu[i]=zuhand.getmj(i);

    int j;
    count=zushowed.getcount();
    for(j=0;j<count;j++)
        mjzu[i++]=zushowed.getmj(j);

    mjzu[i]=painewget;

    assert(i<22);
    return isonecolororzi(mjzu,i+1);

}

int man::qidui()const
{
    mjpai mjzu[14];

    int count=zuhand.getcount();
    int i;
    for(i=0;i<count;i++)
        mjzu[i]=zuhand.getmj(i);

    mjzu[i]=painewget;
    assert(i<14);

    return ::qidui(mjzu,count+1);

}


static bool take3(mjpai *a,int count,int totakefirst)
{
    int i,j;
    int flag[3];
    for(i=0;i<3;i++)   flag[i]=0;

    for(i=0;i<count;i++)
        if(a[i]==totakefirst)
            flag[0]=1;
        else if(a[i]==totakefirst+1)
            flag[1]=1;
        else if(a[i]==totakefirst+2)
            flag[2]=1;

    if(flag[0]!=1||flag[1]!=1||flag[2]!=1) return false;

    for(int k=0;k<3;k++)
    {
        for(i=0;i<count;i++)
            if(a[i]==totakefirst)
            {
                for(j=i;j<count-1;j++)
                    a[j]=a[j+1];
                break;
            }
        assert(i!=count);
        totakefirst++;
        count--;
    }
    return true;
}
bool man::yitiaolong()const
{
    mjpai mjzu[22];

    int count=zuhand.getcount();
    int i;
    for(i=0;i<count;i++)
        mjzu[i]=zuhand.getmj(i);

    int c=count,j;

    int flag[3][3];
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            flag[i][j]=0;

    for(i=0;i<zushowed.getkuaicount();i++)
        if(zushowed.isshun(i))
            switch(zushowed.getfirstofkuai(i))
            {
            case mjpai::wan1:
            case mjpai::wan4:
            case mjpai::wan7:
                flag[0][(zushowed.getfirstofkuai(i)-mjpai::wan1)/3]=1;
                break;
            case mjpai::tiao1:
            case mjpai::tiao4:
            case mjpai::tiao7:
                flag[1][(zushowed.getfirstofkuai(i)-mjpai::tiao1)/3]=1;
                break;
            case mjpai::bing1:
            case mjpai::bing4:
            case mjpai::bing7:
                flag[2][(zushowed.getfirstofkuai(i)-mjpai::bing1)/3]=1;
                break;
            }

    mjpai temp[14];
    int ctemp;
    for(i=0;i<3;i++)
    {   ctemp=c;
        for(j=0;j<ctemp;j++)
            temp[j]=mjzu[j];
        temp[j]=painewget;
        ctemp++;
        
        for(j=0;j<3;j++)
            if(flag[i][j]==0)
                if(!take3(temp,ctemp,mjpai::wan1+20*i+3*j))
                    break;
                else
                    ctemp-=3;

        if(j!=3)continue;
        if(::hupai(temp,ctemp)) return true;
    }
    return false;
}


int  man::hongqipiaopiao()const
{
     int i_s_zikehand=0,i_s_zikeshowed=0;

     int count,i;

     count =zushowed.getkuaicount();

     assert(count<=8);

     for(i=0;i<count;i++)
        if((zushowed.iske(i)||zushowed.isgang(i))&&zushowed.whatcolor(i)==4)
            i_s_zikeshowed++;

     mjpai mj[14];
     count=zuhand.getcount();
     assert(count<=13);

     for(i=0;i<count;i++)
        mj[i]=zuhand.getmj(i);

     mj[i]=painewget;
     assert(i<14);

     mishel(mj,i+1);

     i=0;count++;
     while(i<count&&!mj[i].iszi()) i++;//找到第一个是字的
     assert(i<=14);
     if(i==count-3)i_s_zikehand=1;   //手上1刻
     else if (i==count-6){           //字有6张
        if(mj[i+2]!=mj[i+3])i_s_zikehand=2; //3张+3张
        else i_s_zikehand=1;              //2张+4张
     }
     else if(i==count -5) i_s_zikehand=1;  //5张字,肯定是2+3
     else if(i==count -8)i_s_zikehand=2;   //8张字,肯定是4+4

     assert(i_s_zikehand+i_s_zikeshowed<=2);

     if(i_s_zikeshowed==2)return 1;
     else if(i_s_zikehand+i_s_zikeshowed==2) return 2;
     else return 0;
}


int man::pengpenghu()const
{
    int i,select[14];

    int count=zushowed.getkuaicount();
    for(i=0;i<count ;i++)
        if(zushowed.isshun(i))
            return 0;

    mjpai mj[14];

    count=zuhand.getcount();
    for(i=0;i<count;i++)
    {
        select[i]=0;
        mj[i]=zuhand.getmj(i);
    }

    mj[count]=painewget;
    select[count++]=0;

    int s_2=0;
    int sum,j,i_2;

    mishel(mj,count);
    for(i=0;i<count;i++)
    {
        if(select[i]) continue;
        sum=0;
        for(j=i+1;j<count;j++)
        {
            if(select[j]) continue;
            if(mj[i]==mj[j])
            { sum++;
                select[j]=1;
            }
        }
        sum++;
        if(sum==2){s_2++; select[i]=1; i_2=i;} //i_2,2张相同牌 的位置
        else if (sum==3) {select[i]=1;}
        else if (sum<2||sum>3)return 0;


    }


    if(s_2!=1) return 0; //两张相同的只有1副


    for(i=i_2;i<count-2;i++)
        mj[i]=mj[i+2];
    count-=2;

    for(i=0;i<zushowed.getkuaicount();i++)
        if(zushowed.iske(i)||zushowed.isgang(i)){
             mjpai mj1=zushowed.getfirstofkuai(i);
             if(!mj1.iszi())
             {
                mj[count]=mj[count+1]=mj[count+2]=mj1;
                count+=3;
             }
        }

    if(count<9)return 1;//潇洒必须至少9张相连

    mishel(mj,count);
    for(i=0;i<count-6;i+=3)
        if(mj[i+3]-mj[i]==1&&mj[i+6]-mj[i+3]==1)
            return 2;

    return 1;
}

bool man::menqing()const
{
    int i=0;
    while(i<zushowed.getkuaicount()&&zushowed.whatcolor(i)==5)i++;

    if(i<zushowed.getkuaicount())return false;
    else return true;
}

int man::fa()const
{
    int sum=0;
    for(int i=0;i<zushowed.getkuaicount();i++)
        if(zushowed.whatcolor(i)==5)
            sum++;

    return sum;
}

int man::zimingke()const
{
    int sum=0;
    //if(hongqipiaopiao())return 0; //取消是因为，在输牌的时候只检测 刻，不检测红旗飘飘
    //赢的人再检测红旗飘飘与字刻的冲突
    for(int i=0;i<zushowed.getkuaicount();i++)
        if(zushowed.iske(i)&&zushowed.whatcolor(i)==4)
            sum++;

    return sum;

}

int man::zianke()const
{
    int i,s_71=0,s_91=0,sum=0;
    //if(hongqipiaopiao())return 0;
    for(i=0;i<zuhand.getcount();i++)
        if(zuhand.getmj(i)==71)
            s_71++;
        else if(zuhand.getmj(i)==91)
            s_91++;

    if(s_71>=3) sum++;
    if(s_91>=3) sum++;

    return sum;
}

int man::ziminggang()const
{
    int sum=0;
    //if(hongqipiaopiao())return 0;
    for(int i=0;i<zushowed.getkuaicount();i++)
        if(zushowed.isgang(i)==1&&zushowed.whatcolor(i)==4)
            sum++;

    return sum;

}

int man::ziangang()const
{
    int sum=0;
    //if(hongqipiaopiao())return 0;
    for(int i=0;i<zushowed.getkuaicount();i++)
        if(zushowed.isgang(i)==2&&zushowed.whatcolor(i)==4)
            sum++;

    return sum;

}

int man::minggang()const
{
    int sum=0;
    //if(hongqipiaopiao())return 0;
    for(int i=0;i<zushowed.getkuaicount();i++)
        if(zushowed.isgang(i)==1&&zushowed.whatcolor(i)!=4)
            sum++;

    return sum;

}

int man::angang()const
{
    int sum=0;
    //if(hongqipiaopiao())return 0;
    for(int i=0;i<zushowed.getkuaicount();i++)
        if(zushowed.isgang(i)==2&&zushowed.whatcolor(i)!=4)
            sum++;

    return sum;

}

bool man::mo()
{
    bool bFa=false;
    painewget=Form1->mjot.takeone(true);
    while(painewget==mjpai::fa)
    {
        bFa=true;
        zushowed.insertmjkuai(mjpai::fa,3);
        painewget=Form1->mjot.takeone(false);
    }

    if(!fainhandhavedetected)
    {
        for(int i=0;i<zuhand.getcount();i++) //补手上的发
            if(zuhand.getmj(i)==mjpai::fa)
            {
                bFa=true;
                zuhand.takemj(i);
                zushowed.insertmjkuai(mjpai::fa,3);
                zuhand.insertmj(painewget);
                painewget=Form1->mjot.takeone(false);
                while(painewget==mjpai::fa)
                {
                    zushowed.insertmjkuai(mjpai::fa,3);
                    painewget=Form1->mjot.takeone(false);
                }
                i=-1;
            }
        fainhandhavedetected=true;
    }
    sort();
    return bFa;
}

void man::insertmj(mjpai mj)
 {
    zuhand.insertmj(mj);
 }

int man::numofdui()const
{
    mjpai mj[14];
    int count=zuhand.getcount();
    for(int i=0;i<zuhand.getcount();i++)
        mj[i]=zuhand.getmj(i);

    if(painewget>0)mj[count++]=painewget;

    mishel(mj,count);

    int c=0;
    for(int i=0;i<count-1;i++)
        if(mj[i]==mj[i+1])
        {
            if(i+2<count&&mj[i]==mj[i+2])
                continue;
            c++;
        }

    return c;

}


int man::numofse(int c)const
{
    int count=0;
    for(int i=0;i<zuhand.getcount();i++)
    {
        if(c==wan&&zuhand.getmj(i)>=mjpai::wan1&&zuhand.getmj(i)<=mjpai::wan9)
            count++;
        if(c==tiao&&zuhand.getmj(i)>=mjpai::tiao1&&zuhand.getmj(i)<=mjpai::tiao9)
            count ++;
        if(c==bing&&zuhand.getmj(i)>=mjpai::bing1&&zuhand.getmj(i)<=mjpai::bing9)
            count ++;

    }
    return count;
}

mjpaizu man::pengpeng_(const mjpai * mj,int c,mjpai newget)const
{
    assert(c);

    mjpaizu a,b;
    int n[14];
    for(int i=0;i<c;i++)
    {
        n[i]=0;
        if(a.getcount()>0&&a.find(mj[i])!=100)
            n[a.find(mj[i])]++;
        else
            {a.insertmj(mj[i]);n[a.find(mj[i])]++;}
    }

    n[c]=0;
    if(a.getcount()>0&&a.find(newget)!=100)
         n[a.find(newget)]++;
    else
        {a.insertmj(newget);n[a.find(newget)]++;}

    bool flag=false;
    for(int i=1;i<=4;i++) //找到个数最少的牌,都放在b里
    {
        for(int j=0;j<a.getcount();j++)
            if(n[j]==i){
                b.insertmj(a.getmj(j));
                flag=true;
            }
        if(flag) return b;
    }
    assert(0);//永不执行到这
}
mjpaizu man::dui_(const mjpai * mj,int c,mjpai newget)const
{
    assert(c==13);//打7对必须是没有碰,杠过

    //test for one error--------
    mjpai test[14];
    for(int i=0;i<13;i++)
        test[i]=mj[i];
    test[13]=newget;
    assert(!hupai(test,14));
    //------------------------------

    mjpaizu a,b;
    int n[14];
    for(int i=0;i<c;i++)
    {
        n[i]=0;
        if(a.getcount()>0&&a.find(mj[i])!=100)
            n[a.find(mj[i])]++;
        else
            {a.insertmj(mj[i]);n[a.find(mj[i])]++;}
    }
    n[c]=0;

    if(a.getcount()>0&&a.find(newget)!=100)
         n[a.find(newget)]++;
    else
        {a.insertmj(newget);n[a.find(newget)]++;}

    bool flag=false;

    for(int i=3;i>=1;i-=2) //找到3张的或1张的牌,放一种在b里
    {
        for(int j=0;j<a.getcount();j++)
            if(n[j]==i){
                b.insertmj(a.getmj(j));
                flag=true;
            }
        if(flag) return b;
    }
    assert(0);//永不执行到这
}
mjpaizu man::yise_(const mjpai * mj,int c,mjpai newget)const
{
    assert(c);

    mjpaizu a,b;
    int n[14];
    for(int i=0;i<c;i++)
    {
        n[i]=0;
        if(mj[i].isbing()&&se==bing) continue;
        if(mj[i].istiao()&&se==tiao) continue;
        if(mj[i].iswan()&&se==wan) continue;
        if(a.getcount()>0&&a.find(mj[i])!=100)
            n[a.find(mj[i])]++;
        else
            {a.insertmj(mj[i]);n[a.find(mj[i])]++;}
    }

    n[c]=0;

    if(!((newget.isbing()&&se==bing)||(newget.istiao()&&se==tiao)||(newget.iswan()&&se==wan)))
    {
        if(a.getcount()>0&&a.find(newget)!=100)
             n[a.find(newget)]++;
        else
            {a.insertmj(newget);n[a.find(newget)]++;}
    }

    bool flag=false;
    for(int i=1;i<=4;i++) //找到个数最少的牌,都放在b里
    {
        for(int j=0;j<a.getcount();j++)
            if(n[j]==i){
                b.insertmj(a.getmj(j));
                flag=true;
            }
        if(flag) return b;
    }
    return b;
}

