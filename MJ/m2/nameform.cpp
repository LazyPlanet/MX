//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "nameform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tnameset *nameset;
//---------------------------------------------------------------------------
__fastcall Tnameset::Tnameset(TComponent* Owner,AnsiString*a,int &d,int &l,int &u,int &r,float m)
    : TForm(Owner),name(a),upsc(u),leftsc(l),downsc(d),rightsc(r),moneyperpai(m)
{
}
//---------------------------------------------------------------------------
void __fastcall Tnameset::RadioButton1Click(TObject *Sender)
{
    RadioButton1->Checked=true;
    MaskEdit1->Enabled=true;

    RadioButton2->Checked=false;
    MaskEdit2->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall Tnameset::RadioButton2Click(TObject *Sender)
{
    RadioButton1->Checked=false;
    MaskEdit1->Enabled=false;

    RadioButton2->Checked=true;
    MaskEdit2->Enabled=true;

}
//---------------------------------------------------------------------------
void __fastcall Tnameset::FormCreate(TObject *Sender)
{
    Edit1->Text=name[0];
    Edit2->Text=name[1];
    Edit3->Text=name[2];
    Edit4->Text=name[3];

    MaskEdit1->Text=AnsiString(100.0);
}
//---------------------------------------------------------------------------
void __fastcall Tnameset::Button1Click(TObject *Sender)
{
    if(!Edit1->Text.IsEmpty()) name[0]=Edit1->Text;
    if(!Edit2->Text.IsEmpty()) name[1]=Edit2->Text;
    if(!Edit3->Text.IsEmpty()) name[2]=Edit3->Text;
    if(!Edit4->Text.IsEmpty()) name[3]=Edit4->Text;

    bool flag=false;
    AnsiString s;
    char *c;
    
    if(RadioButton1->Checked==true)//&&!MaskEdit1->Text.Trim().IsEmpty())
    {
        flag=true;
        s=MaskEdit1->Text;
        c=s.c_str();
        while(*c)
        {
            if(*c==' ')
                *c='0';
            c++;
        }
        upsc=downsc=leftsc=rightsc=s.ToInt();
    }

    if(RadioButton2->Checked==true)//&&!MaskEdit2->Text.Trim().IsEmpty())
    {
        flag=true;
        s=MaskEdit2->Text;
        c=s.c_str();
        while(*c)
        {
            if(*c==' ')
                *c='0';
            c++;
        }
        if(moneyperpai>0.0)upsc=downsc=leftsc=rightsc=int(s.ToDouble()/moneyperpai);
    }

    if(!flag)
        Application->MessageBox("牌数或人民币设置不正确",NULL,MB_OK);
    else
        Close();

}
//---------------------------------------------------------------------------
