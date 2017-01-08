#ifndef manH
#define manH


#include "mjpai.h"
#include "mjpaizu.h"
#include "mjpaikuaizu.h"
#include "huform.h"

//
//man 类的头文件
//man为电脑类和玩家类的父类
//
//
//
//
class man
{
protected:
    mjpaikuaizu zushowed;
    mjpaizu zuhand;
    mjpai painewget; //无牌在此位 为0

    int beginx,beginy; //起始作图位置
        //右边电脑为 最下方一子的左上角
        //上边    最右边一子左上角
        //...

    bool isting;//已经听牌了
    bool fainhandhavedetected;//zuhand中的发只检测一次,如果检测过,就为真,下次
    //摸,碰,杠,补杠时就不再检测了


    mjpai discard();//电脑选择出那一张牌
    void setbeginpos(int bx,int by){beginx=bx;beginy=by;}



    enum {yise,pengpeng,dui,nothing} wantto; //几个大的方向,在discard函数中
    enum {wan,tiao,bing} se;

    int numofdui()const;//对的个数
    int numofse(int c)const;//1色的个数  enum {wan,tiao,bing}
    mjpaizu yise_(const mjpai * mj,int c,mjpai newget)const;
    mjpaizu dui_(const mjpai * mj,int c,mjpai newget)const;
    mjpaizu pengpeng_(const mjpai * mj,int c,mjpai newget)const;
    //返回几张可以出的牌,(取出个数为1的,如果没有就个数为2......)

public:
    man(){painewget=0;isting=false;wantto=nothing;isting=false;}

    virtual void resetbegin()=0; //根据麻将数重新排列,draw前要用
    virtual void draw(int image)const=0;
    virtual TRect picrect(mjpai mj)const=0;//返回麻将在图片中的位置

    void clear()
        {zushowed.clear();zuhand.clear();painewget=0;isting=false;
        wantto=nothing;fainhandhavedetected=false;}

    void insertmj(mjpai mj); //用于一开始分牌的时候,在zuhand中插入
                           //返回0如果插入发
    int getcount()const{return zushowed.getcount()+zuhand.getcount()+((painewget==0)?0:1);}

    bool wanttopeng()const;  //player不用此3函数
    bool wanttogang()const;  //想不想碰,杠? 到mjontableshowed取最新一张判断
    bool wanttobugang()const;//把painewget放到zushowed的刻里,或者将手上4张放到刻里,再摸一张
    bool wanttohu(bool iszimo);//询问要不要和,

    void peng();  //碰杠的动作,到mjontableshowed和mjontable取牌
    void gang();
    void bugang();

    virtual void chu()=0;//出牌
    bool mo();//摸牌,返回值表示有没有改变位置,即有没有补过发财



    //------------------------------------------------------和牌后使用的函数
    bool hu()const;//胡了？
    //看牌是否成胡牌牌型,手上3n+1张,摸1张
    //包括七对.不检测牌数是否为14,如果不为14,则不会检测七对
    //
    bool duting()const;//独听?//手上3n+1张
    int pengpenghu()const;//碰碰和?  潇洒 返回2
    bool qingyise()const;//清一色?   不记混一色
    bool hunyise()const;//混一色?  包括清一色,可以先用清一色检测一下,不是,再用混一色
    //检测,确保是混一色
    int qidui()const;//七对?
                                                //非门清及非七对返回0,七对返回1
                                                //一个潇洒+100  一个豪华+10
                                                //如 豪华七对 11  双豪华 21
                                                //   七对 1     双潇洒 201
                                                //须保证 相同牌最多4张
                                                //计算潇洒时已经排序,但为动过pmj_arr
    bool yitiaolong()const;//一条龙
    int  hongqipiaopiao()const;//红旗飘飘,应确保和,明1,暗2,无0     不记字刻
    bool menqing()const;//门清?
    int fa()const;//发?  程序中应保证每次摸到发,自动放到showed里
    int zimingke()const;//字明刻? 红旗飘飘则不记
    int zianke()const;//字暗刻?  红旗飘飘则不记
    int ziminggang()const;//字明杠?
    int ziangang()const;//字暗杠?
    int minggang()const;//普通明杠?
    int angang()const;//普通暗杠?


    //=-----------------------------------



    void sort(){zuhand.sort();};//test
    friend class TForm1;
    friend class Thupai;
};



#endif