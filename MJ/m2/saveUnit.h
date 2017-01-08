//---------------------------------------------------------------------------

#ifndef saveUnitH
#define saveUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TsaveForm : public TForm
{
__published:	// IDE-managed Components
    TCheckBox *CheckBox1;
    TCheckBox *CheckBox2;
    TButton *Button1;
    TCheckBox *CheckBox3;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
    int *paishu;   //牌数状态
    float &moneyperpai;

    AnsiString *name;    //玩家状态
    int &upsc,&downsc,&rightsc,&leftsc;
    int &zhuangjia;
    int &numofgames;

    unsigned int &gamespeed; //桌面状态
    int difficult;
    int &imagetype;
    //bool music;
    //bool bigimage ;
public:		// User declarations
    __fastcall TsaveForm(TComponent* Owner,
        int *i_paishu,float &mpp,AnsiString *n,int &ds,int &ls,int &us,int &rs,int &zj,
        int &nog,unsigned int &gs,int &diff,int &it);
};
//---------------------------------------------------------------------------
extern PACKAGE TsaveForm *saveForm;
//---------------------------------------------------------------------------
#endif
