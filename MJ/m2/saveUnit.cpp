//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "saveUnit.h"
#include <Registry.hpp>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TsaveForm *saveForm;
//---------------------------------------------------------------------------
__fastcall TsaveForm::TsaveForm(TComponent* Owner,
        int *i_paishu,float &mpp,AnsiString *n,int &ds,int &ls,int &us,int &rs,int &zj,
        int &nog,unsigned int &gs,int &diff,int &it)
    : TForm(Owner),paishu(i_paishu),moneyperpai(mpp),name(n),downsc(ds),
        leftsc(ls),upsc(us),rightsc(rs),zhuangjia(zj),gamespeed(gs),difficult(diff),
        numofgames(nog),imagetype(it)
{
}
//---------------------------------------------------------------------------
void __fastcall TsaveForm::Button1Click(TObject *Sender)
{
    TRegistry *reg = new TRegistry;

    reg->OpenKey("\\Software\\zhanghao's jjmj",true);


    reg->WriteBool("cb1",CheckBox1->Checked);
    reg->WriteBool("cb2",CheckBox2->Checked);
    reg->WriteBool("cb3",CheckBox3->Checked);

    if(CheckBox1->Checked)
    {
        for(int i=0;i<25;i++)
            reg->WriteInteger(AnsiString("paishu")+i,paishu[i]);

        reg->WriteFloat("moneyperpai",moneyperpai);
    }

    if(CheckBox2->Checked)
    {
        for(int i=0;i<4;i++)
            reg->WriteString(AnsiString("name")+i,name[i]);

        reg->WriteInteger("downsc",downsc);
        reg->WriteInteger("leftsc",leftsc);
        reg->WriteInteger("upsc",upsc);
        reg->WriteInteger("rightsc",rightsc);
        reg->WriteInteger("zhuangjia",zhuangjia);
        reg->WriteInteger("numofgames",numofgames);
    }

    if(CheckBox3->Checked)
    {
        reg->WriteInteger("gamespeed",int(gamespeed));
        reg->WriteInteger("difficult",difficult);
        reg->WriteInteger("imagetype",imagetype);
        //reg->WriteBool("music",music);
        //reg->WriteBool("bigimage",bigimage);
    }

    reg->CloseKey();

    Close();
}
//---------------------------------------------------------------------------


