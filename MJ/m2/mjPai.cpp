#include"mjpai.h"
#include <math.h>

int comparemj(const mjpai m1,const mjpai m2){
    assert(m1!=0&&m2!=0);
    int a,b,c;

    a=m1.mj; b=m2.mj;  c=a-b;

    if(a==b) return 0;
    if(m1.iszi()&&m2.iszi())return 50;
    if(c>8||c<-8)
        return 100;
    else return c;
}

bool mjpai::iswan()const
{assert(mj);return (mj>=11&&mj<=19)?true:false;}

bool mjpai::istiao()const
{assert(mj);return (mj>=31&&mj<=39)?true:false;}

bool mjpai::isbing()const
{assert(mj);return (mj>=51&&mj<=59)?true:false;}

bool mjpai::iszi()const
{assert(mj);return (mj>=71&&mj<=91)?true:false;}

bool isonecolor(const mjpai *pmj_arr,const int i_num){
    int i_flag=0;
    int i_cmp;

    int i_firstnotfa=0;
    while(pmj_arr[i_firstnotfa]==mjpai(mjpai::fa)) i_firstnotfa++;

    if(pmj_arr[i_firstnotfa].iszi()) return false;
    for(int i=1;i<i_num;i++){
        if(pmj_arr[i]==mjpai(mjpai::fa))continue;
        i_cmp=comparemj(pmj_arr[i],pmj_arr[i_firstnotfa]);
        if(i_cmp==100){
            i_flag=1;
            break;
        }
    }

    return i_flag?false:true;
}
bool isonecolororzi(const mjpai *pmj_arr,const int i_num){
    int i_flag=0;
    int i,i_cmp;
    for(i=0;i<i_num;i++)
        if(!pmj_arr[i].iszi())
            break;

    if(i==i_num)return true;
    int i_firstnotzi=i;

    for(i=0;i<i_num;i++){
        i_cmp=comparemj(pmj_arr[i],pmj_arr[i_firstnotzi]);
        if(i_cmp==100&&!pmj_arr[i].iszi()){
            i_flag=1;
            break;
        }
    }
    return i_flag?false:true;
}

int pengandgang(const mjpai *pmj_arr,const int i_num,const mjpai mj)
{
    int flag[5];//flag[2]记录相同牌个数 3为牌堆中比此牌大一的      
    int i,i_cmp;
    for (i=0;i<5;i++) flag[i]=0;

    for(i=0;i<i_num;i++)
    {
        i_cmp=comparemj(pmj_arr[i],mj);
        if(i_cmp>=-2&&i_cmp<=2)
            flag[i_cmp+2]++;
    }

    if(flag[2]==2)return 1;
    else if(flag[2]==3)return 2;
    else return 0;
}

static bool hupai1(int * pi_mj,int i_num,int *pi_select)
{    //pi_select[]用于在递归调用时记录已经访问过的牌
     int i,j,k;
     int i_max,i_min,i_mid;
     int i_mark1,i_mark2;//,i_sum;

     int i_have3=0;  //int i_i;


     for(i=0;i<i_num;i++){
        if(pi_select[i])continue;
        else i_mark1=i;

        for(j=i+1;j<i_num;j++){
            if(i==j)continue;

            if(pi_select[j]) continue;
            else i_mark2=j;

            for(k=j+1;k<i_num;k++){
                if(i==k||j==k)continue;

                if(pi_select[k])continue;
                i_have3=1;   //说明可以找到3张未选择过的牌,
                //如果最后i_have3==0
                //说明此时只有2张牌了.下面可以用i_mark1,i_mark2获得此二张.

                i_max=0;i_min=100;
                i_max=(pi_mj[i]>i_max)?pi_mj[i]:i_max;
                i_max=(pi_mj[j]>i_max)?pi_mj[j]:i_max;
                i_max=(pi_mj[k]>i_max)?pi_mj[k]:i_max;
                i_min=(pi_mj[k]<i_min)?pi_mj[k]:i_min;
                i_min=(pi_mj[j]<i_min)?pi_mj[j]:i_min;
                i_min=(pi_mj[i]<i_min)?pi_mj[i]:i_min;
                i_mid=(i_max+i_min)/2;

                if(pi_mj[i]==pi_mj[j]&&pi_mj[i]==pi_mj[k])  //刻子
                    ;
                else if(i_max-i_min==2&&(pi_mj[i]==i_mid||pi_mj[j]==i_mid||pi_mj[k]==i_mid))//顺子
                    ;
                else
                    continue;

                pi_select[i]=pi_select[j]=pi_select[k]=1; //记录下来 下面递归

                if(!hupai1(pi_mj,i_num,pi_select)){
                    pi_select[i]=pi_select[j]=pi_select[k]=0; //递归一次结束,返回
                     continue;
                }
                return 1;//选择的3张成刻、顺，而剩下的和了
            }
            //只剩两张牌
            if(!i_have3&&pi_mj[i_mark1]==pi_mj[i_mark2])return 1;
            //只剩两张，相等

        }
      }


      return 0;

}
bool hupai(const mjpai *pmj_arr,const int i_num)
{
    if(i_num>14) return 0;
    if(i_num%3!=2)return 0;//牌数应为3n+2

    int ia_mj[14];
    int i,j;
    for(i=0;i<i_num;i++)
        ia_mj[i]=(mjpai)pmj_arr[i];

    int ia_selected[14];

    for(i=0;i<i_num;i++) ia_selected[i]=0;

    int i_sum=0;
    if(i_num==14){//看看是不是七对
        for(i=0;i<i_num;i++){
            if(ia_selected[i])continue;
            for(j=i+1;j<i_num;j++){
                if(ia_selected[j]||j==i)continue;

                if(ia_mj[i]==ia_mj[j]){
                    ia_selected[i]=ia_selected[j]=1;
                    i_sum++;
                    break;
                }
            }
            if(ia_selected[i]==0)break;
         }
    }
    if(i_sum==7)return true;
    //mishel(ia_mj,i_num);

    //int ia_selected[14];

    for(i=0;i<i_num;i++) ia_selected[i]=0;
    return hupai1(ia_mj,i_num,ia_selected);

}

int hu(const mjpai *pmj_arr, int i_num,const mjpai mj)
{
    if(i_num>13) return 0;
    if(i_num%3!=1)return 0;//牌数应为3n+1

    int ia_mj[14];
    int i,j;
    for(i=0;i<i_num;i++)
        ia_mj[i]=(mjpai)pmj_arr[i];

    ia_mj[i]=(mjpai)mj;
    i_num++;

    int ia_selected[14];

    for(i=0;i<i_num;i++) ia_selected[i]=0;

    int i_sum=0;
    if(i_num==14){//看看是不是七对
        for(i=0;i<i_num;i++){
            if(ia_selected[i])continue;
            for(j=i+1;j<i_num;j++){
                if(ia_selected[j]||j==i)continue;

                if(ia_mj[i]==ia_mj[j]){
                    ia_selected[i]=ia_selected[j]=1;
                    i_sum++;
                    break;
                }
            }
            if(ia_selected[i]==0)break;
         }
    }
    if(i_sum==7)return true;
    //mishel(ia_mj,i_num);

    //int ia_selected[14];

    for(i=0;i<i_num;i++) ia_selected[i]=0;
    return hupai1(ia_mj,i_num,ia_selected);

}

bool duting(const mjpai *pmj_arr,const int i_num)
{
    if(i_num%3!=1)return false ;
    int i_sum=0;
    int mj;

    for(mj=11;mj<20;mj++)
        if(hu(pmj_arr,i_num,mj))
            if(i_sum!=0)
                return false;
            else
                i_sum++;
    for(mj=31;mj<40;mj++)
        if(hu(pmj_arr,i_num,mj))
            if(i_sum!=0)
                return false;
            else
                i_sum++;
    for(mj=51;mj<60;mj++)
        if(hu(pmj_arr,i_num,mj))
            if(i_sum!=0)
                return false;
            else
                i_sum++;
    for(mj=71;mj<=91;mj+=10)
        if(hu(pmj_arr,i_num,mj))
            if(i_sum!=0)
                return false;
            else
                i_sum++;
    return true;

}

int qidui(const mjpai *pmj_arr,const int i_num)
{
    int i,j,k;
    
    if(i_num!=14)return 0;

    int ia_mj[14];
    for(i=0;i<14;i++)
         ia_mj[i]=(mjpai)pmj_arr[i];

    int i_sum=0,ia_selected[14];
    for(i=0;i<14;i++)
         ia_selected[i]=0;

    for(i=0;i<14;i++){
        if(ia_selected[i])continue;
        for(j=i+1;j<14;j++){
            if(ia_selected[j]||j==i)continue;

            if(ia_mj[i]==ia_mj[j]){
                ia_selected[i]=ia_selected[j]=1;
                i_sum++;
                break;
            }
        }
        if(ia_selected[i]==0)break;
    }

    if(i_sum!=7)return 0;

    mishel(ia_mj,14);

    int i_haohua=0;
    for(i=0;i<=10;i+=2)
        if(ia_mj[i]==ia_mj[i+2])
            i_haohua++;

    int i_xiaosa=0;
    for(i=0;i<14;i+=2)
         ia_selected[i]=0;
    for(i=0;i<=8;i+=2){
        if(i_xiaosa>2)break;//最多两个潇洒
        if (!ia_selected[i]&&!ia_selected[i+2]&&!ia_selected[i+4])
            if(ia_mj[i+2]-ia_mj[i]==1&&ia_mj[i+4]-ia_mj[i+2]==1)
            {
                i_xiaosa++;
                ia_selected[i]=ia_selected[i+2]=ia_selected[i+4]=1;
                i=-2;
                continue;
            }
        if (!ia_selected[i]&&!ia_selected[i+2]&&!ia_selected[i+6])
            if(i+6<=12&&ia_mj[i+2]-ia_mj[i]==1&&ia_mj[i+6]-ia_mj[i+2]==1)
            {
                i_xiaosa++;
                ia_selected[i]=ia_selected[i+2]=ia_selected[i+6]=1;
                i=-2;
                continue;
            }
        if (!ia_selected[i]&&!ia_selected[i+4]&&!ia_selected[i+6])
            if(i+6<=12&&ia_mj[i+4]-ia_mj[i]==1&&ia_mj[i+6]-ia_mj[i+4]==1)
            {
                i_xiaosa++;
                ia_selected[i]=ia_selected[i+4]=ia_selected[i+6]=1;
                i=-2;
                continue;
            }
        if (!ia_selected[i]&&!ia_selected[i+4]&&!ia_selected[i+8])
            if(i+8<=12&&ia_mj[i+4]-ia_mj[i]==1&&ia_mj[i+8]-ia_mj[i+4]==1)
            {
                i_xiaosa++;
                ia_selected[i]=ia_selected[i+4]=ia_selected[i+8]=1;
                i=-2;
                continue;
            }
    }

    return 1+i_xiaosa*100+i_haohua*10;
}


static int  power(int base,int iexp)
{
    int i,result=1;
    for(i=0;i<iexp;i++)
        result*=base;
    return result;
}

int getwei(int number,int pos)
{
    assert(number<=999&&pos<=3);
    if (pos==1)
        return number%10;
    else
        return(number%power(10,pos)-number%power(10,pos-1))/power(10,pos-1);
}


bool bukao(const mjpai *pmj_arr,int i_num,const mjpai mj)
{

    int flag=false;
    for(int i=0;i<i_num;i++)
        if(comparemj(pmj_arr[i],mj)>=-2&&comparemj(pmj_arr[i],mj)<=2)
            flag=true;

    return !flag;
}

