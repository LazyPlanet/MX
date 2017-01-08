#include"mjpai.h"
void mishel(mjpai *p,const int n){
	int i,j,k;
    mjpai t(mjpai::wan1);

	k=n/2;
	while(k>0){
		for(j=k;j<=n-1;j++){
			t=p[j];i=j-k;
			while((i>=0)&&(p[i]>t)){
				p[i+k]=p[i];i-=k;
			}
			p[i+k]=t;
		}
		k/=2;
	}
	return;
}
       
void mishel(int *p,int n){
	int i,j,k,t;
	k=n/2;
	while(k>0){
		for(j=k;j<=n-1;j++){
			t=p[j];i=j-k;
			while((i>=0)&&(p[i]>t)){
				p[i+k]=p[i];i-=k;
			}
			p[i+k]=t;
		}
		k/=2;
	}
	return;
}