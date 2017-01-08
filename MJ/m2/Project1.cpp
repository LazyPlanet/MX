//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("Project1.res");
USEFORM("Unit1.cpp", Form1);
USEUNIT("mjPai.cpp");
USEUNIT("MISHEL.CPP");
USEUNIT("mjpaizu.cpp");
USEUNIT("mjpaikuaizu.cpp");
USEUNIT("man.cpp");
USEUNIT("everyman.cpp");
USEFORM("Unit2.cpp", Form2);
USEFORM("huform.cpp", hupai);
USEFORM("setpaishuform.cpp", setpaishu);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TForm1), &Form1);
         Application->CreateForm(__classid(Tsetpaishu), &setpaishu);
         Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
