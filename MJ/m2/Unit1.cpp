//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
#include "colorform.h"
#include "setpaishuform.h"
#include "speedform.h"
#include "saveunit.h"
#include "diffunit.h"
#include "nameform.h"
#include "aboutmj.h"
#include <Registry.hpp>
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
    name[0]="偶自己";
    name[1]="张伯姿";
    name[2]="梁永琪";
    name[3]="赵  薇";//姓名 0下 1左 2上 3右

                paishu[0]=10;  paishu[1]=15; //一条龙 0
                paishu[2]=10;paishu[3]=15; //清一色 2
                paishu[4]=5;paishu[5]=10;  //混一色 （凑一色） 4
                paishu[6]=10;paishu[7]=15; //七对   6
                paishu[8]=10;paishu[9]=15; //碰碰和   8
                paishu[10]=4;paishu[11]=7;   //红旗飘飘  10
                paishu[12]=10;    //杠上开花   12
                paishu[13]=1;     //炮         13
                paishu[14]=2;     //自摸       14
                paishu[15]=2;     //抢杠和      15
                paishu[16]=1;     //发          16
                paishu[17]=1;     //字明刻      17
                paishu[18]=2;     //字暗刻      18
                paishu[19]=3;     //字明杠      19
                paishu[20]=4;     //字暗杠       20
                paishu[21]=1;     //明杠         21
                paishu[22]=2;     //暗杠         22
                paishu[23]=1;     //独听(砍张、边张、吊将) 23
                paishu[24]=1;    //门清        24
    avepaishu=0;
    for(int i=0;i<25;i++)
        avepaishu+=paishu[i];
    avepaishu/=25;

    moneyperpai=-1.0;

    begin =false;
    numofgames=0;
    
    imagetype=1;
    setbkcolor();

    m_sound=true;

    imdownming1=imdownming2=imdownan1=imdownan2=NULL;

    Randomize();
    zhuangjia=random(4);
    #ifndef NDEBUG
    gamespeed=500;
    #else
    gamespeed=1000;
    #endif
    difficult=2;
    downsc=leftsc=upsc=rightsc=100;

    clearanddrawname();


    

}
//---------------------------------------------------------------------------


void TForm1::leftclick(int X,int Y)
{
    int i;

        switch(gamestat)
        {    
        case PlayerWaitForSelectBtn:
            i=downplayer.getindex(X,Y);

            if(i==102&&(playerPGflag&btPeng))
            {
                if(laststat==LeftWaitForMo)
                {
                    if(leftcom.wanttohu(false))
                    {
                        gamestat=LeftWaitForHu;
                        playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=2;
                         timer->Enabled=true;
                         break;
                    }
                     if(rightcom.wanttohu(false))
                    {
                        gamestat=RightWaitForHu;
                        playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=2;
                         timer->Enabled=true;
                        break;
                    }

                }
                else
                {
                    if(rightcom.wanttohu(false)&&laststat!=UpWaitForMo)
                    {
                        gamestat=RightWaitForHu;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=1;
                         timer->Enabled=true;
                        break;
                    }
                    if(upcom.wanttohu(false))
                    {
                        gamestat=UpWaitForHu;
                        playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=(laststat==UpWaitForMo)?3:1;
                         timer->Enabled=true;
                         break;
                    }
                    if(leftcom.wanttohu(false)&&laststat!=PlayerWaitForChu)
                    {
                        gamestat=LeftWaitForHu;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=3;
                         timer->Enabled=true;
                         break;
                    }
                }

                downplayer.peng();
                gang=false;

                clearform(Rect(153,454,648,502));
                downplayer.resetbegin();
                downplayer.draw(imagetype);
                //clearmjot();
                //mjot.draw(imagetype);
                clearform(Rect(278,120,528,400));
                mjots.draw(imagetype);


                playerPGflag=btNo;
                drawbt(playerPGflag);

                gamestat=PlayerWaitForChu;

            }
            if(i==103&&(playerPGflag&btGang))
            {
                if(laststat==LeftWaitForMo&&!flag_bugang)
                {
                    if(leftcom.wanttohu(false))
                    {
                        gamestat=LeftWaitForHu;
                        downplayerwanttogang=true;
                         playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=2;
                         timer->Enabled=true;
                         break;
                    }
                     if(rightcom.wanttohu(false))
                    {
                        gamestat=RightWaitForHu;
                        downplayerwanttogang=true;
                        playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=2;
                         timer->Enabled=true;
                        break;
                    }

                }
                else if(!flag_bugang)
                {
                    if(rightcom.wanttohu(false)&&laststat!=UpWaitForMo)
                    {
                        gamestat=RightWaitForHu;
                        downplayerwanttogang=true;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=1;
                         timer->Enabled=true;
                        break;
                    }
                    if(upcom.wanttohu(false))
                    {
                        gamestat=UpWaitForHu;
                        downplayerwanttogang=true;
                        playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=(laststat==UpWaitForMo)?3:1;
                         timer->Enabled=true;
                         break;
                    }
                    if(leftcom.wanttohu(false)&&laststat!=PlayerWaitForChu)
                    {
                        gamestat=LeftWaitForHu;
                        downplayerwanttogang=true;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=3;
                         timer->Enabled=true;
                         break;
                    }
                }

                if(!flag_bugang)
                    downplayer.gang();
                else
                {
                    downplayer.bugang();
                    flag_bugang=false;
                }
                gang=true;

                playerPGflag=btNo;
                if(downplayer.canbugang())
                {
                    flag_bugang=true;
                     playerPGflag=btGang;
                }

                if(downplayer.canhu(true))
                {
                    flag_zimo=true;
                    playerPGflag=(btstat)(playerPGflag|btHu);
                }
                if(playerPGflag!=0)
                {
                    laststat=PlayerWaitForChu;
                    gamestat=PlayerWaitForSelectBtn;
                }
                else
                    gamestat=PlayerWaitForChu;

                clearform(Rect(153,454,648,502));
                downplayer.resetbegin();
                downplayer.draw(imagetype);
                clearmjot();
                mjot.draw(imagetype);
                if(!flag_bugang)
                {
                    clearform(Rect(278,120,528,400));
                    mjots.draw(imagetype);
                }

                drawbt(playerPGflag);


            }

            if(i==104&&(playerPGflag&btHu))
            {
                if(laststat==LeftWaitForMo&&!flag_zimo)
                {
                    if(leftcom.wanttohu(false))
                    {
                        gamestat=LeftWaitForHu;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=2;
                        timer->Enabled=true;
                         break;
                    }
                }
                if(laststat==UpWaitForMo&&!flag_zimo)
                {
                    if(upcom.wanttohu(false))
                    {
                        gamestat=UpWaitForHu;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=3;
                        timer->Enabled=true;
                         break;
                    }
                    if(leftcom.wanttohu(false))
                    {
                        gamestat=LeftWaitForHu;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=3;
                        timer->Enabled=true;
                         break;
                    }

                }

                int flag_qghu=false;
                if(!flag_zimo)
                    if(rightcom.wanttogang()||leftcom.wanttogang()||upcom.wanttogang())
                        flag_qghu=true;

                if(!flag_zimo)downplayer.painewget=mjots.getnewest();
                /*upcom.painewget=11; //另3人补一张其实只是为了不让红旗飘飘函数检测
                leftcom.painewget=11;//painewget时出错，此牌对输的'人'来说已经无用
                rightcom.painewget=11;   */

                int loser;
                switch(laststat)
                {
                case LeftWaitForMo:
                     loser=2;
                     break;
                case PlayerWaitForChu:
                     loser=1;
                     break;
                case UpWaitForMo:
                     loser=3;
                     break;
                }

                int sc[4];
                soundhu();
                hupaiform=new Thupai(this,downplayer,leftcom,upcom,rightcom,gang,
                    flag_qghu,flag_zimo,0,loser,zhuangjia,name,paishu,imagetype);
                assert(hupaiform);
                hupaiform->calculate(sc);
                hupaiform->ShowModal();
                begin=false;

                delete hupaiform;

                downsc+=sc[0];
                leftsc+=sc[1];
                upsc+=sc[2];
                rightsc+=sc[3];
                
                zhuangjia=0;
                clearanddrawname();

                N3->Enabled=true;
                N4->Enabled=true;
                N11->Enabled=true;
                start->Visible=true;
                                                                              
            }
            break;

        case PlayerWaitForChu:

            i=downplayer.getindex(X,Y);
            assert(i==50||i==100||(i>=101&&i<=104)||(i>=0&&i<downplayer.getcount()));
            if(i<=50)
            {
                downplayer.select(i,imagetype);
                playerPGflag=btChu;

                drawbt(playerPGflag);
            }

            if(i==101&&(playerPGflag&btChu))
            {
                downplayer.chu();
                
                clearform(Rect(153,454,648,502));
                downplayer.resetbegin();
                downplayer.draw(imagetype);
                clearform(Rect(278,120,528,400));
                mjots.draw(imagetype);
                playerPGflag=btNo;
                drawbt(playerPGflag);

                if(rightcom.wanttohu(false))
                {
                     gamestat=RightWaitForHu;
                     flag_zimo=false;
                     loser=0;
                     timer->Enabled=true;
                     break;
                }
                if(upcom.wanttohu(false))
                {
                     gamestat=UpWaitForHu;
                     flag_zimo=false;
                     loser=0;
                     timer->Enabled=true;
                     break;
                }
                if(leftcom.wanttohu(false))
                {
                     gamestat=LeftWaitForHu;
                     flag_zimo=false;
                     loser=0;
                     timer->Enabled=true;
                     break;
                }

                if(leftcom.wanttopeng())
                {
                    gamestat=LeftWaitForPG;
                    flag_pg=1;
                    timer->Enabled=true;
                    break;
                }
                if(leftcom.wanttogang())
                {
                    gamestat=LeftWaitForPG;
                    flag_pg=2;
                    timer->Enabled=true;
                     break;
                }
                if(rightcom.wanttopeng())
                {
                    gamestat=RightWaitForPG;
                    flag_pg=1;
                    timer->Enabled=true;
                    break;
                }
                if(rightcom.wanttogang())
                {
                    gamestat=RightWaitForPG;
                    flag_pg=2;
                    timer->Enabled=true;
                    break;
                }
                if(upcom.wanttopeng())
                {
                    gamestat=UpWaitForPG;
                    flag_pg=1;
                    timer->Enabled=true;
                    break;
                }
                if(upcom.wanttogang())
                {
                    gamestat=UpWaitForPG;
                    flag_pg=2;
                    timer->Enabled=true;
                    break;
                }

                timer->Enabled=true;
                gamestat=RightWaitForMo;

            }
            break;
        }
}
void TForm1::clearanddrawname()
{
                Label1->Caption=name[0];

                Label2->Caption=name[1];

                Label3->Caption=name[2];

                Label4->Caption=name[3];

    switch(zhuangjia)
    {
    case 0:
         Label1->Caption=Label1->Caption+"(庄家)";
         break;
    case 1:
         Label2->Caption=Label2->Caption+"(庄家)";
         break;
    case 2:
         Label3->Caption=Label3->Caption+"(庄家)";
         break;
    case 3:
         Label4->Caption=Label4->Caption+"(庄家)";
         break;
    }

    if(moneyperpai<=0.0)
    {
                Label1->Caption=Label1->Caption+"\n"+downsc;
                Label2->Caption=Label2->Caption+"\n"+leftsc;
                Label3->Caption=Label3->Caption+"\n"+upsc;
                Label4->Caption=Label4->Caption+"\n"+rightsc;
    }
    else
    {
        AnsiString ls;
        ls=(AnsiString(downsc*moneyperpai).Pos(".")>0)?AnsiString(downsc*moneyperpai):AnsiString(downsc*moneyperpai)+".";
        Label1->Caption=Label1->Caption+"\n￥"+ls.SubString(1,ls.Pos(".")+2);

        ls=(AnsiString(leftsc*moneyperpai).Pos(".")>0)?AnsiString(leftsc*moneyperpai):AnsiString(leftsc*moneyperpai)+".";
        Label2->Caption=Label2->Caption+"\n￥"+ls.SubString(1,ls.Pos(".")+2);

        ls=(AnsiString(upsc*moneyperpai).Pos(".")>0)?AnsiString(upsc*moneyperpai):AnsiString(upsc*moneyperpai)+".";
        Label3->Caption=Label3->Caption+"\n￥"+ls.SubString(1,ls.Pos(".")+2);
        
        ls=(AnsiString(rightsc*moneyperpai).Pos(".")>0)?AnsiString(rightsc*moneyperpai):AnsiString(rightsc*moneyperpai)+".";
        Label4->Caption=Label4->Caption+"\n￥"+ls.SubString(1,ls.Pos(".")+2);
    }

    Label8->Caption="第"+AnsiString(numofgames+1)+"局";
                clearform();
                Label1->Refresh();
                Label2->Refresh();
                Label3->Refresh();
                Label4->Refresh();

}
void TForm1::rightclick(int X,int Y)
{
        int i;
        switch(gamestat)
        {
        case PlayerWaitForSelectBtn:

            if(flag_bugang||flag_zimo)
            {
                playerPGflag=btNo;
                drawbt(playerPGflag);

                gamestat=PlayerWaitForChu;
                
                flag_bugang=false;
                break;
            }



            //=================
                if(laststat==LeftWaitForMo)
                {
                    if(leftcom.wanttohu(false))
                    {
                        gamestat=LeftWaitForHu;
                        playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=2;
                        timer->Enabled=true;
                         break;
                    }
                     if(rightcom.wanttohu(false))
                    {
                        gamestat=RightWaitForHu;
                        playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=2;
                        timer->Enabled=true;
                        break;
                    }

                }
                else
                {
                    if(rightcom.wanttohu(false)&&laststat!=UpWaitForMo)
                    {
                        gamestat=RightWaitForHu;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=1;
                        timer->Enabled=true;
                        break;
                    }
                    if(upcom.wanttohu(false))
                    {
                        gamestat=UpWaitForHu;
                        playerPGflag=btNo;
                         drawbt(playerPGflag);
                         loser=(laststat==UpWaitForMo)?3:1;
                        timer->Enabled=true;
                         break;
                    }
                    if(leftcom.wanttohu(false)&&laststat!=PlayerWaitForChu)
                    {
                        gamestat=LeftWaitForHu;
                        playerPGflag=btNo;
                        drawbt(playerPGflag);
                        loser=3;
                        timer->Enabled=true;
                         break;
                    }
                }
            //===================
            if(laststat!=PlayerWaitForChu)
            {
                gamestat=laststat;
                playerPGflag=btNo;
                drawbt(playerPGflag);
                timer->Enabled=true;
                break;
            }
            
            playerPGflag=btNo;
            drawbt(playerPGflag);

            if(mjot.getcount()<8)
            {
                gamestat=WaitForHuangZhuang;
                timer->Enabled=true;
                break;
            }

            gang=false;
            if(downplayer.mo())////
            {
                clearform(Rect(153,454,648,502));
                downplayer.resetbegin();
            }
            downplayer.draw(imagetype);
            clearmjot();
            mjot.draw(imagetype);

            if(downplayer.canbugang())
            {
                flag_bugang=true;
                playerPGflag=(btstat)(playerPGflag|btGang);
            }
            if(downplayer.canhu(true))
            {
                flag_zimo=true;
                 playerPGflag=(btstat)(playerPGflag|btHu);
             }
            if(playerPGflag!=0)
            {
                gamestat=PlayerWaitForSelectBtn;
                drawbt(playerPGflag);
                timer->Enabled=false;
                 break;
             }


            drawbt(btNo);

            gamestat=PlayerWaitForChu;
            break;
        case PlayerWaitForChu:
            if(playerPGflag&btChu)
            {
                downplayer.chu();
                clearform(Rect(153,454,648,502));
                downplayer.resetbegin();
                downplayer.draw(imagetype);
                clearform(Rect(278,120,528,400));
                mjots.draw(imagetype);
                playerPGflag=btNo;
                drawbt(playerPGflag);

                if(rightcom.wanttohu(false))
                {
                     gamestat=RightWaitForHu;
                     flag_zimo=false;
                     loser=0;
                     timer->Enabled=true;
                     break;
                }
                if(upcom.wanttohu(false))
                {
                     gamestat=UpWaitForHu;
                     flag_zimo=false;
                     loser=0;
                     timer->Enabled=true;
                     break;
                }
                if(leftcom.wanttohu(false))
                {
                     gamestat=LeftWaitForHu;
                     flag_zimo=false;
                     loser=0;
                     timer->Enabled=true;
                     break;
                }

                if(leftcom.wanttopeng())
                {
                    gamestat=LeftWaitForPG;
                    flag_pg=1;
                    timer->Enabled=true;
                    break;
                }
                if(leftcom.wanttogang())
                {
                    gamestat=LeftWaitForPG;
                    flag_pg=2;
                    timer->Enabled=true;
                     break;
                }
                if(rightcom.wanttopeng())
                {
                    gamestat=RightWaitForPG;
                    flag_pg=1;
                    timer->Enabled=true;
                    break;
                }
                if(rightcom.wanttogang())
                {
                    gamestat=RightWaitForPG;
                    flag_pg=2;
                    timer->Enabled=true;
                    break;
                }
                if(upcom.wanttopeng())
                {
                    gamestat=UpWaitForPG;
                    flag_pg=1;
                    timer->Enabled=true;
                    break;
                }
                if(upcom.wanttogang())
                {
                    gamestat=UpWaitForPG;
                    flag_pg=2;
                    timer->Enabled=true;
                    break;
                }

                timer->Enabled=true;
                gamestat=RightWaitForMo;
            }
            else//未选中牌,排序
            {
                downplayer.sortall();
                downplayer.draw(imagetype);
            }
            break;
        }
}
void __fastcall TForm1::PaintBox1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{


    if(Button==mbLeft)
        leftclick(X,Y);
    else if(Button==mbRight)
       rightclick(X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormResize(TObject *Sender)
{

    this->Top=-15;
    this->Left=0;

    ClientHeight=600;
    ClientWidth=790;
    PaintBox1->Height=ClientHeight;
    PaintBox1->Width=ClientWidth;

    PaintBox1->Left=0;
    PaintBox1->Top=0;

    start->Left=(ClientWidth-start->Width)/2;
    start->Top=(ClientWidth)*3/5;

    Label1->Top=515;
    Label1->Left=(ClientWidth-Label1->Width)/2;

    Label2->Left=120 ;
    Label2-> Top=ClientHeight/2;

    Label4->Left=687-13-20-Label4->Width ;
    Label4-> Top=ClientHeight/2;

    Label3->Top=69;
    Label3->Left=520;

}
//---------------------------------------------------------------------------


 void TForm1::clearmjot()
 {
    clearform(Rect(238,147,259,374));
    clearform(Rect(288,86,515,107));
    clearform(Rect(537,146,558,373));
    clearform(Rect(287,410,514,431));
 }
 void TForm1::clearform()
{
    PaintBox1->Canvas->FillRect(Rect(0,0,ClientWidth,ClientHeight));
}



void TForm1::clearform(TRect rect)
{
    PaintBox1->Canvas->FillRect(rect);
}





void TForm1::drawbt(btstat bs)
{
    int bttype,i=0,j=0;
    for(bttype=btChu;bttype<=btHu;bttype*=2)
    {
        if(bttype==btPeng) j++;//碰 吃
        if(bttype==btHu) j++;//取消听
        if(bs&bttype)
            PaintBox1->Canvas->CopyRect(Rect(500+i*16,520,500+i*16+16,520+18),
                imanniu->Canvas, Rect(j*16,19,j*16+16,19+18));

        else
            PaintBox1->Canvas->CopyRect(Rect(500+i*16,520,500+i*16+16,520+18),
                imanniu->Canvas, Rect(j*16,0,j*16+16,18));


        i++; j++;
    }
}

void TForm1::setbkcolor()
{
    if(imagetype==1)
    {
        Color=clGreen;
        PaintBox1->Color=clGreen;
    }
    else
    {
        Color=(TColor)0x00004080;
        PaintBox1->Color=(TColor)0x00004080;

    }
}
void __fastcall TForm1::PaintBox1Paint(TObject *Sender)
{
    if(!begin) return;
    mjots.draw(imagetype);
    mjot.draw(imagetype);

    leftcom.resetbegin();
    leftcom.draw(imagetype);

    upcom.resetbegin();
    upcom.draw(imagetype);

    rightcom.resetbegin();
    rightcom.draw(imagetype);

    downplayer.resetbegin();
    downplayer.draw(imagetype);

    drawbt(playerPGflag);
}
//---------------------------------------------------------------------------
void TForm1::timerfun()
{
    playerPGflag=btNo;

    int  sc[4];//和了  得分数
    
    bool flag_qghu1;
    flag_qghu1=false;

    switch(gamestat)
    {
    case fapai:
        if(fapai_num==52)
        {
            downplayer.sort();
/*            #ifndef NVIEWMAN
            leftcom.sort();          upcom.sort();   rightcom.sort();
            #endif    */

            clearform(Rect(153,454,648,502));
            downplayer.resetbegin();
            downplayer.draw(imagetype);
            clearform(Rect(52,33,100,528));
            leftcom.resetbegin();
            leftcom.draw(imagetype);
            clearform(Rect(132,20,627,68));
            upcom.resetbegin();
            upcom.draw(imagetype);
            clearform(Rect(687,37,735,532));
            rightcom.resetbegin();
            rightcom.draw(imagetype);


            switch(zhuangjia)
            {
            case 0:
                bool bMo;
                bMo=downplayer.mo();
                //gang=false;

                if(downplayer.canbugang())
                {
                     flag_bugang=true;
                     playerPGflag=btGang;
                }
                if(downplayer.canhu(true))
                {
                    flag_zimo=true;
                    playerPGflag=(btstat)(playerPGflag|btHu);
                }
                if(playerPGflag!=0)
                    gamestat=PlayerWaitForSelectBtn;
                if(bMo)////
                {
                    clearform(Rect(153,454,648,502));
                    downplayer.resetbegin();
                }
                downplayer.draw(imagetype);

                gamestat=PlayerWaitForChu;
                break;
            case 1:
                gamestat=LeftWaitForMo;
                 break;
            case 2:
                gamestat=UpWaitForMo;
                break;
            case 3:
                gamestat=RightWaitForMo;
                break;
            }

            downplayerwanttogang=false;

            timer->Interval=gamespeed;
            timer->Enabled=(gamestat==PlayerWaitForChu||
                gamestat==PlayerWaitForSelectBtn)?false:true;

            break;
        }
        switch(fapai_dir)
        {
        case 0:
            if(fapai_num<=44)
            {
                downplayer.insertmj(mjot.takeone(true));
                downplayer.insertmj(mjot.takeone(true));
                downplayer.insertmj(mjot.takeone(true));
                downplayer.insertmj(mjot.takeone(true));
                fapai_num+=4;
            }
            else
            {
                downplayer.insertmj(mjot.takeone(true));
                fapai_num+=1;
            }
            clearform(Rect(153,454,648,502));
            downplayer.resetbegin();
            downplayer.draw(imagetype);
            clearmjot();
            mjot.draw(imagetype);

            break;
        case 1:
            if(fapai_num<=44)
            {
                leftcom.insertmj(mjot.takeone(true));
                leftcom.insertmj(mjot.takeone(true));
                leftcom.insertmj(mjot.takeone(true));
                leftcom.insertmj(mjot.takeone(true));
                fapai_num+=4;
            }
            else
            {
                leftcom.insertmj(mjot.takeone(true));
                fapai_num+=1;
            }
            clearform(Rect(52,33,100,528));
            leftcom.resetbegin();
            leftcom.draw(imagetype);
            clearmjot();
            mjot.draw(imagetype);

            break;
        case 2:
            if(fapai_num<=44)
            {
                upcom.insertmj(mjot.takeone(true));
                upcom.insertmj(mjot.takeone(true));
                upcom.insertmj(mjot.takeone(true));
                upcom.insertmj(mjot.takeone(true));
                fapai_num+=4;
            }
            else
            {
                upcom.insertmj(mjot.takeone(true));
                fapai_num+=1;
            }

            clearform(Rect(132,20,627,68));
            upcom.resetbegin();
            upcom.draw(imagetype);
            clearmjot();
            mjot.draw(imagetype);
            break;
        case 3:
            if(fapai_num<=44)
            {
                rightcom.insertmj(mjot.takeone(true));
                rightcom.insertmj(mjot.takeone(true));
                rightcom.insertmj(mjot.takeone(true));
                rightcom.insertmj(mjot.takeone(true));
                fapai_num+=4;
            }
            else
            {
                rightcom.insertmj(mjot.takeone(true));
                fapai_num+=1;
            }
            clearform(Rect(687,37,735,532));
            rightcom.resetbegin();
            rightcom.draw(imagetype);
            clearmjot();
            mjot.draw(imagetype);

            break;
        }

        fapai_dir=fapai_dir==0?3:fapai_dir-1;


        break;
    case UpWaitForPG:
        if(flag_pg==1)
        {
            upcom.peng();
            gang=false;
        }
        else
        {
            upcom.gang();
            gang=true;
        }

        clearform(Rect(132,20,627,68));
        upcom.resetbegin();
        upcom.draw(imagetype);
        clearform(Rect(278,120,528,400));
        mjots.draw(imagetype);

        if(flag_pg==2&&upcom.wanttohu(true))
        {
            flag_zimo=true;
            gamestat=UpWaitForHu;
        }
        else if(upcom.wanttobugang())
            gamestat=UpWaitForBu;
        else
            gamestat=UpWaitForChu;

        break;
    case UpWaitForMo:
            if(mjot.getcount()<8)
            {
                gamestat=WaitForHuangZhuang;
                //timer->Enabled=true;
                break;
            }

        gang=false;
        if(upcom.mo())////
        {
            clearform(Rect(132,20,627,68));
            upcom.resetbegin();
        }
        upcom.draw(imagetype);
        clearmjot();
        mjot.draw(imagetype);

        if(upcom.wanttohu(true))
        {
             gamestat=UpWaitForHu;
             flag_zimo=true;
             break;
        }

        if(upcom.wanttobugang())
             gamestat=UpWaitForBu;
        else gamestat=UpWaitForChu;

        break;
    case UpWaitForBu:
        upcom.bugang();
        gang=true;

        clearform(Rect(132,20,627,68));
        upcom.resetbegin();
        upcom.draw(imagetype);
        clearmjot();
        mjot.draw(imagetype);

        if(upcom.wanttohu(true))
        {
            flag_zimo=true;
            gamestat=UpWaitForHu;
        }
        else if(upcom.wanttobugang())
            gamestat=UpWaitForBu;
        else
            gamestat=UpWaitForChu;

        break;
    case UpWaitForChu:
        upcom.chu();
        
        clearform(Rect(132,20,627,68));
        upcom.resetbegin();
        upcom.draw(imagetype);
        //clearform(Rect(278,120,528,400));
        mjots.draw(imagetype);

        if(downplayer.canpeng()){
            playerPGflag=(btstat)(playerPGflag|btPeng);
            flag_bugang=flag_zimo=false;

        }
        if(downplayer.cangang())
        {    flag_bugang=false;
            playerPGflag=(btstat)(playerPGflag|btGang);
        }
        if(downplayer.canhu(false))
        {
             playerPGflag=(btstat)(playerPGflag|btHu);
             flag_zimo=false;
        }
        if(playerPGflag!=0)
        {
            drawbt(playerPGflag);
            gamestat=PlayerWaitForSelectBtn;
            laststat=LeftWaitForMo;
            timer->Enabled=false;
            break;
        }

        if(leftcom.wanttohu(false))
        {
             gamestat=LeftWaitForHu;
             flag_zimo=false;
             loser=2;
             break;
        }

        if(rightcom.wanttohu(false))
        {
             gamestat=RightWaitForHu;
             flag_zimo=false;
             loser=2;
             break;
        }
        if(leftcom.wanttopeng())
        {
             gamestat=LeftWaitForPG;
             flag_pg=1;
             break;
        }
        if(leftcom.wanttogang())
        {
            gamestat=LeftWaitForPG;
            flag_pg=2;
            break;
        }
        if(rightcom.wanttopeng())
        {
             gamestat=RightWaitForPG;
             flag_pg=1;
             break;
        }
        if(rightcom.wanttogang())
        {
            gamestat=RightWaitForPG;
            flag_pg=2;
            break;
        }


        gamestat=LeftWaitForMo;

        break;
    case LeftWaitForPG:
        if(flag_pg==1)
        {
            leftcom.peng();
            gang=false;
        }
        else
        {
            leftcom.gang();
            gang=true;
        }

        clearform(Rect(52,33,100,528));
        leftcom.resetbegin();
        leftcom.draw(imagetype);
        clearform(Rect(278,120,528,400));
        mjots.draw(imagetype);

        if(flag_pg==2&&leftcom.wanttohu(true))
        {
            flag_zimo=true;
            gamestat=LeftWaitForHu;
        }
        else if(leftcom.wanttobugang())
            gamestat=LeftWaitForBu;
        else
            gamestat=LeftWaitForChu;
        break;
    case LeftWaitForMo:
            if(mjot.getcount()<8)
            {
                gamestat=WaitForHuangZhuang;
                //timer->Enabled=true;
                break;
            }

        gang=false;
        if(leftcom.mo())////
        {
            clearform(Rect(52,33,100,528));
            leftcom.resetbegin();
        }
        leftcom.draw(imagetype);
        clearmjot();
        mjot.draw(imagetype);

        if(leftcom.wanttohu(true))
        {
             gamestat=LeftWaitForHu;
             flag_zimo=true;
             break;
        }

        if(leftcom.wanttobugang())
             gamestat=LeftWaitForBu;
        else gamestat=LeftWaitForChu;

        break;
    case LeftWaitForBu:
        leftcom.bugang();
        gang=true;

        clearform(Rect(52,33,100,528));
        leftcom.resetbegin();
        leftcom.draw(imagetype);
        clearmjot();
        mjot.draw(imagetype);

        if(leftcom.wanttohu(true))
        {
            flag_zimo=true;
            gamestat=LeftWaitForHu;
        }
        else if(leftcom.wanttobugang())
            gamestat=LeftWaitForBu;
        else
            gamestat=LeftWaitForChu;
        break;
    case LeftWaitForChu:
        leftcom.chu();

        clearform(Rect(52,33,100,528));
        leftcom.resetbegin();
        leftcom.draw(imagetype);
        //clearform(Rect(278,120,528,400));
        mjots.draw(imagetype);

        if(downplayer.canpeng()){
            playerPGflag=(btstat)(playerPGflag|btPeng);
            flag_bugang=flag_zimo=false;
        }
        if(downplayer.cangang())
        {    flag_bugang=false;
            playerPGflag=(btstat)(playerPGflag|btGang);
        }
        if(downplayer.canhu(false))
        {
             playerPGflag=(btstat)(playerPGflag|btHu);
             flag_zimo=false;
        }
        if(playerPGflag!=0)
        {
            drawbt(playerPGflag);
            gamestat=PlayerWaitForSelectBtn;
            laststat=PlayerWaitForChu;
            timer->Enabled=false;
            break;
        }

        if(rightcom.wanttohu(false))
        {
             gamestat=RightWaitForHu;
             flag_zimo=false;
             loser=1;
             break;
        }
        if(upcom.wanttohu(false))
        {
             gamestat=UpWaitForHu;
             flag_zimo=false;
             loser=1;
             break;
        }
        
        if(upcom.wanttopeng())
        {
             gamestat=UpWaitForPG;
             flag_pg=1;
             break;
        }
        if(upcom.wanttogang())
        {
            gamestat=UpWaitForPG;
            flag_pg=2;
            break;
        }
        if(rightcom.wanttopeng())
        {
             gamestat=RightWaitForPG;
             flag_pg=1;
             break;
        }
        if(rightcom.wanttogang())
        {
            gamestat=RightWaitForPG;
            flag_pg=2;
            break;
        }

            if(mjot.getcount()<8)
            {
                gamestat=WaitForHuangZhuang;
//                timer->Enabled=true;
                break;
            }

        gang=false;
        if(downplayer.mo())////
        {
            clearform(Rect(153,454,648,502));
            downplayer.resetbegin();
        }
        downplayer.draw(imagetype);
        clearmjot();
        mjot.draw(imagetype);



        if(downplayer.canbugang())
        {
             flag_bugang=true;
             playerPGflag=btGang;
        }
        if(downplayer.canhu(true))
        {
            flag_zimo=true;
            playerPGflag=(btstat)(playerPGflag|btHu);
        }
        if(playerPGflag!=0)
        {
            gamestat=PlayerWaitForSelectBtn;
            drawbt(playerPGflag);
            timer->Enabled=false;
            break;
        }




        drawbt(btNo);

        gamestat=PlayerWaitForChu;
        timer->Enabled=false;


        break;
    case RightWaitForPG:
        if(flag_pg==1)
        {
            rightcom.peng();
            gang=false;
        }
        else
        {
            rightcom.gang();
            gang=true;
        }

        clearform(Rect(687,37,735,532));
        rightcom.resetbegin();
        rightcom.draw(imagetype);
        clearform(Rect(278,120,528,400));
        mjots.draw(imagetype);
        
        if(flag_pg==2&&rightcom.wanttohu(true))
        {
            flag_zimo=true;
            gamestat=RightWaitForHu;
        }
        else if(rightcom.wanttobugang())
            gamestat=RightWaitForBu;
        else
            gamestat=RightWaitForChu;
        break;
    case RightWaitForMo:
            if(mjot.getcount()<8)
            {
                gamestat=WaitForHuangZhuang;
//                timer->Enabled=true;
                break;
            }

        gang=false;
        if(rightcom.mo())////
        {
            clearform(Rect(687,37,735,532));
            rightcom.resetbegin();
        }
        rightcom.draw(imagetype);
        clearmjot();
        mjot.draw(imagetype);

        if(rightcom.wanttohu(true))
        {
             gamestat=RightWaitForHu;
             flag_zimo=true;
             break;
        }

        if(rightcom.wanttobugang())
             gamestat=RightWaitForBu;
        else gamestat=RightWaitForChu;

        break;
    case RightWaitForBu:
        rightcom.bugang();
        gang=true;
        
        clearform(Rect(687,37,735,532));
        rightcom.resetbegin();
        rightcom.draw(imagetype);
        clearmjot();
        mjot.draw(imagetype);

        if(rightcom.wanttohu(true))
        {
            flag_zimo=true;
            gamestat=RightWaitForHu;
        }
        else if(rightcom.wanttobugang())
            gamestat=RightWaitForBu;
        else
            gamestat=RightWaitForChu;
        break;
    case RightWaitForChu:
        rightcom.chu();

        clearform(Rect(687,37,735,532));
        rightcom.resetbegin();
        rightcom.draw(imagetype);
        //clearform(Rect(278,120,528,400));
        mjots.draw(imagetype);

        if(downplayer.canpeng()){
            playerPGflag=(btstat)(playerPGflag|btPeng);
            flag_bugang=flag_zimo=false;
        }
        if(downplayer.cangang())
        {    flag_bugang=false;
            playerPGflag=(btstat)(playerPGflag|btGang);
        }
        if(downplayer.canhu(false))
        {
             playerPGflag=(btstat)(playerPGflag|btHu);
             flag_zimo=false;
        }
        if(playerPGflag!=0)
        {
            drawbt(playerPGflag);
            gamestat=PlayerWaitForSelectBtn;
            laststat=UpWaitForMo;
            timer->Enabled=false;
            break;
        }

        if(upcom.wanttohu(false))
        {
             gamestat=UpWaitForHu;
             flag_zimo=false;
             loser=3;
             break;
        }
        if(leftcom.wanttohu(false))
        {
             gamestat=LeftWaitForHu;
             flag_zimo=false;
             loser=3;
             break;
        }
        
        if(leftcom.wanttopeng())
        {
             gamestat=LeftWaitForPG;
             flag_pg=1;
             break;
        }
        if(leftcom.wanttogang())
        {
            gamestat=LeftWaitForPG;
            flag_pg=2;
            break;
        }
        if(upcom.wanttopeng())
        {
             gamestat=UpWaitForPG;
             flag_pg=1;
             break;
        }
        if(upcom.wanttogang())
        {
            gamestat=UpWaitForPG;
            flag_pg=2;
            break;
        }

        gamestat=UpWaitForMo;

        break;

    case UpWaitForHu:
        // 访问两家电脑 及downplayerwanttogang=true表示玩家也杠
        //判断抢杠和
         //判断painewget是否为零,判断自摸
            timer->Enabled=false;

            if(!flag_zimo)
            {
                upcom.painewget=mjots.getnewest();
            /*rightcom.painewget=11; //另3人补一张其实只是为了不让红旗飘飘函数检测
            leftcom.painewget=11;//painewget时出错，此牌对输的人来说已经无用
            downplayer.painewget=11; */

            //flag_qghu1=false;
                if(downplayerwanttogang||rightcom.wanttogang()||leftcom.wanttogang())
                    flag_qghu1=true;

                //if(!flag_zimo)
                assert(loser!=2);
            }
            soundhu();
            hupaiform=new Thupai(this,downplayer,leftcom,upcom,rightcom,gang,
                flag_qghu1,flag_zimo,2,loser,zhuangjia,name,paishu,imagetype);
            assert(hupaiform);
            hupaiform->calculate(sc);
            hupaiform->ShowModal();
            begin=false;
            delete hupaiform;
                downsc+=sc[0];
                leftsc+=sc[1];
                upsc+=sc[2];
                rightsc+=sc[3];

                zhuangjia=2;
                clearanddrawname();

                N3->Enabled=true;
                N4->Enabled=true;
                N11->Enabled=true;
                start->Visible=true;

            break;
    case LeftWaitForHu:
        // 访问两家电脑 及downplayerwanttogang=true表示玩家也杠
        //判断抢杠和
         //判断painewget是否为零,判断自摸
            timer->Enabled=false;

            if(!flag_zimo)
            {
                leftcom.painewget=mjots.getnewest();
            /*upcom.painewget=11; //另3人补一张其实只是为了不让红旗飘飘函数检测
            rightcom.painewget=11;//painewget时出错，此牌对输的人来说已经无用
            downplayer.painewget=11;  */

            //flag_qghu1=false;
                if(downplayerwanttogang||rightcom.wanttogang()||upcom.wanttogang())
                    flag_qghu1=true;


                //if(!flag_zimo)
                assert(loser!=1);
            }
            soundhu();
            hupaiform=new Thupai(this,downplayer,leftcom,upcom,rightcom,gang,
                flag_qghu1,flag_zimo,1,loser,zhuangjia,name,paishu,imagetype);
            assert(hupaiform);
            hupaiform->calculate(sc);
            hupaiform->ShowModal();
            begin=false;
            delete hupaiform;
                downsc+=sc[0];
                leftsc+=sc[1];
                upsc+=sc[2];
                rightsc+=sc[3];


                zhuangjia=1;
                clearanddrawname();

                N3->Enabled=true;
                N4->Enabled=true;
                N11->Enabled=true;
                start->Visible=true;


            break;
    case RightWaitForHu:
        // 访问两家电脑 及downplayerwanttogang=true表示玩家也杠
        //判断抢杠和
         //判断painewget是否为零,判断自摸
            timer->Enabled=false;

            if(!flag_zimo)
            {
                rightcom.painewget=mjots.getnewest();
            /*upcom.painewget=11; //另3人补一张其实只是为了不让红旗飘飘函数检测
            leftcom.painewget=11;//painewget时出错，此牌对输的人来说已经无用
            downplayer.painewget=11; */

                //flag_qghu1=false;
                if(downplayerwanttogang||upcom.wanttogang()||leftcom.wanttogang())
                    flag_qghu1=true;


                //if(!flag_zimo)
                assert(loser!=3);
            }
            soundhu();
            hupaiform=new Thupai(this,downplayer,leftcom,upcom,rightcom,gang,
                flag_qghu1,flag_zimo,3,loser,zhuangjia,name,paishu,imagetype);
            assert(hupaiform);
            hupaiform->calculate(sc);
            hupaiform->ShowModal();
            begin=false;
            delete hupaiform;
                downsc+=sc[0];
                leftsc+=sc[1];
                upsc+=sc[2];
                rightsc+=sc[3];

                zhuangjia=3;
                clearanddrawname();

                N3->Enabled=true;
                N4->Enabled=true;
                N11->Enabled=true;
                start->Visible=true;

            break;
     case WaitForHuangZhuang:
            timer->Enabled=false;
            Application->MessageBox("流局","流局",MB_OK);

            //clearform();
            begin=false;
            clearanddrawname();

            N3->Enabled=true;
            N4->Enabled=true;
            N11->Enabled=true;
            start->Visible=true;
            break;

    }

}
void __fastcall TForm1::timerTimer(TObject *Sender)
{   timerfun();
}
//---------------------------------------------------------------------------



void __fastcall TForm1::PaintBox1DblClick(TObject *Sender)
{
    if(!begin) return;
    bool a=  timer->Enabled;
    timer->Enabled=false;

    Form2=new TForm2(this);
    assert(Form2);
    Form2->ShowModal();

    delete Form2;
    timer->Enabled=a;    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::startClick(TObject *Sender)
{
    mjot.setfirst(random(4),random(15)*2);

    mjot.reset();
    mjot.wash();

    mjots.clear();
    
    leftcom.clear();
    upcom.clear();
    rightcom.clear();
    downplayer.clear();

    fapai_dir=zhuangjia;
    fapai_num=0;
    gamestat=fapai;
    begin=true;
    numofgames++;

    gang=false;
    flag_bugang=false;
    downplayerwanttogang=false;
    flag_zimo=false;
    playerPGflag=btNo;

    N3->Enabled=false;
    N4->Enabled=false;
    N11->Enabled=false;
    start->Visible=false;

    timer->Interval=gamespeed/5;
    timer->Enabled=true;
    this->Refresh();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::N3Click(TObject *Sender)
{
    if(start->Visible==true)start->Click();    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N4Click(TObject *Sender)
{
    setpaishu=new Tsetpaishu(this,paishu,avepaishu,moneyperpai);
    assert(setpaishu);
    setpaishu->ShowModal();
    delete setpaishu;
    //this->Refresh();

    clearanddrawname();

    if(begin) PaintBox1Paint(Sender); 

}
//---------------------------------------------------------------------------

void __fastcall TForm1::N2Click(TObject *Sender)
{
    saveForm=new TsaveForm(this,paishu,moneyperpai,name,downsc,leftsc,upsc,rightsc,
        zhuangjia,numofgames,gamespeed,difficult,imagetype);
    assert(saveForm);
    saveForm->ShowModal();
    delete saveForm;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N5Click(TObject *Sender)
{
    speedset=new Tspeedset(this,gamespeed);
    assert(speedset);
    speedset->ShowModal();
    delete  speedset;

    timer->Interval=gamespeed;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N10Click(TObject *Sender)
{
    colorset=new Tcolorset(this,imagetype);
    assert(colorset);
    colorset->ShowModal();
    delete colorset;
  /*
if(bigsize) {
    if (imdownming1==NULL)
    {
         imdownming1=new Graphics::TBitmap;
         imdownming1->LoadFromFile("upming1_big.bmp");
    }
    if (imdownming2==NULL)
    {
         imdownming2=new Graphics::TBitmap;
         imdownming2->LoadFromFile("upming2_big.bmp");
    }
    if (imdownan1==NULL)
    {
         imdownan1=new Graphics::TBitmap;
         imdownan1->LoadFromFile("upan1_big.bmp");
    }
    if (imdownan2==NULL)
    {
         imdownan2=new Graphics::TBitmap;
         imdownan2->LoadFromFile("upan2_big.bmp");
    }
}         */

    clearform();
    Refresh();
    setbkcolor();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N11Click(TObject *Sender)
{
    nameset=new Tnameset(this,name,downsc,leftsc,upsc,rightsc,moneyperpai);
    assert(nameset);
    nameset->ShowModal();
    delete nameset;

    clearanddrawname(); 
    if(begin) PaintBox1Paint(Sender); 
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    #ifndef NLABEL
    Label5->Visible=true;
    Label6->Visible=true;
    Label7->Visible=true;
    #endif
    /*mjpai a[]={2,3,4,5,6};
    mjpaizu zu(a,5);

    int d=zu.find(6);*/
}
//---------------------------------------------------------------------------


void __fastcall TForm1::N12Click(TObject *Sender)
{
    timer->Enabled=false;

    TRegistry *reg = new TRegistry;
    assert(reg);
    reg->OpenKeyReadOnly("\\Software\\zhanghao's jjmj");
    bool cb1=false,cb2=false,cb3=false;


    if(reg->ValueExists("cb1"))
        cb1=reg->ReadBool("cb1");
    if(reg->ValueExists("cb2"))
        cb2=reg->ReadBool("cb2");
    if(reg->ValueExists("cb3"))
        cb3=reg->ReadBool("cb3");

    if(!cb1&&!cb2&&!cb3){
        Application->MessageBox("进度为空","无进度",MB_OK);
        reg->CloseKey();
        return;
    }

    if(cb1)
    {

        for(int i=0;i<25;i++)
            paishu[i]=reg->ReadInteger(AnsiString("paishu")+i);
            
        avepaishu=0;
        for(int i=0;i<25;i++)
            avepaishu+=paishu[i];
        avepaishu/=25;

        moneyperpai=reg->ReadFloat("moneyperpai");
    }

    if(cb2)
    {
        for(int i=0;i<4;i++)
            name[i]=reg->ReadString(AnsiString("name")+i);

        downsc=reg->ReadInteger("downsc");
        leftsc=reg->ReadInteger("leftsc");
        upsc=reg->ReadInteger("upsc");
        rightsc=reg->ReadInteger("rightsc");
        zhuangjia=reg->ReadInteger("zhuangjia");
        numofgames=reg->ReadInteger("numofgames");
    }

    if(cb3)
    {
        gamespeed=(unsigned int)reg->ReadInteger("gamespeed");
        difficult=reg->ReadInteger("difficult");
        imagetype=reg->ReadInteger("imagetype");
        //N16->Checked=reg->ReadBool("music");
        //bigsize=reg->ReadBool("bigimage");
    }

    reg->CloseKey();
           /*
if(bigsize) {
    if (imdownming1==NULL)
    {
         imdownming1=new Graphics::TBitmap;
         imdownming1->LoadFromFile("upming1_big.bmp");
    }
    if (imdownming2==NULL)
    {
         imdownming2=new Graphics::TBitmap;
         imdownming2->LoadFromFile("upming2_big.bmp");
    }
    if (imdownan1==NULL)
    {
         imdownan1=new Graphics::TBitmap;
         imdownan1->LoadFromFile("upan1_big.bmp");
    }
    if (imdownan2==NULL)
    {
         imdownan2=new Graphics::TBitmap;
         imdownan2->LoadFromFile("upan2_big.bmp");
    }
}
     */
    begin=false;
    clearanddrawname();
    //N16->Checked?MediaPlayer1->Play():MediaPlayer1->Stop();
    setbkcolor();

    N3->Enabled=true;
    N4->Enabled=true;
    N11->Enabled=true;
    start->Visible=true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N6Click(TObject *Sender)
{
    difficultf=new Tdifficult(this,difficult);
    assert(difficultf);
    difficultf->ShowModal();
    delete difficultf;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N8Click(TObject *Sender)
{
    Form3=new TForm3(this);
    assert(Form3);
    Form3->ShowModal();
    delete Form3;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::N16Click(TObject *Sender)
{
    N16->Checked=!N16->Checked;

    if(N16->Checked)
        m_sound=true;
    else
        m_sound=false;

}
//---------------------------------------------------------------------------



void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    if(imdownming1!=NULL) delete imdownming1;    
    if(imdownming2!=NULL) delete imdownming2;
    if(imdownan1!=NULL) delete imdownan1;
    if(imdownan2!=NULL) delete imdownan2;
}
//---------------------------------------------------------------------------

void soundpeng()
{
    if(!Form1->m_sound)return;
     Form1->sound->FileName="wav\\wfn42.wav";
     Form1->sound->Open();
     Form1->sound->Play();
}
void soundgang()
{
     if(!Form1->m_sound)return;

     Form1->sound->FileName="wav\\wfn43.wav";
     Form1->sound->Open();
     Form1->sound->Play();

}
void soundhu()
{
    if(!Form1->m_sound)return;
     Form1->sound->FileName="wav\\wfn44.wav";
     Form1->sound->Open();
     Form1->sound->Play();

}
