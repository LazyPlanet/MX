#ifndef mjpaikuaizuH
#define mjpaikuaizuH
#include "mjpaizu.h"

//2003.2-4 九江麻将 ---张浩
//
//麻将块组是把麻将按亮出的方式存储的,可以为
//顺,刻,杠,发
//
class mjpaikuaizu:private mjpaizu    //亮出的一组牌  发财单独放
{
    int i_numofkuai;   //块数
    int i_beginofkuai[8]; //块头的位置,最多只有8个块(吃碰杠四次,4个发财)
    bool isangang[8];//是暗杠? 


public:
    mjpaikuaizu():mjpaizu(),i_numofkuai(0){}

    int whatcolor(int i_ikuai)const;//第i_ikuai块是 万 1 条2 饼3 字 4 发5

    bool iske(int i_ikuai)const;//3
    bool isshun(int i_ikuai)const;//3
    int isgang(int i_ikuai)const;//4  明杠返回1,暗杠2,不是0

    void insertmjkuai(mjpai firstmj,int type,bool isangang=false);
    //type-0吃 1碰 2杠 3发 firstmj为最小一张
         //将一块插入(1-4张),  isangang是否是暗杠,默认false
         //必须保证发是一个个输入,程序不检测(只有assert)
    void deletemjkuai(int i);
    //int findmjkuai(mjpai firstmj,int type)const;//没用暂时不做
    int getkuaicount()const {return i_numofkuai;}
    void clear(){i_numofkuai=0;mjpaizu::clear();}

    mjpaikuaizu & operator =(mjpaikuaizu &s);
    mjpaikuaizu(mjpaikuaizu &s);

    void bugang(mjpai mj,bool isangang=false);//补杠  assert  此刻存在
    int havezikeorgang()const;//有无字刻、杠  刻+1，杠+10 ,暗杠+100,无0
    int havegang()const;//有无普通杠,不包括字杠,明杠+1暗杠+10
    int havefa()const;//有无发,一个+1

    int findke(mjpai mj)const;//为了补杠，需要查找刻，无返回100

    mjpaizu::getmj;
    mjpaizu::getcount;
    mjpai getfirstofkuai(int kuai)const;  //碰碰和 的潇洒 要用

};

#endif