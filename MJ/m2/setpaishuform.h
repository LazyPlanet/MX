//---------------------------------------------------------------------------

#ifndef setpaishuformH
#define setpaishuformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class Tsetpaishu : public TForm
{
__published:	// IDE-managed Components
    TComboBox *ComboBox1;
    TCheckBox *CheckBox1;
    TLabel *Label1;
    TLabel *Label2;
    TButton *Button1;
    TLabel *Label3;
    TLabel *Label4;
    TComboBox *ComboBox2;
    TLabel *Label5;
    TComboBox *ComboBox3;
    TLabel *Label6;
    TComboBox *ComboBox4;
    TLabel *Label7;
    TComboBox *ComboBox5;
    TLabel *Label8;
    TLabel *Label9;
    TLabel *Label10;
    TLabel *Label11;
    TLabel *Label12;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *Label17;
    TLabel *Label18;
    TLabel *Label19;
    TLabel *Label20;
    TLabel *Label21;
    TLabel *Label22;
    TMaskEdit *MaskEdit1;
    TMaskEdit *MaskEdit2;
    TMaskEdit *MaskEdit3;
    TMaskEdit *MaskEdit4;
    TMaskEdit *MaskEdit5;
    TMaskEdit *MaskEdit6;
    TMaskEdit *MaskEdit7;
    TMaskEdit *MaskEdit8;
    TMaskEdit *MaskEdit9;
    TMaskEdit *MaskEdit10;
    TMaskEdit *MaskEdit11;
    TMaskEdit *MaskEdit12;
    TMaskEdit *MaskEdit13;
    TMaskEdit *MaskEdit14;
    TMaskEdit *MaskEdit15;
    TMaskEdit *MaskEdit16;
    void __fastcall CheckBox1Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations

    int *paishu;
    int &avepaishu;
    float &money;
public:		// User declarations
    __fastcall Tsetpaishu(TComponent* Owner,int *a,int &aveps,float &m);
};
//---------------------------------------------------------------------------
extern PACKAGE Tsetpaishu *setpaishu;
//---------------------------------------------------------------------------
#endif
