//---------------------------------------------------------------------------

#ifndef huformH
#define huformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "man.h"
//---------------------------------------------------------------------------
class man;
class Thupai : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TButton *Button1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
private:	// User declarations

    man &up,&down,&left,&right;

    man *winner;
    bool qiangganghu,iszimo,gangshangkaihua;//抢杠,自摸,杠上开花
    int win,lose,master;//赢家,点炮,0下 1左 2上 3右
    AnsiString *playername;

    int image;

    int *paishu;

public:		// User declarations           
    __fastcall Thupai(TComponent* Owner,man &dow,man &le,man &u,man &ri,bool gskh,
        bool qianggang,bool zimo,int w,int l,int mst,AnsiString *n,int *ps,int im);


    void calculate(int *);
private:
    void drawwinner()const;
};
//---------------------------------------------------------------------------
extern PACKAGE Thupai *hupaiform;
//---------------------------------------------------------------------------
#endif
