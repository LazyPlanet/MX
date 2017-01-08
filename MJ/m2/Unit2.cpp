//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"
#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TForm2::Button1Click(TObject *Sender)
{
    *(Form1->mjot.begin())=Edit1->Text.ToInt();
    //Form1->mjot.mja_mj[Form1->mjot.count-1]=Edit1->Text.ToInt();

    Close();
}
//---------------------------------------------------------------------------
 