#ifndef mjpaiH
#define mjpaiH

//#define NDEBUG   //不要assert
//#define NLABEL    //桌面上不显示电脑状态
#include <assert.h>

#include <Graphics.hpp>

//2003.2-4 九江麻将 ---张浩
//
//声明了mjpai类和一些属性,如转换到int的转换操作
//判断万,条,饼 的函数
//
//还有一些算法,--判断牌堆是否和,七对,碰碰和等
class mjpai{
    int mj;

public:
    enum mjcode{
        wan1=11,wan2,wan3,wan4,wan5,wan6,wan7,wan8,wan9,
        tiao1=31,tiao2,tiao3,tiao4,tiao5,tiao6,tiao7,tiao8,tiao9,
        bing1=51,bing2,bing3,bing4,bing5,bing6,bing7,bing8,bing9,
        zhong=71,fa=81,bai=91
    };
    mjpai(int im=0){mj=im;}
    //mjpai(){mj=0;}


    bool iswan()const;//万
    bool istiao()const;//条
    bool isbing()const;//饼
    bool iszi()const; //字
    friend int comparemj(const mjpai m1,const mjpai m2);//-1...-8前者小于后者
            //1...8前者大于后者
            //0相等,100非同色,中与发返回50,中与中返回0

    operator int()const{return mj;}//转换操作符
};

bool isonecolor(const mjpai *pmj_arr,const int i_num);
//判断一堆麻将是否同色 ,全字为假.遇到发财跳过,不理会 ,有发财也可以返回1
bool isonecolororzi(const mjpai *pmj_arr,const int i_num);
//同色和含字,全字也返回真
int pengandgang(const mjpai *pmj_arr,const int i_num,const mjpai mj);//碰 1,杠 2,无0
//int chi(const mjpai *pmj_arr,const int i_num,const mjpai mj);//吃,1万 与2,3万,返回100
                                                                //2与1,3 返回10
                                                                //3与1,2 返回1
                                                                //多种都行返回 和
                                                                //无 0
bool hupai(const mjpai *pmj_arr,const int i_num);//看牌是否成胡牌牌型,
    //包括七对.不检测牌数是否为14,如果不为14,则不会检测七对
    //i_num必须小等于14  3n+2
int hu(const mjpai *pmj_arr,int i_num,const mjpai mj);//
    //i_num必须小等于13   3n+1

bool duting(const mjpai *pmj_arr,const int i_num);//独听,听一张牌   i_num=3n+1 ,确保和
int qidui(const mjpai *pmj_arr,const int i_num);//七对 i_num=14
                                                //非14即非七对返回0,七对返回1
                                                //一个潇洒+100  一个豪华+10
                                                //如 豪华七对 11  双豪华 21
                                                //   七对 1     双潇洒 201
                                                //须保证 相同牌最多4张
                                                //计算潇洒时已经排序,但为动过pmj_arr

bool bukao(const mjpai *pmj_arr,int i_num,const mjpai mj);//不靠用于优化听牌



void mishel(mjpai *p,const int n);//排序
void mishel(int *p,int n);//排序
int getwei(int number,int pos);//获得某个数的某位的值(个1,十2,百3)
#endif
