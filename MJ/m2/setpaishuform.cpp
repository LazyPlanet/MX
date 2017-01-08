//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "setpaishuform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tsetpaishu *setpaishu;
//---------------------------------------------------------------------------
__fastcall Tsetpaishu::Tsetpaishu(TComponent* Owner,int a[],int &aveps,float &m)
    : TForm(Owner),money(m),paishu(a) ,avepaishu(aveps)
{
}
//---------------------------------------------------------------------------

void __fastcall Tsetpaishu::CheckBox1Click(TObject *Sender)
{
    if(CheckBox1->Checked)
    {
        Label1->Visible=true;
        Label2->Visible=true;
        MaskEdit16->Visible=true;
    }
    else
    {
        Label1->Visible=false;
        Label2->Visible=false;
        MaskEdit16->Visible=false;
    }
            
}
//---------------------------------------------------------------------------


void __fastcall Tsetpaishu::FormCreate(TObject *Sender)
{
    ComboBox1->ItemIndex=paishu[2]/5-1;
    ComboBox2->ItemIndex=paishu[4]/5-1;
    ComboBox3->ItemIndex=paishu[0]/5-1;
    ComboBox4->ItemIndex=paishu[6]/5-1;
    ComboBox5->ItemIndex=paishu[8]/5-1;

    MaskEdit1->Text=paishu[12];
    MaskEdit2->Text=paishu[13];
    MaskEdit3->Text=paishu[14];
    MaskEdit4->Text=paishu[15];
    MaskEdit5->Text=paishu[17];

    MaskEdit6->Text=paishu[19];
    MaskEdit7->Text=paishu[21];
    MaskEdit8->Text=paishu[10];
    MaskEdit9->Text=paishu[18];
    MaskEdit10->Text=paishu[20];

    MaskEdit11->Text=paishu[22];
    MaskEdit12->Text=paishu[11];
    MaskEdit13->Text=paishu[16];
    MaskEdit14->Text=paishu[24];
    MaskEdit15->Text=paishu[23];

    if(money>0)
    {
        Label1->Visible=true;
        Label2->Visible=true;
        MaskEdit16->Visible=true;

        AnsiString s(money);
        MaskEdit16->Text=(s+"000").SubString(1,s.Pos(".")+2);;

        CheckBox1->Checked=true;
    }
}
//---------------------------------------------------------------------------

void __fastcall Tsetpaishu::Button1Click(TObject *Sender)
{
    paishu[2]=(ComboBox1->ItemIndex+1)*5;
    paishu[4]=(ComboBox2->ItemIndex+1)*5;
    paishu[0]=(ComboBox3->ItemIndex+1)*5;
    paishu[6]=(ComboBox4->ItemIndex+1)*5;
    paishu[8]=(ComboBox5->ItemIndex+1)*5;

    //MaskEdit1->Text=paishu[12];
    paishu[12]=MaskEdit1->Text.Trim().ToInt();
    paishu[13]=MaskEdit2->Text.Trim().ToInt();
    paishu[14]=MaskEdit3->Text.Trim().ToInt();
    paishu[15]=MaskEdit4->Text.Trim().ToInt();
    paishu[16]=MaskEdit5->Text.Trim().ToInt();

    paishu[19]=MaskEdit6->Text.Trim().ToInt();
    paishu[21]=MaskEdit7->Text.Trim().ToInt();
    paishu[10]=MaskEdit8->Text.Trim().ToInt();
    paishu[18]=MaskEdit9->Text.Trim().ToInt();
    paishu[20]=MaskEdit10->Text.Trim().ToInt();

    paishu[22]=MaskEdit11->Text.Trim().ToInt();
    paishu[11]=MaskEdit12->Text.Trim().ToInt();
    paishu[16]=MaskEdit13->Text.Trim().ToInt();
    paishu[24]=MaskEdit14->Text.Trim().ToInt();
    paishu[23]=MaskEdit15->Text.Trim().ToInt();
    
    avepaishu=0;
    for(int i=0;i<25;i++)
        avepaishu+=paishu[i];
    avepaishu/=25;

    AnsiString s;
    char *c;
    //int p=MaskEdit16->Text.Pos(" ");

    if(CheckBox1->Checked&&!MaskEdit16->Text.Trim().IsEmpty())
    {
        s=MaskEdit16->Text;
        c=s.c_str();
        while(*c)
        {
            if(*c==' ')
                *c='0';
            c++;
        }
        money=(float)s.ToDouble();
    }
    else
        money=-1.0;
    Close();
}
//---------------------------------------------------------------------------

