//---------------------------------------------------------------------------

#ifndef speedformH
#define speedformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class Tspeedset : public TForm
{
__published:	// IDE-managed Components
    TRadioButton *RadioButton1;
    TRadioButton *RadioButton2;
    TRadioButton *RadioButton3;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
    unsigned int &speed;
public:		// User declarations
    __fastcall Tspeedset(TComponent* Owner,unsigned int &s);
};
//---------------------------------------------------------------------------
extern PACKAGE Tspeedset *speedset;
//---------------------------------------------------------------------------
#endif
