#include "everyman.h"
#include "unit1.h"

player::player()
{
    setbeginpos(153,467);
}    //修改此处需再修改resetbegin()

bool player::cangang()const
{
    mjpai mj[13];
    int mjs;

    for(int i=0;i<zuhand.getcount();i++)
        mj[i]=zuhand.getmj(i);

    mjs=Form1->mjots.getnewest();
    return (::pengandgang(mj,zuhand.getcount(),mjs)==2)?true:false;
}

bool player::canpeng()const
{

    mjpai mj[13];
    int mjs;

    for(int i=0;i<zuhand.getcount();i++)
        mj[i]=zuhand.getmj(i);

    mjs=Form1->mjots.getnewest();
    return (::pengandgang(mj,zuhand.getcount(),mjs)>=1)?true:false;
}
bool player::canbugang()const
{
    int i,j,s;
    int mja[14];

    i=zushowed.findke(painewget);
    if(i!=100) return true;

    for(i=0;i<zuhand.getcount();i++)
    {
        mja[i]=zuhand.getmj(i);
        if(zushowed.findke(mja[i])!=100)
            return true;
    }

    mja[i]=painewget;

    for(i=0;i<zuhand.getcount()+1;i++)
    {
        s=0;
        for(j=i+1;j<zuhand.getcount()+1;j++)
            if(mja[i]==mja[j]) s++;
        if(s==3)
            break;
    }

    if(i!=zuhand.getcount()+1)
        return true;
    return false;
}
bool player::canting(int index)const
{   //此时 painewget已经有牌,且参数表示用户选择准备出的牌(i=50未为painewget),

//除去这张,判断是否听
    mjpai mj[14];

    int c=zuhand.getcount();
    assert(index<c||index==50);
    int i;
    if(index!=50)
    {
        for(i=0;i<c;i++)
            mj[i]=(mjpai)zuhand.getmj(i);
        mj[index]=painewget;
    }
    else
        for(i=0;i<c;i++)
            mj[i]=zuhand.getmj(i);

    for(i=mjpai::wan1;i<=mjpai::wan9;i++)
        if(::hu(mj,c,i)) return true;

    for(i=mjpai::tiao1;i<=mjpai::tiao9;i++)
        if(::hu(mj,c,i)) return true;

    for(i=mjpai::bing1;i<=mjpai::bing9;i++)
        if(::hu(mj,c,i)) return true;

    if(::hu(mj,c,mjpai::zhong)) return true;
    if(::hu(mj,c,mjpai::bai)) return true;

    return false;

}
bool player::canhu(bool iszimo)const
{
    player tempplayer;

    tempplayer=* const_cast<player*>(this);

    if(!iszimo) //不是自摸,则到桌上取别人刚出的牌
    {
        mjpai mj=Form1->mjots.getnewest();

        tempplayer.painewget=mj;
    }

    if(!tempplayer.hu()) return false;

    if(iszimo) return true;

    if(zushowed.getkuaicount()==0) return true;
    if(tempplayer.fa()||tempplayer.zimingke()||tempplayer.zianke()||
        tempplayer.ziminggang()||tempplayer.ziangang()||tempplayer.minggang()||
        tempplayer.angang())
        return true;

    if(tempplayer.hunyise()||tempplayer.pengpenghu()||tempplayer.qidui()||tempplayer.yitiaolong())
        return true;

    return false;
    //碰了以后若无发、字刻、杠：普通和只能自摸，大和（碰碰和、清一色、混一色、七对）可以炮、摸
}

TRect player::picrect(mjpai mj) const
{
    if(mj>=11&&mj<=19)
        return Rect((mj-11)*25,0,24+(mj-11)*25,34);
    else if(mj>=31&&mj<=39)
        return Rect((mj-31)*25,35,24+(mj-31)*25,69);
    else if(mj>=51&&mj<=59)
        return Rect((mj-51)*25,70,24+(mj-51)*25,104);
    else if(mj==91)
        return Rect(0,105,24,139);
    else if(mj==81)
        return Rect(25,105,49,139);
    else if(mj==71)
        return Rect(50,105,74,139);
    else if(mj==100)
        return Rect(175,105,199,139);


    return Rect(0,0,0,0);
}
void player::draw(int image)const
{
    int showedcount,handcount;

    TCanvas *imming,*iman;

    showedcount=zushowed.getkuaicount();
    handcount=zuhand.getcount();
       switch(image)
        {
        case 1:
            imming=Form1->imdownming1o->Canvas;
            iman=Form1->imdownan1o->Canvas;
            break;
        case 2:
            imming=Form1->imdownming2o->Canvas;
            iman=Form1->imdownan2o->Canvas;
            break;
        }

        int i; int current=beginx,c=0,j;

        for(i=0;i<showedcount;i++)
            if(zushowed.iske(i)||zushowed.isshun(i))
            {
                for(j=0;j<3;j++)
                {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+34),
                        imming, picrect(mj));
                    current+=25;
                }
                current +=5;
            }
            else if(zushowed.isgang(i))
            {
                for(j=0;j<4;j++)
                {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    if(zushowed.isgang(i)==1)//明杠
                        if(j==2)
                            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy-13,current+24,beginy+34-13),
                                imming, picrect(mj));
                        else
                            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+34),
                                imming, picrect(mj));
                    else
                        if(j==2)
                            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy-13,current+24,beginy+34-13),
                                imming, picrect(mj));
                        else
                            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+34),
                                imming, picrect(100));
                    if(j!=1)
                        current+=25;

                }
                current +=5;
            }
            else if(zushowed.whatcolor(i)==5)
            {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+34),
                        imming, picrect(mj));
                    current+=25;

                    current+=5;
            }
        //current +=15;

        for(i=0;i<handcount;i++)
        {
            mjpai mj;
            mj=zuhand.getmj(i);
            Form1->PaintBox1->Canvas->CopyRect(Rect(current,
                beginy-5,current+24,beginy+34-5), iman,
                picrect(mj));
            current+=25;

        }
        current+=5;

        if(painewget!=0)
            Form1->PaintBox1->Canvas->CopyRect(Rect(current,
                beginy-5,current+24,beginy+34-5), iman,
                picrect(painewget));
}


void player::select(int s,int image)
{
    int c=zushowed.getkuaicount();

    int sum=0;int i;
    for(i=0;i<c;i++)
    {
        if(zushowed.iske(i)||zushowed.isshun(i)||zushowed.isgang(i))
            sum+=3*25;
        else
            sum+=25;

        sum+=5;
    }

    int x=beginx+sum;
    int x2=x+zuhand.getcount()*25;


    TImage *iman;
    switch(image)
        {
        case 1:
            iman=Form1->imdownan1o;
            break;
        case 2:
            iman=Form1->imdownan2o;
            break;
    }
    if(selected!=100)
        if(selected==50){
            Form1->PaintBox1->Canvas->FillRect(Rect(x2+5,457,x2+5+24,457+34+5));
            Form1->PaintBox1->Canvas->CopyRect(Rect(x2+5,457+5,x2+5+24,457+34+5),
                iman->Canvas,picrect(painewget));
        }
        else{
            Form1->PaintBox1->Canvas->FillRect(Rect(x+25*selected,457,x+25*selected+24,457+34+5));
            Form1->PaintBox1->Canvas->CopyRect(Rect(x+25*selected,457+5,x+25*selected+24,457+34+5),
                iman->Canvas,picrect(zuhand.getmj(selected)));
        }

    selected=s;

    if(selected==50){
        Form1->PaintBox1->Canvas->FillRect(Rect(x2+5,457,x2+5+24,457+34+5));
        Form1->PaintBox1->Canvas->CopyRect(Rect(x2+5,457,x2+5+24,457+34),
            iman->Canvas,picrect(painewget));
    }
    else{
        Form1->PaintBox1->Canvas->FillRect(Rect(x+25*selected,457,x+25*selected+24,457+34+5));
        Form1->PaintBox1->Canvas->CopyRect(Rect(x+25*selected,457,x+25*selected+24,457+34),
            iman->Canvas,picrect(zuhand.getmj(selected)));
    }

}


void player::chu()
{
    if(selected==50)
        Form1->mjots.insertmjdown(painewget);
    else
    {
        Form1->mjots.insertmjdown(zuhand.takemj(selected));
        zuhand.insertmj(painewget);
        sort();
    }
    painewget=0;

    selected=100;


}

 void player::sort(){

    zuhand.sort();
 }
 void player::sortall(){
    mjpai mja[14];
    int c;
    c=zuhand.getcount();
    for (int i=0;i<c;i++)
        mja[i]=zuhand.getmj(i);
    assert(painewget);
    mja[c]=painewget;

    mishel(mja,c+1);

    zuhand.clear();
    for (int i=0;i<c;i++)
        zuhand.insertmj(mja[i]);

    painewget=mja[c];

    //zuhand.sort();
 }
//------------------------------------------------

TRect computerright::picrect(mjpai mj) const
{
    if(mj>=11&&mj<=19)
        return Rect(0,(19-mj)*25,35,(19-mj)*25+24);
    else if(mj>=31&&mj<=39)
        return Rect(35,(39-mj)*25,70,(39-mj)*25+24);
    else if(mj>=51&&mj<=59)
        return Rect(70,(59-mj)*25,105,(59-mj)*25+24);
    else if(mj==91)
        return Rect(35*3,25*4,35*4,25*5);
    else if(mj==81)
        return Rect(35*3,25*3,35*4,25*4);
    else if(mj==71)
        return Rect(35*3,25*2,35*4,25*3);
    else if(mj==100)
        return Rect(35*3,25*1,35*4,25*2);
    else if(mj==200)
        return Rect(78,110,89,135);
    return Rect(0,0,0,0);
}

void computerright::draw(int image)const
{
    int showedcount,handcount;

    TImage *imming,*iman;

    showedcount=zushowed.getkuaicount();
    handcount=zuhand.getcount();



    switch(image)
    {
        case 1:
            imming=Form1->imright1;
            iman=Form1->imdownan1o;//imleft1;
            break;
        case 2:
            imming=Form1->imright2;
            iman=Form1->imdownan2o;
            break;
    }
        
    int i; int current=beginy,c=0,j;

    for(i=0;i<showedcount;i++)
            if(zushowed.iske(i)||zushowed.isshun(i))
            {
                for(j=0;j<3;j++)
                {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+34,current+24),
                        imming->Canvas, picrect(mj));
                    current-=25;
                }
                current -=5;
            }
            else if(zushowed.isgang(i))
            {
                for(j=0;j<4;j++)
                {
                    //mage *temp=isgang(i)==1?
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    if(zushowed.isgang(i)==1)//明杠
                        if(j==2)
                            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx+13,current,beginx+34+13,
                                current+24),imming->Canvas, picrect(mj));
                        else
                            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+34,
                            current+24),imming->Canvas, picrect(mj));
                    else
                        if(j==2)
                            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx+13,current,beginx+34+13,current+24),
                                imming->Canvas, picrect(100));
                        else
                            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+34,current+24),
                                imming->Canvas, picrect(100));
                    if(j!=1)
                        current-=25;

                }
                current -=5;
            }
            else if(zushowed.whatcolor(i)==5)
            {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+34,current+24),
                        imming->Canvas, picrect(mj));
                    current-=25;

                    current-=5;
            }
        //current +=15;

    for(i=0;i<handcount;i++)
        {
            mjpai mj;
            mj=zuhand.getmj(i);
            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx+24,current,beginx+10+24,current+24),
/*         #ifndef NVIEWMAN
                imming->Canvas ,picrect(mj));
         #else   */
                iman->Canvas,picrect(200));//test
    //     #endif
            current-=25;

        }
    current-=5;

    if(painewget!=0)
        Form1->PaintBox1->Canvas->CopyRect(Rect(beginx+24,current,beginx+10+24,current+24),
/*         #ifndef NVIEWMAN
                imming->Canvas ,picrect(painewget));
         #else   */
                iman->Canvas,picrect(200));//test
    //     #endif


}


//------------------------------------------------

TRect computerup::picrect(mjpai mj) const
{
    if(mj>=11&&mj<=19)
        return Rect((19-mj)*25,35*3,(19-mj)*25+25,35*4);
    else if(mj>=31&&mj<=39)
        return Rect((39-mj)*25,35*2,(39-mj)*25+25,35*3);
    else if(mj>=51&&mj<=59)
        return Rect((59-mj)*25,35*1,(59-mj)*25+25,35*2);
    else if(mj==91)
        return Rect(25*4,0,25*5,35);
    else if(mj==81)
        return Rect(25*3,0,25*4,35);
    else if(mj==71)
        return Rect(25*2,0,25*3,35);
    else if(mj==100)
        return Rect(25*1,0,25*2,35);
    else if(mj==200)
        return Rect(90,110,115,121);
    return Rect(0,0,0,0);    
}

void computerup::draw(int image)const
{
    int showedcount,handcount;

    TCanvas *imming,*iman;

    showedcount=zushowed.getkuaicount();
    handcount=zuhand.getcount();



    switch(image)
    {
        case 1:
            imming=Form1->imup1->Canvas;
            iman=Form1->imdownan1o->Canvas;
            break;
        case 2:
            imming=Form1->imup2->Canvas;
            iman=Form1->imdownan2o->Canvas;
            break;
    }
        
    int i; int current=beginx,c=0,j;

    for(i=0;i<showedcount;i++)
            if(zushowed.iske(i)||zushowed.isshun(i))
            {
                for(j=0;j<3;j++)
                {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+34),
                        imming, picrect(mj));
                    current-=25;
                }
                current -=5;
            }
            else if(zushowed.isgang(i))
            {
                for(j=0;j<4;j++)
                {
                    //mage *temp=isgang(i)==1?
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    if(zushowed.isgang(i)==1)//明杠
                        if(j==2)
                            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy-13,current+24,beginy+34-13),
                                imming, picrect(mj));
                        else
                            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+34),imming, picrect(mj));
                    else
                        if(j==2)
                            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy-13,current+24,beginy+34-13),
                                imming, picrect(100));
                        else
                            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+34),
                                imming, picrect(100));
                    if(j!=1)
                        current-=25;

                }
                current -=5;
            }
            else if(zushowed.whatcolor(i)==5)
            {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+34),
                        imming, picrect(mj));
                    current-=25;

                    current-=5;
            }
        //current +=15;

    for(i=0;i<handcount;i++)
        {
            mjpai mj;
            mj=zuhand.getmj(i);
            Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+10),
/*         #ifndef NVIEWMAN
                imming ,picrect(mj));
         #else  */
                iman,picrect(200));//test
        // #endif
            current-=25;

        }
    current-=5;

    if(painewget!=0)
        Form1->PaintBox1->Canvas->CopyRect(Rect(current,beginy,current+24,beginy+10),
/*         #ifndef NVIEWMAN
                imming ,picrect(painewget));
         #else  */
                iman,picrect(200));//test
        // #endif

  //030325换联众图片
}

//------------------------------------------------

TRect computerleft::picrect(mjpai mj) const
{
    if(mj>=11&&mj<=19)
        return Rect(35*3,25*(mj-11),35*4,25*(mj-11)+24);
    else if(mj>=31&&mj<=39)
        return Rect(35*2,25*(mj-31),35*3,25*(mj-31)+24);
    else if(mj>=51&&mj<=59)
        return Rect(35*1,25*(mj-51),35*2,25*(mj-51)+24);
    else if(mj==91)
        return Rect(0,25*4,35,25*5);
    else if(mj==81)
        return Rect(0,25*5,35,25*6);
    else if(mj==71)
        return Rect(0,25*6,35,25*7);
    else if(mj==100)
        return Rect(0,25*7,35,25*8);
    else if(mj==200)
        return Rect(116,110,127,135);
    return Rect(0,0,0,0);
}

void computerleft::draw(int image)const
{
    int showedcount,handcount;

    TImage *imming,*iman;

    showedcount=zushowed.getkuaicount();
    handcount=zuhand.getcount();



    switch(image)
    {
        case 1:
            imming=Form1->imleft1;
            iman=Form1->imdownan1o;
            break;
        case 2:
            imming=Form1->imleft2;
            iman=Form1->imdownan2o;
            break;
    }
        
    int i; int current=beginy,c=0,j;

    for(i=0;i<showedcount;i++)
            if(zushowed.iske(i)||zushowed.isshun(i))
            {
                for(j=0;j<3;j++)
                {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+34,current+24),
                        imming->Canvas, picrect(mj));
                    current+=25;
                }
                current +=5;
            }
            else if(zushowed.isgang(i))
            {
                for(j=0;j<4;j++)
                {
                    //mage *temp=isgang(i)==1?
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    if(zushowed.isgang(i)==1)//明杠
                        if(j==2)
                            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx-13,current,beginx+34-13,current+24),
                                imming->Canvas, picrect(mj));
                        else
                            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+34,current+24),imming->Canvas, picrect(mj));
                   else
                        if(j==2)
                            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx-13,current,beginx+34-13,current+24),
                                imming->Canvas, picrect(100));
                        else
                            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+34,current+24),
                                imming->Canvas, picrect(100));
                    if(j!=1)
                        current+=25;

                }
                current +=5;
            }
            else if(zushowed.whatcolor(i)==5)
            {
                    mjpai mj;
                    mj=zushowed.getmj(c++);
                    Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+34,current+24),
                        imming->Canvas, picrect(mj));
                    current+=25;

                    current+=5;
            }
        //current +=15;

    for(i=0;i<handcount;i++)
        {
            mjpai mj;
            mj=zuhand.getmj(i);
            Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+10,current+24),
 /*        #ifndef NVIEWMAN
                imming->Canvas ,picrect(mj));
         #else  */
                iman->Canvas,picrect(200));//test
    //     #endif
            current+=25;

        }
    current+=5;

    if(painewget!=0)
        Form1->PaintBox1->Canvas->CopyRect(Rect(beginx,current,beginx+10,current+24),
 /*        #ifndef NVIEWMAN
                imming->Canvas ,picrect(painewget));
         #else  */
                iman->Canvas,picrect(200));//test
    //     #endif

}


//---------------------------
TRect mjontable::picrect(int direct,int color)const
{
    assert(direct>0&&direct<3&& color>0&&color<3);

     if(direct==1&&color==1)
        return Rect(0,0,19,15);
     else if(direct==2&&color==1)
        return Rect(19,0,34,19);
     else if(direct==1&&color==2)
        return Rect(15,19,34,34);
     else if(direct==2&&color==2)
        return Rect(0,15,15,34);

     return Rect(0,0,0,0);
}

void mjontable::reset()    //插入120张牌
{
    int i,j;
    clear();
    for(i=mjpai::wan1;i<=mjpai::wan9;i++)
        for(j=0;j<4;j++)
            mjpaizu::insertmj(i);
    for(i=mjpai::tiao1;i<=mjpai::tiao9;i++)
        for(j=0;j<4;j++)
            mjpaizu::insertmj(i);
    for(i=mjpai::bing1;i<=mjpai::bing9;i++)
        for(j=0;j<4;j++)
            mjpaizu::insertmj(i);
    for(i=mjpai::zhong;i<=mjpai::bai;i+=10)
        for(j=0;j<4;j++)
            mjpaizu::insertmj(i);

    takenfromhead=takenfromtail=0;
}

void mjontable::wash()   //洗牌
{
    assert(mjpaizu::getcount()==120);
    randomize();

    int i;
    int r=random(120);

    for(i=0;i<r;i++)        //先取出头上random张放到尾部
        mjpaizu::insertmj(mjpaizu::takemj(0));
    for(i=0;i<480;i++)
    {
        r=random(120);      //再取第random张放到尾部,做480次
        mjpaizu::insertmj(mjpaizu::takemj(random(2)?r:(119-r)));
    }

}

void mjontable::setfirst(int pos,int num) //设置第一张牌从哪里取
{
    assert(num%2==0);
    firstpos=pos;
    firstpai=num;

}

void mjontable::drawonedirect(int image,int from,int to,int direct)const
{
    assert(from>=0&&to<30&&to>=from);
    int i,head,tail,length,lengthup,begin,beginup;

        head=from/2;
        tail=(30-to-1)/2;

        length=15-head-tail;

        lengthup=(from%2)?(length-1):length;
        lengthup=((to+1)%2)?(lengthup-1):lengthup;

        begin=head;
        beginup=begin+from%2;

    switch(direct)
    {
    case down:
        for(i=0;i<length;i++)
            Form1->PaintBox1->Canvas->CopyRect(Rect(bxdown-(i+begin)*15,bydown,bxdown-(i+begin)*15+15,bydown+19),
                Form1->imgai->Canvas, picrect(2,image));
        for(i=0;i<lengthup;i++)
            Form1->PaintBox1->Canvas->CopyRect(Rect(bxdown-(i+beginup)*15+2,bydown-2,bxdown-(i+beginup)*15+15+2,bydown+19-2),
                Form1->imgai->Canvas, picrect(2,image));
        break;
    case up:
        for(i=0;i<length;i++)
            Form1->PaintBox1->Canvas->CopyRect(Rect(bxup+(i+begin)*15,byup,bxup+(i+begin)*15+15,byup+19),
                Form1->imgai->Canvas, picrect(2,image));
        for(i=0;i<lengthup;i++)
            Form1->PaintBox1->Canvas->CopyRect(Rect(bxup+(i+beginup)*15+2,byup-2,bxup+(i+beginup)*15+15+2,byup+19-2),
                Form1->imgai->Canvas, picrect(2,image));
        break;
    case left:
        for(i=0;i<length;i++)
            Form1->PaintBox1->Canvas->CopyRect(Rect(bxleft,byleft-(i+begin)*15,bxleft+19,byleft-(i+begin)*15+15),
                Form1->imgai->Canvas, picrect(1,image));
        for(i=0;i<lengthup;i++)
            Form1->PaintBox1->Canvas->CopyRect(Rect(bxleft+2,byleft-(i+beginup)*15-2,bxleft+19+2,byleft-(i+beginup)*15+15-2),
                Form1->imgai->Canvas, picrect(1,image));
        break;
    case right:
        for(i=0;i<length;i++)
            Form1->PaintBox1->Canvas->CopyRect(Rect(bxright,byright+(i+begin)*15,bxright+19,byright+(i+begin)*15+15),
                Form1->imgai->Canvas, picrect(1,image));
        for(i=0;i<lengthup;i++)
            Form1->PaintBox1->Canvas->CopyRect(Rect(bxright+2,byright+(i+beginup)*15-2,bxright+19+2,byright+(i+beginup)*15+15-2),
                Form1->imgai->Canvas, picrect(1,image));
        break;
    }

}
void mjontable::draw(int image)const
{
    //int count=getcount();

    assert(takenfromhead+takenfromtail<=120);
    int i,j,th,fp,tt;
    int taken[4][2];

    for(i=0;i<4;i++)
        for(j=0;j<2;j++)
            taken[i][j]=0;


    th=takenfromhead;

    th-=30-firstpai;

    fp=(firstpos==3)?0:(firstpos+1);

    while(th>=30)
    {
        th-=30;
        taken[fp][0]=taken[fp][1]=30;
        fp=(fp==3)?0:(fp+1);
    }

     if(th>0)taken[fp][0]=th;
     //-------
     tt=takenfromtail;

     tt-=firstpai;

     fp=(firstpos==0)?3:(firstpos-1);

     while(tt>=30)
    {
        tt-=30;
        taken[fp][0]=taken[fp][1]=30;
        fp=(fp==0)?3:(fp-1);
    }

    if(tt>0)taken[fp][1]=tt;
    //-------以上 得到除了第一方外的3方的麻将堆的被取情况,[0]为顺时针方向取的
    fp=(firstpos==3)?0:(firstpos+1);
    for(i=0;i<3;i++)
    {
        if(taken[fp][0]==30&&taken[fp][1]==30);
        else
            if(taken[fp][0]<=29-taken[fp][1])
                drawonedirect(image,taken[fp][0],29-taken[fp][1],fp);
        fp=(fp==3)?0:(fp+1);
    }  //由前面得到的taken[][]来画3方牌

    //下面画初始取牌的一方
    if(takenfromtail==0&&takenfromhead==0)
    {
        drawonedirect(image,0,29,firstpos);
        return;
    }
    if(takenfromtail<firstpai)
        if(taken[firstpos][0]<=firstpai-takenfromtail-1)
            drawonedirect(image,taken[firstpos][0],firstpai-takenfromtail-1,firstpos);

    if(takenfromhead<30-firstpai)
        if(firstpai+takenfromhead<=29-taken[firstpos][1])
            drawonedirect(image,firstpai+takenfromhead,29-taken[firstpos][1],firstpos);

}

int player::getindex(int mousex,int mousey)const
{
    int c=zushowed.getkuaicount();

    int s=0;int i;
    for(i=0;i<c;i++)
    {
        if(zushowed.iske(i)||zushowed.isshun(i)||zushowed.isgang(i))
            s+=3*25;
        else
            s+=25;

        s+=5;
    }

    int x=beginx+s;

    int x2=x+zuhand.getcount()*25;
    if(mousex<x2&&mousex>=x&&mousey<=497&&mousey>=462)
        return (mousex-x)/25;

    if(mousex<=x2+24+5&&mousex>=x2+5&&mousey<=497&&mousey>=462)
        return  50;

    if(mousex<564&&mousex>=500&&mousey<=538&&mousey>=520)
        return (mousex-500)/16+101;

    return 100;
}
void player::resetbegin()
{
    int c=zushowed.getkuaicount();

    int s=0;int i;
    for(i=0;i<c;i++)
    {
        if(zushowed.iske(i)||zushowed.isshun(i)||zushowed.isgang(i))
            s+=3*25;
        else
            s+=25;

        s+=5;
    }

    s+=zuhand.getcount()*25;

    beginx=153+(495-30-s)/2;
}

void computerright::resetbegin()
{
    int c=zushowed.getkuaicount();

    int s=0;int i;
    for(i=0;i<c;i++)
    {
        if(zushowed.iske(i)||zushowed.isshun(i)||zushowed.isgang(i))
            s+=3*25;
        else
            s+=25;

        s+=5;
    }

    s+=zuhand.getcount()*25;

    beginy=507-(495-30-s)/2;

}
void computerleft::resetbegin()
{
    int c=zushowed.getkuaicount();

    int s=0;int i;
    for(i=0;i<c;i++)
    {
        if(zushowed.iske(i)||zushowed.isshun(i)||zushowed.isgang(i))
            s+=3*25;
        else
            s+=25;

        s+=5;
    }

    s+=zuhand.getcount()*25;

    beginy=33+(495-30-s)/2;

}

void computerup::resetbegin()
{
    int c=zushowed.getkuaicount();

    int s=0;int i;
    for(i=0;i<c;i++)
    {
        if(zushowed.iske(i)||zushowed.isshun(i)||zushowed.isgang(i))
            s+=3*25;
        else
            s+=25;

        s+=5;
    }

    s+=zuhand.getcount()*25;

    beginx=602-(495-30-s)/2;

}

mjpai mjontable::takeone(bool fromhead) //取一张,变量代表从头还是尾
{
    if(fromhead)
    {
        takenfromhead++;
        return takemj(0);
    }
    else
    {
        takenfromtail++;
        return takemj(mjpaizu::getcount()-1);
    }
}

//---------------------------------------

void mjontableshowed::clear()
{
    int i,j;

    for(i=0;i<10;i++)
        for(j=0;j<8;j++)
        {
            posmj1[i][j]=char(0);
            if(i<2) newestmj[i]=100;
            if(i<8&&j<5)
                  posmj2[i][j]=char(0);
        }

    flag_firstlayerused=false;

    showedinhand.clear();

    count=0;
}
mjontableshowed::mjontableshowed():showedinhand(80)//初始化vector的capacity为80张
{
    clear();
}
void mjontableshowed::insertmjup(mjpai mj)
{
    assert(count<120);

    sound(Form1,mj);

    int i,j;

    if(!flag_firstlayerused)
        for(j=0;j<8;j++)
            for(i=9;i>=0;i--)
                if(posmj1[i][j]==char(0))
                {
                     posmj1[i][j]=char(mj);
                     count++;
                     newestmj[0]=i;
                     newestmj[1]=j;
                     return;
                }
     assert(0);//现行120张麻将不可能执行到这
     flag_firstlayerused=true;

     for(j=0;j<5;j++)
        for(i=7;i>=0;i--)
            if(posmj2[i][j]==char(0))
            {
                posmj2[i][j]=char(mj);
                count++;
                newestmj[0]=i;
                newestmj[1]=j;
                return;
            }

}
void mjontableshowed::sound(TForm1 *Form1,mjpai mj)const
{
    if(!Form1->m_sound)return;

        if (mj.iswan())
         {
            Form1->sound->FileName="wav\\wfn0"+AnsiString(mj-10)+".wav";
         }
         if(mj.istiao())
         {
            Form1->sound->FileName="wav\\wfn1"+AnsiString(mj-30)+".wav";
         }
         if(mj.isbing())
         {
            Form1->sound->FileName="wav\\wfn2"+AnsiString(mj-50)+".wav";
         }
         if(mj.iszi())
         {
            Form1->sound->FileName="wav\\wfn3"+AnsiString((mj-71)/10+5)+".wav";
         }
         Form1->sound->Open();
         Form1->sound->Play();
}
void mjontableshowed::insertmjdown(mjpai mj)
{
    assert(count<120);

    sound(Form1,mj);

    int i,j;

    if(!flag_firstlayerused)
        for(j=7;j>=0;j--)
            for(i=0;i<10;i++)
                if(posmj1[i][j]==char(0))
                {
                     posmj1[i][j]=char(mj);
                     count++;
                     newestmj[0]=i;
                     newestmj[1]=j;
                     return;
                }

     assert(0);//现行120张麻将不可能执行到这
     flag_firstlayerused=true;

     for(j=4;j>=0;j--)
        for(i=0;i<8;i++)
            if(posmj2[i][j]==char(0))
            {
                posmj2[i][j]=char(mj);
                count++;
                newestmj[0]=i;
                newestmj[1]=j;
                return;
            }

}

void mjontableshowed::insertmjleft(mjpai mj)
{
    assert(count<120);

    sound(Form1,mj);

    int i,j;

    if(!flag_firstlayerused)
        for(i=0;i<10;i++)
            for(j=0;j<8;j++)
                if(posmj1[i][j]==char(0))
                {
                     posmj1[i][j]=char(mj);
                     count++;
                     newestmj[0]=i;
                     newestmj[1]=j;
                     return;
                }

     assert(0);//现行120张麻将不可能执行到这
     flag_firstlayerused=true;

     for(i=0;i<8;i++)
        for(j=0;j<5;j++)
            if(posmj2[i][j]==char(0))
            {
                posmj2[i][j]=char(mj);
                count++;
                newestmj[0]=i;
                newestmj[1]=j;
                return;
            }

}

void mjontableshowed::insertmjright(mjpai mj)
{
    assert(count<120);

    sound(Form1,mj);

    int i,j;

    if(!flag_firstlayerused)
        for(i=9;i>=0;i--)
            for(j=7;j>=0;j--)
                if(posmj1[i][j]==char(0))
                {
                     posmj1[i][j]=char(mj);
                     count++;
                     newestmj[0]=i;
                     newestmj[1]=j;
                     return;
                }

     assert(0);//现行120张麻将不可能执行到这
     flag_firstlayerused=true;

     for(i=7;i>=0;i--)
         for(j=4;j>=0;j--)
            if(posmj2[i][j]==char(0))
            {
                posmj2[i][j]=char(mj);
                count++;
                newestmj[0]=i;
                newestmj[1]=j;
                return;
            }

}

void mjontableshowed::insertshowedinhand(mjpai mj)
{
     showedinhand.insertmj(mj);
}

int mjontableshowed::find(mjpai mj)const
{
    int c=0;

    int cc=showedinhand.getcount( );

    for(int i=0;i<cc;i++)
        if(showedinhand.getmj(i)==mj)
            c++;



    if(flag_firstlayerused)
    {
        for(int i=0;i<10;i++)
            for(int j=0;j<8;j++)
            {
                if(posmj1[i][j]==char(mj))
                    c++;
                if(i<8&&j<5)
                    if(posmj2[i][j]==char(mj))
                        c++;
            }
    }
    else
    {
        for(int i=0;i<10;i++)
            for(int j=0;j<8;j++)
                if(posmj1[i][j]==char(mj))
                    c++;
    }

    assert(c<=4);

    return c;
}

TRect mjontableshowed::picrect(mjpai mj) const
{      
    if(mj>=11&&mj<=19)
        return Rect((mj-11)*25,0,25+(mj-11)*25,35);
    else if(mj>=31&&mj<=39)
        return Rect((mj-31)*25,35,25+(mj-31)*25,70);
    else if(mj>=51&&mj<=59)
        return Rect((mj-51)*25,70,25+(mj-51)*25,105);
    else if(mj==91)
        return Rect(0,105,24,139);
    else if(mj==81)
        return Rect(25,105,49,139);
    else if(mj==71)
        return Rect(50,105,74,139);
    else if(mj==100)
        return Rect(175,105,200,140);


    return Rect(0,0,0,0);
}
void mjontableshowed::draw(int image)const
{
    if(count==0)return;
    
    int i,j;
    //player *pp;

    TImage *im=image==1?Form1->imdownming1o:Form1->imdownming2o;

    for(i=0;i<10;i++)
        for(j=0;j<8;j++)
            if(posmj1[i][j]!=char(0))
                 Form1->PaintBox1->Canvas->CopyRect(Rect(278+i*25,120+j*35,278+i*25+24,120+j*35+34),
                    im->Canvas, picrect(mjpai(posmj1[i][j])));

    if(flag_firstlayerused) {

        for(i=0;i<8;i++)
          for(j=0;j<5;j++)
            if(posmj2[i][j]!=char(0))
                 Form1->PaintBox1->Canvas->CopyRect(Rect(278+25+3+i*25,120+70-16+j*35,278+25+3+i*25+24,120+70-16+j*35+34),
                    im->Canvas, picrect(mjpai(posmj1[i][j])));
     }

    //画最新一张边框
    if(newestmj[0]==100||newestmj[1]==100)return;
    
    if(!flag_firstlayerused){
        Form1->PaintBox1->Canvas->CopyRect(Rect(278+newestmj[0]*25,120+newestmj[1]*35,278+newestmj[0]*25+2,120+newestmj[1]*35+34),
            Form1->imbiankuang->Canvas, Rect(0,0,2,35));
        Form1->PaintBox1->Canvas->CopyRect(Rect(278+newestmj[0]*25,120+newestmj[1]*35,278+newestmj[0]*25+24,120+newestmj[1]*35+2),
            Form1->imbiankuang->Canvas, Rect(0,0,25,2));
        Form1->PaintBox1->Canvas->CopyRect(Rect(278+newestmj[0]*25+23,120+newestmj[1]*35,278+newestmj[0]*25+24,120+newestmj[1]*35+34),
            Form1->imbiankuang->Canvas, Rect(23,0,25,35));
        Form1->PaintBox1->Canvas->CopyRect(Rect(278+newestmj[0]*25,120+newestmj[1]*35+33,278+newestmj[0]*25+24,120+newestmj[1]*35+34),
            Form1->imbiankuang->Canvas, Rect(0,33,25,35));
    }
    else {
        Form1->PaintBox1->Canvas->CopyRect(Rect(278+25+3+newestmj[0]*25,120+70-16+newestmj[1]*35,278+25+3+newestmj[0]*25+2,120+70-16+newestmj[1]*35+34),
            Form1->imbiankuang->Canvas, Rect(0,0,2,35));
        Form1->PaintBox1->Canvas->CopyRect(Rect(278+25+3+newestmj[0]*25,120+70-16+newestmj[1]*35,278+25+3+newestmj[0]*25+24,120+70-16+newestmj[1]*35+2),
            Form1->imbiankuang->Canvas, Rect(0,0,25,2));
        Form1->PaintBox1->Canvas->CopyRect(Rect(278+25+3+newestmj[0]*25+23,120+70-16+newestmj[1]*35,278+25+3+newestmj[0]*25+24,120+70-16+newestmj[1]*35+34),
            Form1->imbiankuang->Canvas, Rect(23,0,25,35));
        Form1->PaintBox1->Canvas->CopyRect(Rect(278+25+3+newestmj[0]*25,120+70-16+newestmj[1]*35+33,278+25+3+newestmj[0]*25+24,120+70-16+newestmj[1]*35+34),
            Form1->imbiankuang->Canvas, Rect(0,33,25,35));
    }


}

mjpai mjontableshowed::takenewest()
{
    assert(newestmj[0]!=100&&newestmj[1]!=100);
     int mj;
     if(!flag_firstlayerused){
         mj=(int)posmj1[newestmj[0]][newestmj[1]];
         posmj1[newestmj[0]][newestmj[1]]=char(0);
     }
     else{
         mj=(int)posmj2[newestmj[0]][newestmj[1]];
         posmj2[newestmj[0]][newestmj[1]]=char(0);
     }
     newestmj[0]=newestmj[1]=100;


     count--;
     if(count<80)
        flag_firstlayerused=false;



     assert(mj>=mjpai::wan1&&mj<=mjpai::bai);
     return mj;
}

mjpai mjontableshowed::getnewest()const
{
    assert(newestmj[0]!=100&&newestmj[1]!=100);

     if(!flag_firstlayerused){
         return (int)posmj1[newestmj[0]][newestmj[1]];
     }
     else{
         return (int)posmj2[newestmj[0]][newestmj[1]];
     }
}








