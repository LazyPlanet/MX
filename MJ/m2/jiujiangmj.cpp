//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("jiujiangmj.res");
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
USEFORM("speedform.cpp", speedset);
USEFORM("colorform.cpp", colorset);
USEFORM("nameform.cpp", nameset);
USEUNIT("telligent.cpp");
USEFORM("saveUnit.cpp", saveForm);
USEFORM("diffUnit.cpp", difficult);
USEFORM("aboutmj.cpp", Form3);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "中国麻将";
         Application->CreateForm(__classid(TForm1), &Form1);
         Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
