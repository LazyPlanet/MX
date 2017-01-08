//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "speedform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tspeedset *speedset;
//---------------------------------------------------------------------------
__fastcall Tspeedset::Tspeedset(TComponent* Owner,unsigned int &s)
    : TForm(Owner),speed(s)
{
}
//---------------------------------------------------------------------------

void __fastcall Tspeedset::FormCreate(TObject *Sender)
{
    switch(speed)
    {
    case 500:
        RadioButton1->Checked=true;
        break;
    case 2000:
        RadioButton3->Checked=true;
        break;
    default:
        RadioButton2->Checked=true;
        break;

    }
}
//---------------------------------------------------------------------------

void __fastcall Tspeedset::FormClose(TObject *Sender, TCloseAction &Action)
{
    if(RadioButton1->Checked==true) speed=500;
    if(RadioButton2->Checked==true) speed=1000;
    if(RadioButton3->Checked==true) speed=2000;

}
//---------------------------------------------------------------------------
