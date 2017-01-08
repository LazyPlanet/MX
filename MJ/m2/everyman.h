#ifndef everymanH
#define everymanH

#include"man.h"

//
//3个电脑类 和 玩家类 的 声明文件
// 还包括桌面上未取过的牌堆类mjontable,
//和一打过的牌堆类mjontableshowed

class player:public man
{
    int selected;
public:

    player();

    virtual void draw(int image)const;
    virtual TRect picrect(mjpai mj)const; //mj=100表示暗杠的那张按着的牌
    virtual void resetbegin();

    virtual void chu();  //无需使用此函数

    void sort();//排序
    void sortall();//排序,包括painewget一起排序,仅右键排序时有用
    bool canpeng()const;//测试是否可碰可杠,如果可以,就停止timer点亮按钮
    bool cangang()const;
    bool canbugang()const;
    bool canting(int index)const;//看看选择了一张牌准备出后,剩下的是不是听.
    bool canhu(bool iszimo)const;//看看别人打的牌或者自己摸的牌能不能构成和
    //用于点亮按钮用,iszimo===是自摸 .

    int getindex(int mousex,int mousey)const;//返回鼠标点击出是第几张牌
    //返回101-105如果是点击按钮;两者都不是就返回100
    //如果是新摸的一张 50

    void select(int s,int image);//鼠标选择一牌
    //s=50表示选择了painewget
    //出完置为100
};


class computerright:public man
{
public:
    computerright(){setbeginpos(687,507);}  //修改此处需再修改resetbegin()
    virtual void resetbegin();
    virtual void draw(int image)const;
    virtual TRect picrect(mjpai mj)const;//100表示暗杠那张,200表示立着的
        //200时,image应在对方找
    virtual void chu();
};
class computerup:public man
{
public:
    computerup(){setbeginpos(602,33);}//修改此处需再修改resetbegin()
    virtual void resetbegin();//
    virtual void draw(int image)const;
    virtual TRect picrect(mjpai mj)const;//100表示暗杠那张,200表示立着的
    virtual void chu();
};
class computerleft:public man
{
public:
    computerleft(){setbeginpos(65,33);}//修改此处需再修改resetbegin()
    virtual void resetbegin();
    virtual void draw(int image)const;
    virtual TRect picrect(mjpai mj)const;//100表示暗杠那张,200表示立着的
    virtual void chu();
};

class mjontable:private mjpaizu//桌上没摸的麻将堆      takemj(0)从头
{
//public:
    int firstpos;     //第一张方位
    int firstpai;     //第一张序数,顺时针数,头的那张
    int takenfromhead; //从头去了几张?
    int takenfromtail;  //从尾(补牌)


    int bxdown,bydown,bxup,byup,           //四个方向的起始作图点
        bxleft,byleft,bxright,byright;


    void drawonedirect(int image,int from,int to,int direct)const;
    //画某个方向的图,direct取{up,right,down,left}
    //from to 从0到29
    //image  1棕色  2绿色
    void setdrawpos(int xdown,int ydown,int xup,int yup,int xleft,int yleft,int xright,int yright)
        {bxdown=xdown;bydown=ydown;bxup=xup;byup=yup;
        bxleft=xleft;byleft=yleft;bxright=xright;byright=yright;}
        //设置4个方向的起始画图位置
    TRect picrect(int direct,int color)const;//返回imgai中图片位置
        //direct = 1 横向,2 纵向
        //color = 1 棕色,2 绿色
        //
        
public:
    mjontable():mjpaizu(120),firstpos(0),firstpai(0),takenfromhead(0),
        takenfromtail(0){
        setdrawpos(497,412,288,88,238,359,537,148);//默认位置
    }
    enum {up=0,right=1,down=2,left=3};
    void reset();//重置,120张.按顺序,不洗牌
    void wash();//洗牌,
    void setfirst(int pos,int num);//设置第一张牌抓取的位置

    void draw(int image)const;//画
    mjpai takeone(bool fromhead=true);//取一张


    mjpaizu::getcount;

    


};

class mjontableshowed //桌上已亮出的牌,包括玩家手上亮过的
{
    mjpaizu showedinhand;//为了find的使用,画图时不画这部分

    char posmj1[10][8];//桌上能放10*8个麻将 ,0表示没有,
    char posmj2[8][5];//第二层8*5

    bool flag_firstlayerused;
    int newestmj[2];//存放最新的一张麻将的坐标,结合
    //flag_firstlayerused判断是第几层
    //初始化为100

    int count;



    TRect picrect(mjpai mj) const ;
    void sound(TForm1 *Form1,mjpai mj)const;

public:
    mjontableshowed();    //
    
    void insertmjup(mjpai mj);  //
    void insertmjdown(mjpai mj); //
    void insertmjleft(mjpai mj); //
    void insertmjright(mjpai mj); //

    void insertshowedinhand(mjpai mj);//为了find的使用,画图时不画这部分

    void clear();  //
    int getcount()const{return showedinhand.getcount()+count;}   //

    void draw(int image)const;

    int find(mjpai mj)const;//在showedinhand  和posmj2里找,返回个数 无返回0

    mjpai takenewest();
    mjpai getnewest()const;


};




#endif


