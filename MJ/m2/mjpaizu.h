#ifndef mjpaizuH
#define mjpaizuH
#include "mjpai.h"
#include <assert.h>

//2003.2-4 九江麻将 ---张浩
//
//用stl vector 实现的 麻将组,应用于整个程序
//
//
//

#include <vector>
using namespace std;

//以上是自己写的,下面是用c++ stl库重新写过
class mjpaizu//:public vector<mjpai>  //麻将牌组
{
    //mjpaizu(mjpaizu &s);
    vector<mjpai> m_mjzu;
protected:
    inline void reverse(int capa){m_mjzu.reserve(capa);}
public:
    mjpaizu(mjpai *pmj_arr,int i_num);
    inline mjpaizu(){reverse(22);}
    inline mjpaizu(int capa){reverse(capa);}
    mjpai getmj(int i)const;//如果i过大,返回0 .i从0开始
    mjpai takemj(int i);// take与get不同:take从组中删了
    int find(mjpai mj)const; //返回第一个的下标 ,没找到返回100
    void insertmj(mjpai mj); //插入尾部
    inline int getcount()const{return m_mjzu.size();}//返回麻将牌数

    void clear();

    void sort();//排序

    //mjpaizu & operator =(mjpaizu &s);//赋值

    friend class TForm2;//test
    inline vector<mjpai>::iterator begin(){return m_mjzu.begin();}
};

mjpaizu ting(const mjpai *pmj_arr,int i_num);//听多少牌?


#endif