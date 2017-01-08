#include "everyman.h"
#include "unit1.h"

void computerleft::chu()
{
    #ifndef NLABEL
    Form1->Label5->Caption=wantto;
    #endif

    mjpai mj=man::discard();

    //mj=painewget;//test

    if(mj==painewget)
    {
         Form1->mjots.insertmjleft(mj);
         painewget=0;
    }
    else
    {
        Form1->mjots.insertmjleft(mj);
        zuhand.takemj(zuhand.find(mj));
        zuhand.insertmj(painewget);
        painewget=0;
    }
}

void computerright::chu()
{
    #ifndef NLABEL
    Form1->Label7->Caption=wantto;
    #endif

    mjpai mj=man::discard();

    if(mj==painewget)
    {
         Form1->mjots.insertmjright(mj);
         painewget=0;
    }
    else
    {
        Form1->mjots.insertmjright(mj);
        zuhand.takemj(zuhand.find(mj));
        zuhand.insertmj(painewget);
        painewget=0;
    }
}

void computerup::chu()
{

    mjpai mj=man::discard();

    if(mj==painewget)
    {
         Form1->mjots.insertmjup(mj);
         painewget=0;
    }
    else
    {
        Form1->mjots.insertmjup(mj);
        zuhand.takemj(zuhand.find(mj));
        zuhand.insertmj(painewget);
        painewget=0;
    }
    #ifndef NLABEL
    zuhand.sort();
    Form1->Label6->Caption="";
    for(int i=0;i<zuhand.getcount();i++)
        Form1->Label6->Caption=Form1->Label6->Caption+AnsiString(zuhand.getmj(i))+" ";
    //wantto;
    #endif
}

bool man::wanttobugang()const   //只是有补就补,但是也不全,
{
    int i;

    if(wantto==dui)return false;//打七对不补杠

    i=zushowed.findke(painewget);
    if(i!=100) return true;

    int c=0;
    for(i=0;i<zuhand.getcount();i++)
        if(painewget==zuhand.getmj(i))
            c++;

    if(c==3)//可以补杠,
    {
        if(isting)     //可以听,则看杠后能不能继续听,能,就杠
        {
            mjpai mj[14];
            mjpai mjs=painewget;
            for(i=0;i<zuhand.getcount();i++)
                mj[i]=zuhand.getmj(i);

           ::mishel(mj,zuhand.getcount());

            for(int i=0;i<zuhand.getcount()-3;i++)
                if(mj[i]==mjs)
                {
                    for(int j=i;j<zuhand.getcount()-3;j++)
                        mj[j]=mj[j+3];
                    break;
                }

            if(::ting(mj,zuhand.getcount()-3).getcount()>0)
                return true;
            else return false;
        }

        return true;
    }

    return false;
}

bool man::wanttohu(bool iszimo) //检测和牌合不合规则亦在这里
{
    if(!iszimo) //不是自摸,则到桌上取别人刚出的牌
    {
        mjpai mj=Form1->mjots.getnewest();

        painewget=mj;
    }

    if(!this->hu())
    {
        if(!iszimo)painewget=0;
        return false;
    }

    if(iszimo)
    {
        return true;
    }

    if(zushowed.getkuaicount()==0)
    {
        if(!iszimo)painewget=0;
        return true;
    }
    if(fa()||zimingke()||zianke()||
        ziminggang()||ziangang()||minggang()||
        angang())
    {
        if(!iszimo)painewget=0;
        return true;
    }

    if(hunyise()||pengpenghu()||qidui())
    {
        if(!iszimo)painewget=0;
        return true;
    }

    if(!iszimo)painewget=0;
    return false;
    //碰了以后若无发、字刻、杠：普通和只能自摸，大和（碰碰和、清一色、混一色、七对）可以炮、摸
}


bool man::wanttopeng()const
{
    //这里先简单实现有吃就吃,有碰就碰

    if(wantto==dui)return false;//打七对 不碰

    mjpai mj[13];
    int mjs;

    if(isting) return false; //听牌就不碰

    for(int i=0;i<zuhand.getcount();i++)
        mj[i]=zuhand.getmj(i);

    mjs=Form1->mjots.getnewest();
    return (::pengandgang(mj,zuhand.getcount(),mjs)==1)?true:false;
}

bool man::wanttogang()const
{
    //这里先简单实现有吃就吃,有碰就碰
    mjpai mj[13];
    int mjs;

    if(wantto==dui)return false;//打七对 不杠

    mjs=Form1->mjots.getnewest();

    for(int i=0;i<zuhand.getcount();i++)
        mj[i]=zuhand.getmj(i);

    if(::pengandgang(mj,zuhand.getcount(),mjs)==2)//如果可以杠且听牌
    {                                              //如果杠后仍听,则杠
        if(isting)
        {
            mishel(mj,zuhand.getcount());

            for(int i=0;i<zuhand.getcount()-3;i++)
                if(mj[i]==mjs)
                {
                    for(int j=i;j<zuhand.getcount()-3;j++)
                        mj[j]=mj[j+3];
                    break;
                }

            if(::ting(mj,zuhand.getcount()-3).getcount()>0)
                return true;
            else return false;
        }


        return true;
    }
    return false;
}

void man::peng()
{
    soundpeng();
    //这里先简单的找牌吃碰杠
    mjpai mjs=Form1->mjots.takenewest();

    if(!fainhandhavedetected)
    {
        for(int i=0;i<zuhand.getcount();i++) //补手上的发
            if(zuhand.getmj(i)==mjpai::fa)
            {
                mjpai temp;
                zuhand.takemj(i);
                zushowed.insertmjkuai(mjpai::fa,3);
                temp=Form1->mjot.takeone(false);
                while(temp==mjpai::fa)
                {
                    zushowed.insertmjkuai(mjpai::fa,3);
                    temp=Form1->mjot.takeone(false);
                }
                zuhand.insertmj(temp);
            i=-1;
            }
        fainhandhavedetected=true;
    }

    int i=zuhand.find(mjs);
    assert(i!=100);
    zuhand.takemj(i);

    i=zuhand.find(mjs);
    assert(i!=100);
    zuhand.takemj(i);

    zushowed.insertmjkuai(mjs,1);

                if(wantto==yise)//如果在打一色,且杠了别的牌,就不打一色了
                {
                    if(se==wan&&(mjs.isbing()||mjs.istiao()))
                        wantto=nothing;
                    if(se==tiao&&(mjs.isbing()||mjs.iswan()))
                        wantto=nothing;
                    if(se==bing&&(mjs.iswan()||mjs.istiao()))
                        wantto=nothing;
                }


    painewget=zuhand.takemj(zuhand.getcount()-1);

    for(i=0;i<3;i++)
        Form1->mjots.insertshowedinhand(mjs);

}



void man::gang()
{
    soundgang();
    //这里先简单的找牌吃碰杠
    mjpai mjs=Form1->mjots.takenewest();


    if(!fainhandhavedetected)
    {
        for(int i=0;i<zuhand.getcount();i++) //补手上的发
            if(zuhand.getmj(i)==mjpai::fa)
            {
                mjpai temp;
                zuhand.takemj(i);
                zushowed.insertmjkuai(mjpai::fa,3);
                temp=Form1->mjot.takeone(false);
                while(temp==mjpai::fa)
                {
                    zushowed.insertmjkuai(mjpai::fa,3);
                    temp=Form1->mjot.takeone(false);
                }
                zuhand.insertmj(temp);
            i=-1;
            }
        fainhandhavedetected=true;
    }

    int mjn=Form1->mjot.takeone(false);
    while(mjn==mjpai::fa)
    {
        zushowed.insertmjkuai(mjpai::fa,3);
        mjn=Form1->mjot.takeone(false);
    }

    int i=zuhand.find(mjs);
    assert(i!=100);
    zuhand.takemj(i);

    i=zuhand.find(mjs);
    assert(i!=100);
    zuhand.takemj(i);

    i=zuhand.find(mjs);
    assert(i!=100);
    zuhand.takemj(i);

    zushowed.insertmjkuai(mjs,2,false);
    
                if(wantto==yise)//如果在打一色,且杠了别的牌,就不打一色了
                {
                    if(se==wan&&(mjs.isbing()||mjs.istiao()))
                        wantto=nothing;
                    if(se==tiao&&(mjs.isbing()||mjs.iswan()))
                        wantto=nothing;
                    if(se==bing&&(mjs.iswan()||mjs.istiao()))
                        wantto=nothing;
                }

    painewget=mjn;

    for(i=0;i<4;i++)
        Form1->mjots.insertshowedinhand(mjs);
}

void man::bugang()//有两种情况,可能新摸的要杠,或者,手上早有4张
{               //wanttobugang可以配合(相当于叫),属于电脑智能一部分
    soundgang();
    mjpai mj;
    mjpai mja[14];

    if(!fainhandhavedetected)
    {
        for(int i=0;i<zuhand.getcount();i++) //补手上的发
            if(zuhand.getmj(i)==mjpai::fa)
            {
                mjpai temp;
                zuhand.takemj(i);
                zushowed.insertmjkuai(mjpai::fa,3);
                temp=Form1->mjot.takeone(false);
                while(temp==mjpai::fa)
                {
                    zushowed.insertmjkuai(mjpai::fa,3);
                    temp=Form1->mjot.takeone(false);
                }
                zuhand.insertmj(temp);
            i=-1;
            }
        fainhandhavedetected=true;
    }
    
    mj=Form1->mjot.takeone(false);
    while(mj==mjpai::fa)
    {
        zushowed.insertmjkuai(mjpai::fa,3);
        mj=Form1->mjot.takeone(false);
    }

    int i=zushowed.findke(painewget);//看看摸到的牌能不能补杠
    if(i!=100)
    {
        zushowed.bugang(painewget);

                if(wantto==yise)//如果在打一色,且杠了别的牌,就不打一色了
                {
                    if(se==wan&&(painewget.isbing()||painewget.istiao()))
                        wantto=nothing;
                    if(se==tiao&&(painewget.isbing()||painewget.iswan()))
                        wantto=nothing;
                    if(se==bing&&(painewget.iswan()||painewget.istiao()))
                        wantto=nothing;
                }
                
        painewget=mj;
        return;
    }

    int s,j;


    for(i=0;i<zuhand.getcount();i++)//看看手上的牌能不能补杠
    {
        mja[i]=zuhand.getmj(i);
        if(zushowed.findke(mja[i])!=100)
        {
            zushowed.bugang(mja[i],false);
            zuhand.takemj(zuhand.find(mja[i]));
            //zuhand.insertmj(mj);
            zuhand.insertmj(painewget);
            painewget=mj;

                Form1->mjots.insertshowedinhand(mja[i]);
                if(wantto==yise)//如果在打一色,且杠了别的牌,就不打一色了
                {
                    if(se==wan&&(mja[i].isbing()||mja[i].istiao()))
                        wantto=nothing;
                    if(se==tiao&&(mja[i].isbing()||mja[i].iswan()))
                        wantto=nothing;
                    if(se==bing&&(mja[i].iswan()||mja[i].istiao()))
                        wantto=nothing;
                }
            return ;
        }
    }

    mja[i]=painewget;

    for(i=0;i<zuhand.getcount()+1;i++) //剩下的就是手上有4张相同
    {
        s=0;
        for(j=i+1;j<zuhand.getcount()+1;j++)
            if(mja[i]==mja[j]) s++;
        if(s==3)
            break;
    }
    //assert(c==3);
    assert(i!=zuhand.getcount()+1);
    zushowed.insertmjkuai(mja[i],2,true);
    
                if(wantto==yise)//如果在打一色,且杠了别的牌,就不打一色了
                {
                    if(se==wan&&(mja[i].isbing()||mja[i].istiao()))
                        wantto=nothing;
                    if(se==tiao&&(mja[i].isbing()||mja[i].iswan()))
                        wantto=nothing;
                    if(se==bing&&(mja[i].iswan()||mja[i].istiao()))
                        wantto=nothing;
                }

    for(j=0;j<3;j++)
    {
        zuhand.takemj(zuhand.find(mja[i]));
    }

    if(mja[i]!=painewget)
    {
         zuhand.takemj(zuhand.find(mja[i]));
         zuhand.insertmj(painewget);
    }

    painewget=mj;
    sort();
}

mjpai man::discard()  //电脑出牌方法的核心
{
    mjpai mj[14];
    int c,i;

    assert(painewget!=0);

    if(Form1->mjots.getcount()<10&&wantto==nothing&&Form1->difficult==3)//前阶段判断大体的3个方向
    {                                                          //难度3 才有
        bool flag;
        if(zuhand.getcount()-numofse(wan)<=5||(zuhand.getcount()-numofse(wan)==6
            &&(Form1->paishu[2]-Form1->avepaishu>=20-6||Form1->paishu[4]-Form1->avepaishu>=15-6)))
        {    //六张的时候如果要打一色,必须是一色的分数比较高
            flag=false;
            //在已经亮出的牌中找,如果不符合要打的一色的牌,就算了,不打一色
            for(int i=0;i<zushowed.getkuaicount();i++)
                 if(zushowed.whatcolor(i)==2||zushowed.whatcolor(i)==3)
                    flag=true;

            if(!flag)
            {
                se=wan; wantto=yise;
            }
        }
        if(zuhand.getcount()-numofse(tiao)<=5||(zuhand.getcount()-numofse(tiao)==6
            &&(Form1->paishu[2]-Form1->avepaishu>=20-6||Form1->paishu[4]-Form1->avepaishu>=15-6))) {
            flag=false;
            for(int i=0;i<zushowed.getkuaicount();i++)
                 if(zushowed.whatcolor(i)==1||zushowed.whatcolor(i)==3)
                    flag=true;

            if(!flag)
            {
                se=tiao; wantto=yise;
            }
        }
        if(zuhand.getcount()-numofse(bing)<=5||(zuhand.getcount()-numofse(bing)==6
            &&(Form1->paishu[2]-Form1->avepaishu>=20-6||Form1->paishu[4]-Form1->avepaishu>=15-6))) {
            flag=false;
            for(int i=0;i<zushowed.getkuaicount();i++)
                 if(zushowed.whatcolor(i)==2||zushowed.whatcolor(i)==1)
                    flag=true;

            if(!flag)
            {
                se=bing; wantto=yise;
            }
        }

        if(zuhand.getcount()-numofdui()*2<=5||(zuhand.getcount()-numofdui()*2==6
            &&(Form1->paishu[6]-Form1->avepaishu>=20-6||Form1->paishu[8]-Form1->avepaishu>=20-6)))
        {
            if(zuhand.getcount()==13)
                wantto=dui;
            else wantto=pengpeng;
        }


    }

    c=zuhand.getcount();
    for(i=0;i<c;i++)
        mj[i]=zuhand.getmj(i);


    if(Form1->difficult==3){  //难度3 才有
        mjpaizu zu;

        switch(wantto)   //3大方向
        {
        case yise:
            if(yise_(mj,c,painewget).getcount()>0)
            {
                zu=(mjpaizu&)yise_(mj,c,painewget);
                for(int i=0;i<zu.getcount();i++)
                    if(Form1->mjots.find(zu.getmj(i))>0)
                        return zu.getmj(i);

                return zu.getmj(0);
            }
            break;
        case dui:
            zu= (mjpaizu&)dui_(mj,c,painewget);
            for(int i=0;i<zu.getcount();i++)
                    if(Form1->mjots.find(zu.getmj(i))>0)
                        return zu.getmj(i);

            return zu.getmj(0);
        case pengpeng:
            zu= (mjpaizu&)pengpeng_(mj,c,painewget);
            for(int i=0;i<zu.getcount();i++)
                    if(Form1->mjots.find(zu.getmj(i))>0)
                        return zu.getmj(i);

            return zu.getmj(0);
        }
    }


    int n[14];
    if(ting(mj,c).getcount()>0&&Form1->difficult>=2)  //已经听牌?  出一张能听最多的
    {                                               //难度2以上
        isting=true;
        int max=ting(mj,c).getcount();

        for(int j=0;j<max;j++)    //除去这么多可以和的牌中已经出完的
            if(Form1->mjots.find(ting(mj,c).getmj(j))==4)
                max--;


        int index=-1,temp;
        for(i=0;i<c;i++)
        {
            temp=mj[i];  mj[i]=painewget;
            n[i]=ting(mj,c).getcount();
            for(int j=0;j<n[i];j++)
                if(Form1->mjots.find(ting(mj,c).getmj(j))==4) //除去这么多可以和的牌中已经出完的
                    n[i]--;

            if(n[i]>max)
                {max=n[i];index=i;}
            mj[i]=temp;
        }

            if(Form1->mjots.getcount()>8*4&&Form1->difficult==3)   //如果到了结尾,注意出牌,没大过的不出
            {
                 int m=max;
                 while(m>0){
                     for(int j=0;j<c;j++)
                        if(m==n[j]&&Form1->mjots.find(mj[j])>0)
                            return mj[j];
                     m--; //减少m  再搜索
                 }
            }
        if(index==-1) return painewget;
        else return mj[index];
    }

    if(Form1->difficult>=2) //难度二以上  //打出不靠的 和能听的
    {
        int temp;
        mjpaizu zu;

        if(bukao(mj,c,painewget)) zu.insertmj(painewget);//先对painewget处理

        for(i=0;i<c;i++)
        {
             temp=mj[i]; mj[i]=painewget;
             if(bukao(mj,c,temp)) zu.insertmj(temp); //有可能要出的,存到zu里
             if(ting(mj,c).getcount()>0)
                {zu.insertmj(temp);isting=true;}
             mj[i]=temp;
        }

        if(Form1->mjots.getcount()>8*4&&Form1->difficult==3)
        {for(i=0;i<zu.getcount();i++)
            if(Form1->mjots.find(zu.getmj(i))>0)
                return zu.getmj(i);
        }
        if(zu.getcount()>0)    //实在出不去 就出第1张
            return zu.getmj(0);
    }

    
   //下面是还未听牌的时候的常规打法
    mj[c++]=painewget;

    int value[14];
    for(i=0;i<c;i++)
        value[i]=0;

    value[0]=value[c-1]=1;
    
    mishel(mj,c);//排序

    for(i=1;i<c-1;i++)
    {
        if(mj[i]==mj[i-1])
            value[i]+=3;
        if(mj[i]==mj[i+1])
            value[i]+=3;

        if(mj[i]-mj[i-1]==1||mj[i-1]-mj[i]==1)
             value[i]+=2;
        if(mj[i]-mj[i+1]==1||mj[i+1]-mj[i]==1)
             value[i]+=2;

        if(i==1||i==c-2)continue;
        if(mj[i]-mj[i+2]==1||mj[i+2]-mj[i]==1||mj[i]-mj[i+2]==2||mj[i+2]-mj[i]==2)
             value[i]+=1;
        if(mj[i]-mj[i-2]==1||mj[i-2]-mj[i]==1||mj[i]-mj[i-2]==2||mj[i-2]-mj[i]==2)
             value[i]+=1;
    }

    int min=100,i_min;
    for(i=0;i<c;i++)
        if(min>value[i])
        {
            min=value[i];
            i_min=i;
        }
    if(Form1->mjots.getcount()>8*4&&Form1->difficult>=2)//打了8*4张以后且难度为2,就得拣熟牌打
        while(min<100)
        {
        for(i=0;i<c;i++)
            if(min==value[i]&&Form1->mjots.find(mj[i])>0)
                return mj[i];
        min++;
        }

    //实在都是生牌,就随便拣一张
    return  mj[i_min];
}
