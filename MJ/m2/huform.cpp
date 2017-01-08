//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "huform.h"
#include "unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Thupai *hupaiform;
//---------------------------------------------------------------------------
__fastcall Thupai::Thupai(TComponent* Owner,man &dow,man &le,man &u,man &ri,bool gskh,bool qianggang,bool zimo,
        int w,int l,int mst,AnsiString *n,int *ps,int im):TForm(Owner),up(u),left(le),right(ri),down(dow)
{
    qiangganghu=qianggang;
    gangshangkaihua=gskh;
    iszimo=zimo;
    image=im;
    win=w; lose=l; master=mst;
    playername=n;
    paishu=ps;
    /*for(int i=0;i<4;i++)
        playername[i]=n[i];
    for(int i=0;i<25;i++)
        paishu[i]=ps[i];  */
}


void Thupai::calculate(int * ret)
{
             /*
                10,15, //一条龙 0
                10,15, //清一色 2
                5,10,  //混一色 （凑一色） 4
                10,15, //七对   6
                10,15, //碰碰和   8
                4,7,   //红旗飘飘  10
                10,    //杠上开花   12
                1,     //炮         13
                2,     //自摸       14
                2,     //抢杠和      15
                1,     //发          16
                1,     //字明刻      17
                2,     //字暗刻      18
                3,     //字明杠      19
                4,     //字暗杠       20
                1,     //明杠         21
                2,     //暗杠         22
                1,     //独听(砍张、边张、吊将) 23
                1};    //门清        24             */


    switch(win)
    {
    case 0:
        winner=&down;
        break;
    case 1:
        winner=&left;
        break;
    case 2:
        winner=&up;
        break;
    case 3:
        winner=&right;
        break;
    }
    winner->sort();//为了画图

    AnsiString tempstring;
    tempstring="";

    tempstring=(win==master)?AnsiString("(庄家)"):AnsiString("");
    tempstring= AnsiString( playername[win] )+ tempstring;
    if(iszimo)
        Label3->Caption=tempstring+AnsiString("  自摸\n");
    else
    {
         tempstring=tempstring+ "  和牌  ";
         tempstring=tempstring+playername[lose];
         tempstring=tempstring+(lose==master?AnsiString("(庄家)"):AnsiString(""));
         tempstring=tempstring+"  放炮\n";
        Label3->Caption=tempstring;
        assert(lose!=win);
    }

    Label1->Caption="牌种\n\n";
    Label2->Caption="牌数\n\n";

    int sum_sc=0;

    bool zimodone=false; //自摸,门清,独听,只算一次
    bool menqingdone=false;
    bool dutingdone=false;

    if(!iszimo)
    {
        Label1->Caption=Label1->Caption+"放炮 "+"\n";
        Label2->Caption=Label2->Caption+paishu[13]+"\n";
        sum_sc+=paishu[13];
    }
    if(winner->qingyise())
    {   int sc=paishu[2];
        if(iszimo&&!zimodone)
        {
            Label1->Caption=Label1->Caption+"自摸 ";
            zimodone=true;
            sc+=5;
        }
        if(winner->menqing()&&!menqingdone)
        {
            Label1->Caption=Label1->Caption+"门清 ";
            menqingdone=true;
            sc+=5;
        }
        if(winner->duting()&&!dutingdone)
        {
            Label1->Caption=Label1->Caption+"独听 ";
            dutingdone=true;
            sc+=5;
        }
        Label1->Caption=Label1->Caption+"清一色 "+"\n";
        Label2->Caption=Label2->Caption+sc+"\n";
        sum_sc+=sc;
    }

    else if(winner->hunyise())
    {   int sc=paishu[4];
        if(iszimo&&!zimodone)
        {
            Label1->Caption=Label1->Caption+"自摸 ";
            zimodone=true;
            sc+=5;
        }
        if(winner->menqing()&&!menqingdone)
        {
            Label1->Caption=Label1->Caption+"门清 ";
            menqingdone=true;
            sc+=5;
        }
        if(winner->duting()&&!dutingdone)
        {
            Label1->Caption=Label1->Caption+"独听 ";
            dutingdone=true;
            sc+=5;
        }
        Label1->Caption=Label1->Caption+"混一色 "+"\n";
        Label2->Caption=Label2->Caption+sc+"\n";
        sum_sc+=sc;
    }


    
    if(winner->qidui())
    {   int sc=paishu[6];
        int xs=winner->qidui();
        if(iszimo&&!zimodone)
        {
            Label1->Caption=Label1->Caption+"自摸 ";
            zimodone=true;
            sc+=5;
        }
        if(getwei(xs,3)==1)
        {
            Label1->Caption=Label1->Caption+"潇洒 ";
            sc+=5;
        }
        else if(getwei(xs,3)==2)
        {
            Label1->Caption=Label1->Caption+"双潇洒 ";
            sc+=10;
        }
        if(getwei(xs,2)==1)
        {
            Label1->Caption=Label1->Caption+"豪华 ";
            sc+=5;
        }
        else if(getwei(xs,2)==1)
        {
            Label1->Caption=Label1->Caption+"双豪华 ";
            sc+=10;
        }
        Label1->Caption=Label1->Caption+"七对 "+"\n";
        Label2->Caption=Label2->Caption+sc+"\n";
        sum_sc+=sc;
    }



    if(winner->pengpenghu())
    {   int sc=paishu[8];
        int xs=winner->pengpenghu();
        if(iszimo&&!zimodone)
        {
            Label1->Caption=Label1->Caption+"自摸 ";
            zimodone=true;
            sc+=5;
        }
        if(winner->menqing()&&!menqingdone)
        {
            Label1->Caption=Label1->Caption+"门清 ";
            menqingdone=true;
            sc+=5;
        }
        if(winner->duting()&&!dutingdone)
        {
            Label1->Caption=Label1->Caption+"独听 ";
            dutingdone=true;
            sc+=5;
        }
        if(xs==2)
        {
            Label1->Caption=Label1->Caption+"潇洒 ";
            sc+=5;
        }
        Label1->Caption=Label1->Caption+"碰碰和 "+"\n";
        Label2->Caption=Label2->Caption+sc+"\n";
        sum_sc+=sc;
    }

    if(winner->yitiaolong())
    {   int sc=paishu[0];
        //int xs=winner->pengpenghu();
        if(iszimo&&!zimodone)
        {
            Label1->Caption=Label1->Caption+"自摸 ";
            zimodone=true;
            sc+=5;
        }
        if(winner->menqing()&&!menqingdone)
        {
            Label1->Caption=Label1->Caption+"门清 ";
            menqingdone=true;
            sc+=5;
        }
        if(winner->duting()&&!dutingdone)
        {
            Label1->Caption=Label1->Caption+"独听 ";
            dutingdone=true;
            sc+=5;
        }
        Label1->Caption=Label1->Caption+"一条龙 "+"\n";
        Label2->Caption=Label2->Caption+sc+"\n";
        sum_sc+=sc;
    }


    if(winner->hongqipiaopiao())
    {   int ma=winner->hongqipiaopiao();
        int sc;
        if(ma==1)
        {
            sc=paishu[10];
            Label1->Caption=Label1->Caption+"明 ";
        }
        else
        {
            sc=paishu[11];
            Label1->Caption=Label1->Caption+"暗 ";
        }
        Label1->Caption=Label1->Caption+"红旗飘飘 "+"\n";
        Label2->Caption=Label2->Caption+sc+"\n";
        sum_sc+=sc;
    }

    if(winner->menqing()&&!menqingdone)
    {
        Label1->Caption=Label1->Caption+"门清 "+"\n";
        Label2->Caption=Label2->Caption+paishu[24]+"\n";
        sum_sc+=paishu[24];
    }
    if(iszimo&&!zimodone)
    {
        Label1->Caption=Label1->Caption+"自摸 "+"\n";
        Label2->Caption=Label2->Caption+paishu[14]+"\n";
        sum_sc+=paishu[14];
    }
    if(winner->duting()&&!dutingdone)
    {
        Label1->Caption=Label1->Caption+"独听 "+"\n";
        Label2->Caption=Label2->Caption+paishu[23]+"\n";
        sum_sc+=paishu[23];
    }

    if(winner->fa())
    {   int n=winner->fa();
        Label1->Caption=Label1->Caption+n+"发财 "+"\n";
        Label2->Caption=Label2->Caption+paishu[16]*n+"\n";
        sum_sc+=paishu[16]*n;
    }
    if(winner->zimingke()&&!winner->hongqipiaopiao())
    {   int n=winner->zimingke();
        Label1->Caption=Label1->Caption+n+"字明刻 "+"\n";
        Label2->Caption=Label2->Caption+paishu[17]*n+"\n";
        sum_sc+=paishu[17]*n;
    }
    if(winner->zianke()&&!winner->hongqipiaopiao())
    {   int n=winner->zianke();
        Label1->Caption=Label1->Caption+n+"字暗刻 "+"\n";
        Label2->Caption=Label2->Caption+paishu[18]*n+"\n";
        sum_sc+=paishu[18]*n;
    }
    if(winner->ziminggang())
    {   int n=winner->ziminggang();
        Label1->Caption=Label1->Caption+n+"字明杠 "+"\n";
        Label2->Caption=Label2->Caption+paishu[19]*n+"\n";
        sum_sc+=paishu[19]*n;
    }
    if(winner->ziangang())
    {   int n=winner->ziangang();
        Label1->Caption=Label1->Caption+n+"字暗杠 "+"\n";
        Label2->Caption=Label2->Caption+paishu[20]*n+"\n";
        sum_sc+=paishu[20]*n;
    }
    if(winner->minggang())
    {   int n=winner->minggang();
        Label1->Caption=Label1->Caption+n+"明杠 "+"\n";
        Label2->Caption=Label2->Caption+paishu[21]*n+"\n";
        sum_sc+=paishu[21]*n;
    }
    if(winner->angang())
    {   int n=winner->angang();
        Label1->Caption=Label1->Caption+n+"暗杠 "+"\n";
        Label2->Caption=Label2->Caption+paishu[22]*n+"\n";
        sum_sc+=paishu[22]*n;
    }
    if(qiangganghu)
    {
        Label1->Caption=Label1->Caption+"抢杠和 "+"\n";
        Label2->Caption=Label2->Caption+paishu[15]+"\n";
        sum_sc+=paishu[15];
    }
    if(gangshangkaihua)
    {
        Label1->Caption=Label1->Caption+"杠上开花 "+"\n";
        Label2->Caption=Label2->Caption+paishu[12]+"\n";
        sum_sc+=paishu[12];
    }

    Label1->Caption=Label1->Caption+"\n\n总牌数       "+sum_sc+"\n";

    int pai[4];
    man * pm;
    Label4->Caption="";

    for(int i=0;i<4;i++)
    {
        pai[i]=0;
        if(i==win)continue;

        switch(i)
        {
        case 0:
            pm=&down;
            break;
        case 1:
            pm=&left;
            break;
        case 2:
            pm=&up;
            break;
        case 3:
            pm=&right;
            break;
        }

        Label4->Caption=Label4->Caption+playername[i]+(i==master?AnsiString("(庄家)"):AnsiString(""))+"  ";
        if(pm->fa())
        {   int n=pm->fa();
            Label4->Caption=Label4->Caption+n+"发财 ";
            Label4->Caption=Label4->Caption+paishu[16]*n+" ";
            pai[i]+=paishu[16]*n;
        }
        if(pm->zimingke())
        {   int n=pm->zimingke();
            Label4->Caption=Label4->Caption+n+"字明刻 ";
            Label4->Caption=Label4->Caption+paishu[17]*n+" ";
            pai[i]+=paishu[17]*n;
        }
        if(pm->ziminggang())
        {   int n=pm->ziminggang();
            Label4->Caption=Label4->Caption+n+"字明杠 ";
            Label4->Caption=Label4->Caption+paishu[19]*n+" ";
            pai[i]+=paishu[19]*n;
        }
        if(pm->ziangang())
        {   int n=pm->ziangang();
            Label4->Caption=Label4->Caption+n+"字暗杠 ";
            Label4->Caption=Label4->Caption+paishu[20]*n+" ";
            pai[i]+=paishu[20]*n;
        }
        if(pm->minggang())
        {   int n=pm->minggang();
            Label4->Caption=Label4->Caption+n+"明杠 ";
            Label4->Caption=Label4->Caption+paishu[21]*n+" ";
            pai[i]+=paishu[21]*n;
        }
        if(pm->angang())
        {   int n=pm->angang();
            Label4->Caption=Label4->Caption+n+"暗杠 ";
            Label4->Caption=Label4->Caption+paishu[22]*n+" ";
            pai[i]+=paishu[22]*n;
        }

        Label4->Caption=Label4->Caption+"总 "+pai[i]+"\n";
    }

    Label5->Caption="";
    for(int i=0;i<4;i++)
    {   ret[i]=0;
        if(i==win)continue;


        if(iszimo)
        {
            if(win==master||i==master)
                ret[i]=(sum_sc+pai[i])*(-2);
            else
                ret[i]=(sum_sc+pai[i])*(-1);
        }
        else
        {
            if(i==lose&&(i==master||win==master))
                ret[i]=(sum_sc+pai[i])*(-2);
            else if(i==lose)
                ret[i]=(sum_sc+pai[i])*(-1);
        }
        Label5->Caption=Label5->Caption+ret[i]+"\n";

        //ret[win]+=ret[i];
    }
    for(int i=0;i<4;i++)
        if(i!=win)
            ret[win]+=ret[i];
    ret[win]=0-ret[win];
    Label4->Caption=Label4->Caption+playername[win]+(win==master?AnsiString("(庄家)"):AnsiString(""))+"\n";
    Label5->Caption=Label5->Caption+ret[win]+"\n";

    this->Resize();
}

void __fastcall Thupai::Button1Click(TObject *Sender)
{
    Close();    
}
//---------------------------------------------------------------------------


void __fastcall Thupai::FormResize(TObject *Sender)
{
    //Label2->Left=Label1->Left+Label1->Width+30;

    ClientWidth=555;

    //Label3->Left=(ClientWidth-Label1->Width)/2;

    Label4->Top=Label1->Height+Label1->Top+20;
    Label5->Top=Label4->Top;

    ClientHeight=Label4->Height+Label4->Top+70;

    Label1->Left=(ClientWidth-Label1->Width-Label2->Width)/3;

    Label2->Left=ClientWidth-Label1->Left-Label2->Width;
    
    Button1->Top=ClientHeight-50;

    Button1->Left=(ClientWidth-Button1->Width)/2;

    Label3->Left=(ClientWidth-Label3->Width)/2;

    Label4->Left=(ClientWidth-Label4->Width-Label5->Width)/3;
    Label5->Left=ClientWidth-Label4->Left-Label5->Width;
}
//---------------------------------------------------------------------------
static TRect picrect(mjpai mj)
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
void Thupai::drawwinner()const
{


    int showedcount,handcount;

    TImage *imming,*iman;

    showedcount=winner->zushowed.getkuaicount();
    handcount=winner->zuhand.getcount();



        switch(image)
        {
        case 1:
            imming=Form1->imdownming1o;
            iman=Form1->imdownan1o;
            break;
        case 2:
            imming=Form1->imdownming2o;
            iman=Form1->imdownan2o;
            break;
        }

        int i; int current=10,c=0,j;

        for(i=0;i<showedcount;i++)
            if(winner->zushowed.iske(i)||winner->zushowed.isshun(i))
            {
                for(j=0;j<3;j++)
                {
                    mjpai mj;
                    mj=winner->zushowed.getmj(c++);
                    Canvas->CopyRect(Rect(current,15,current+25,15+35),
                        imming->Canvas, picrect(mj));
                    current+=25;
                }
                current +=5;
            }
            else if(winner->zushowed.isgang(i))
            {
                for(j=0;j<4;j++)
                {
                    //mage *temp=isgang(i)==1?
                    mjpai mj;
                    mj=winner->zushowed.getmj(c++);
                    if(winner->zushowed.isgang(i)==1)//明杠
                        if(j==2)
                            Canvas->CopyRect(Rect(current,15-13,current+25,15+35-13),
                                imming->Canvas, picrect(mj));
                        else
                            Canvas->CopyRect(Rect(current,15,current+25,15+35),
                                imming->Canvas, picrect(mj));
                    else
                        if(j==2)
                            Canvas->CopyRect(Rect(current,15-13,current+25,15+35-13),
                                imming->Canvas, picrect(mj));
                        else
                            Canvas->CopyRect(Rect(current,15,current+25,15+35),
                                imming->Canvas, picrect(100));
                    if(j!=1)
                        current+=25;

                }
                current +=5;
            }
            else if(winner->zushowed.whatcolor(i)==5)
            {
                    mjpai mj;
                    mj=winner->zushowed.getmj(c++);
                    Canvas->CopyRect(Rect(current,15,current+25,15+35),
                        imming->Canvas, picrect(mj));
                    current+=25;

                    current+=5;
            }
        //current +=15;
        current=10;
        for(i=0;i<handcount;i++)
        {
            mjpai mj;
            mj=winner->zuhand.getmj(i);
            Canvas->CopyRect(Rect(current,
                60-5,current+25,60+35-5), iman->Canvas,
                picrect(mj));
            current+=25;

        }
        current+=5;

        if(winner->painewget!=0)
            Canvas->CopyRect(Rect(current,
                60-5,current+25,60+35-5), iman->Canvas,
                picrect(winner->painewget));

}
void __fastcall Thupai::FormPaint(TObject *Sender)
{
    drawwinner();
}
//---------------------------------------------------------------------------


