//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "mjpaikuaizu.h"
#include "mjpaizu.h"
#include "everyman.h"
#include <Graphics.hpp>
#include <Menus.hpp>
#include <MPlayer.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TPaintBox *PaintBox1;
    TImage *imup1;
    TImage *imup2;
    TImage *imleft1;
    TImage *imleft2;
    TImage *imright1;
    TImage *imright2;
    TMainMenu *MainMenu1;
    TMenuItem *N1;
    TMenuItem *N3;
    TMenuItem *N4;
    TMenuItem *N5;
    TMenuItem *N6;
    TMenuItem *N7;
    TMenuItem *N8;
    TMenuItem *N9;
    TTimer *timer;
    TImage *imgai;
    TImage *imbiankuang;
    TImage *imanniu;
    TMenuItem *N10;
    TButton *start;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TMenuItem *N11;
    TLabel *Label5;
    TLabel *Label6;
    TLabel *Label7;
    TMenuItem *N2;
    TMenuItem *N12;
    TMenuItem *N13;
    TMenuItem *N14;
    TMenuItem *N15;
    TLabel *Label8;
    TImage *imdownming1o;
    TImage *imdownming2o;
    TImage *imdownan2o;
    TImage *imdownan1o;
    TMediaPlayer *sound;
    TMenuItem *N16;
    void __fastcall PaintBox1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall PaintBox1Paint(TObject *Sender);
    void __fastcall timerTimer(TObject *Sender);
    void __fastcall PaintBox1DblClick(TObject *Sender);
    void __fastcall startClick(TObject *Sender);
    void __fastcall N3Click(TObject *Sender);
    void __fastcall N4Click(TObject *Sender);
    void __fastcall N2Click(TObject *Sender);
    void __fastcall N5Click(TObject *Sender);
    void __fastcall N10Click(TObject *Sender);
    void __fastcall N11Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall N12Click(TObject *Sender);
    void __fastcall N6Click(TObject *Sender);
    void __fastcall N8Click(TObject *Sender);
    void __fastcall N16Click(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:

        __fastcall TForm1(TComponent* Owner);

        Graphics::TBitmap *imdownming1,*imdownming2,*imdownan1,*imdownan2;
        void leftclick(int X,int Y);
        void rightclick(int X,int Y);   //鼠标处理函数
        void timerfun();//计时器处理函数


  /////-----------------------------------------
    void clearform(); //清除整个界面
    void clearform(TRect);//清除指定界面
    void clearmjot(); //清除桌面上没有取过的暗牌,这些分成4块所以要这样另声明函数
    void clearanddrawname();    

    enum btstat{btNo=0x0,btChu=0x1,btPeng=0x2,btGang=0x4,btHu=0x8};
    btstat playerPGflag;//玩家右下脚按钮显示状态
    void drawbt(btstat bs); //标志吃碰和...按钮要显示那些,可用 | 操作符连接


    //===================标志位====================
    int imagetype;//使用图片 1 棕色 2 绿色
    void setbkcolor();//用imagetype确定选择背景颜色

    bool m_sound;

    AnsiString name[4];////姓名 0下 1左 2上 3右

    int upsc,downsc,rightsc,leftsc;//分数

    int zhuangjia;//庄家   0下 1左 2上 3右

    unsigned int gamespeed;//毫秒
    int difficult;//难度   3难 2中 1易

    int paishu[25];//各种牌的 得分
    int avepaishu;//上 数组的平均值,在man::discard中判断3大方向时使用
    
    float moneyperpai;//每牌兑换人民币,<0表示未设置

    enum {UpWaitForPG,/*碰杠*/UpWaitForMo,UpWaitForChu,UpWaitForBu,/*补牌,包括自己摸到1张与手上3张组成暗杠*/UpWaitForHu ,
                  LeftWaitForPG,LeftWaitForMo,LeftWaitForChu,LeftWaitForBu,LeftWaitForHu ,
                  RightWaitForPG,RightWaitForMo,RightWaitForChu,RightWaitForBu,RightWaitForHu ,
                  PlayerWaitForSelectBtn,PlayerWaitForChu,
                  fapai,/*发牌*/WaitForHuangZhuang,/*荒庄*/

    }gamestat,laststat;
    //=================进行时标志位================
    int flag_pg;  //1碰2杠
    //int laststat; // PlayerWaitForSelectBtn的时候取消碰杠时主动权交给laststat
    bool flag_bugang;//处理鼠标点击'杠'时判断是别人打出的还是自己摸到的,进行不同操作
    bool flag_zimo; //和牌判断自摸,也通过此,来进行不通操作,(是从桌上抓别人的牌,还是不抓)
    int loser;//有人和的时候使用
    bool gang;//有人杠牌,此为真,到下一人摸牌(碰杠),置为假

    int fapai_dir,fapai_num;//发牌时用的标志

    bool begin;//游戏正在进行
    int numofgames;//盘数

    bool downplayerwanttogang;//当别的电脑要和,而玩家选了杠,则true,用于判断抢杠和

    //===============================================
    computerleft leftcom; //3个电脑和1玩家
    computerup upcom;
    computerright rightcom;
    player downplayer;

    //man *currentplayer;//当前玩家

    mjontable mjot;  //桌面上放着的未取过的牌

    mjontableshowed  mjots;   //桌面已经亮出的牌


    
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
extern void soundpeng();
extern void soundgang();
extern void soundhu();
//---------------------------------------------------------------------------
#endif
