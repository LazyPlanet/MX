//---------------------------------------------------------------------------

#ifndef nameformH
#define nameformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class Tnameset : public TForm
{
__published:	// IDE-managed Components
    TRadioButton *RadioButton1;
    TRadioButton *RadioButton2;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TLabel *Label5;
    TEdit *Edit1;
    TEdit *Edit2;
    TEdit *Edit3;
    TEdit *Edit4;
    TButton *Button1;
    TLabel *Label6;
    TMaskEdit *MaskEdit1;
    TMaskEdit *MaskEdit2;
    void __fastcall RadioButton1Click(TObject *Sender);
    void __fastcall RadioButton2Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
    AnsiString *name;
    int &upsc,&downsc,&rightsc,&leftsc;

    float moneyperpai;
public:		// User declarations
    __fastcall Tnameset(TComponent* Owner,AnsiString*,int &d,int &l,int &u,int &r,float m);
};
//---------------------------------------------------------------------------
extern PACKAGE Tnameset *nameset;
//---------------------------------------------------------------------------
#endif
