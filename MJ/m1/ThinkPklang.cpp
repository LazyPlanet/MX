// ThinkPklang.cpp: implementation of the CThinkPklang class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CMajiangGame.h"
#include "ThinkPklang.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThinkPklang::CThinkPklang()
{

	m_bHaveDui=FALSE;
}

CThinkPklang::~CThinkPklang()
{

}
void CThinkPklang::SetMjPaiArray(int *nPailist, int nNum)
{

//	m_fp.Open("d:\\a.txt",CFile::modeCreate|CFile::modeWrite);
/*
	int n;

			CString str1="",str2;
			for( n=0;n<nNum;n++ )
			{
				str2.Format("%d--",nPailist[n] );
				str1+=str2;

			}
				
			AfxMessageBox(str1);

*/
	for( int i=0;i<17;i++ )
		m_bMjSelect[i]=FALSE;
	m_nCheckNum=0;
	m_nMaxScore2=m_nMaxScore1=m_nMaxScore3=0;
	m_nMjNum=nNum;
	m_nScore3=m_nScore2=0;
	m_nBadly=0;

	for(int j=0;j<nNum;j++)
		m_nMajiang[j]=nPailist[j];
	SortMj(m_nMajiang,nNum);

	m_nNeedPai=nPailist[nNum-1];
	
}

int CThinkPklang::FindPos(int pai,int nStart)
{

	for( int i=nStart;i<m_nMjNum;i++ )
		if( m_nMajiang[i]==pai && m_bMjSelect[i]==FALSE )
			return i;
	return -1;
}

int CThinkPklang::FindPos2(int pai,int nStart)
{
	for( int i=nStart;i<m_nRemainNum3;i++ )
		if( m_nRemain3[i]==pai && m_bMjSelect[i]==FALSE )
			return i;
	return -1;
}

BOOL CThinkPklang::IsSameTeam(UINT pai,int &pos1,int &pos2)
{

	pos1=FindPos(pai);
	if( pos1==-1 )return FALSE;
	pos2=FindPos(pai,pos1+1);
	if( pos2==-1 )return FALSE;
    return TRUE;

}

BOOL CThinkPklang::IsStepTeam(UINT pai,int &pos1,int &pos2)
{
    if( pai>=27 ) return FALSE;
	int d1=pai+1;
	int d2=pai+2;

	if( d1/9!=d2/9 || d1/9!=pai/9 || d1>=27 || d2>=27 )return FALSE;

	pos1=FindPos(d1);
	pos2=FindPos(d2);
	if( pos1!=-1 && pos2!=-1)
		return TRUE;
	return FALSE;

}


BOOL CThinkPklang::IsSameTeam2(UINT pai,int &pos1,int nStart)
{

	pos1=FindPos2(pai,nStart);
//	pos1=FindPos(pai,nStart);
	return pos1==-1?FALSE:TRUE;

}

BOOL CThinkPklang::IsStepTeam2(UINT pai,int type,int &pos1,int nStart)
{
    if( pai>=27 ) return FALSE;
	 
	int d1,d2;
	if(type==1)
	{
		d1=pai+1;
		
	}
	else 
	{
		d1=pai+2;
		
	}


	if(  d1/9!=pai/9 || d1>=27  )return FALSE;

	pos1=FindPos2(d1,nStart);
//	pos1=FindPos(d1,nStart);
	return pos1==-1?FALSE:TRUE;

}



void CThinkPklang::Think()
{
/*	if( m_nMjNum==2 )
	{
		Think1();
		return;
	}

	ThinkGood(0);
	Think1();
//	CString str;
//	str.Format("%d-%d,<%d>",m_nMaxScore3,m_nMaxScore2,m_nMaxScore3+m_nMaxScore2);
//	AfxMessageBox(str);
	return;

*/
	Think3();
	
	if( m_nMaxScore3==0 || m_nRemainNum3==0 )
	{
		m_nRemainNum3=m_nMjNum;
		for( int j=0;j<m_nRemainNum3;j++ )
			m_nRemain3[j]=m_nMajiang[j];
	}

/*   int i;

	CString str,str1;
	str1.Format("[%d]<%d> ",m_nRemainNum3,m_nMaxScore3);
	str=str+str1;
    for( i=0;i<m_nRemainNum3;i++ )
	{
		str1.Format("%d-",m_nRemain3[i]);
		str=str+str1;
		
	}

	AfxMessageBox(str);
//	m_fp.WriteString(str);

//	m_fp.WriteString("\r\n");
	*/
	Think2();


	if( m_nMaxScore2==0  )
	{
		m_nRemainNum2=m_nRemainNum3;
		for( int jj=0;jj<m_nRemainNum2;jj++ )
			m_nRemain2[jj]=m_nRemain3[jj];
	}
	if(  m_nRemainNum2==0  )
	{
		CheckOnlyTwo();
		return;

	}
/*
//	CString str,str1;
	str="";
	str1.Format("<%d> ",m_nMaxScore2);
	str=str+str1;
    for( i=0;i<m_nRemainNum2;i++ )
	{
		str1.Format("%d-",m_nRemain2[i]);
		str=str+str1;
		
	}
	m_fp.WriteString(str);

	m_fp.WriteString("\r\n");
*/	
	Think1();
	
}

int CThinkPklang::GetMaxScore()
{

	
	return m_nMaxScore3+m_nMaxScore2;
}

void CThinkPklang::Think3()
{
	int pos1,pos2;
//	if( m_nCheckNum<=2 )return;
	for( int i=0;i<m_nMjNum;i++ )
		if( m_bMjSelect[i]==FALSE )
		{
			m_bMjSelect[i]=TRUE;
		//	m_nCheckNum-=1;
            if( IsSameTeam(m_nMajiang[i],pos1,pos2 ) )
			{
				m_nPaiTeam3[m_nCheckNum*3]=m_nMajiang[i];
				m_nPaiTeam3[m_nCheckNum*3+1]=m_nMajiang[i];
				m_nPaiTeam3[m_nCheckNum*3+2]=m_nMajiang[i];

				if( m_nMajiang[i]==m_nNeedPai )
					SetTeamPai(0);

				m_nScore3+=300;
				m_nCheckNum++;
				m_bMjSelect[pos1]=TRUE;
				m_bMjSelect[pos2]=TRUE;
				Think3();
				m_bMjSelect[pos1]=FALSE;
				m_bMjSelect[pos2]=FALSE;
				m_nScore3-=300;
				m_nCheckNum--;

			}
		    if( IsStepTeam(m_nMajiang[i],pos1,pos2 ) )
			{
					if( m_nMajiang[i]==m_nNeedPai )
						SetTeamPai(1);
					else if( m_nMajiang[pos1]==m_nNeedPai )
						SetTeamPai(2);
					else if( m_nMajiang[pos2]==m_nNeedPai )
						SetTeamPai(3);

						
					m_nPaiTeam3[m_nCheckNum*3]=m_nMajiang[i];
					m_nPaiTeam3[m_nCheckNum*3+1]=m_nMajiang[i]+1;
					m_nPaiTeam3[m_nCheckNum*3+2]=m_nMajiang[i]+2;


				    m_nScore3+=300;
					m_nCheckNum++;
					m_bMjSelect[pos1]=TRUE;
					m_bMjSelect[pos2]=TRUE;
					Think3();
					m_bMjSelect[pos1]=FALSE;
					m_bMjSelect[pos2]=FALSE;
					m_nScore3-=300;
					m_nCheckNum--;
			}
			

		//	m_nCheckNum+=1;
			m_bMjSelect[i]=FALSE;
		}

	
		if( m_nScore3>m_nMaxScore3 )//
		{
				m_nRemainNum3=0;
				m_nMaxScore3=m_nScore3;
				int good=0;
				for( int kk=0;kk<m_nMjNum;kk++ )
				{
					if( m_bMjSelect[kk]==FALSE )
					{
						m_nRemain3[m_nRemainNum3]=m_nMajiang[kk];
						m_nRemainNum3++;
					}
				}
				
		}	

}

void CThinkPklang::Think2()
{


	int pos1;
//	if( m_nCheckNum==0 ){AfxMessageBox("00000000");return;}
	for( int i=0;i<m_nRemainNum3;i++ )
		if( m_bMjSelect[i]==FALSE )
		{
			m_bMjSelect[i]=TRUE;
		//	m_nCheckNum-=1;

			if( IsSameTeam2(m_nRemain3[i],pos1,i ) )
			{
				int sam=90;
				if( m_bHaveDui==FALSE )
				{
					sam=120;
					m_bHaveDui=TRUE;
				}
				m_nScore2+=sam;
			//	m_nCheckNum-=1;
				m_bMjSelect[pos1]=TRUE;
				Think2();
				m_bMjSelect[pos1]=FALSE;
				if( sam==120 )
					m_bHaveDui=FALSE;
			
				m_nScore2-=sam;
				
			//	m_nCheckNum+=1;

			}
			for( int k=1;k<=2;k++ )
			{
			   if( IsStepTeam2(m_nRemain3[i],k,pos1,i ) )
				{
				   int ste;
				    if( k>=2 )
					{
						if( IsEdge( m_nRemain3[i],m_nRemain3[i]+2 ) )
							ste=70;
						else ste=90;
					}
					else
					{
				  
						if( IsEdge( m_nRemain3[i],m_nRemain3[i]+1 ) )
							ste=80;
						else ste=100;
					}
					m_nScore2+=ste;
				//	m_nCheckNum-=1;
					m_bMjSelect[pos1]=TRUE;
					Think2();
					m_bMjSelect[pos1]=FALSE;
					m_nScore2-=ste;
				//	m_nCheckNum+=1;
				}
			}
		
		//	m_nCheckNum+=1;
			m_bMjSelect[i]=FALSE;
	

		}//end if for
		if( m_nScore2>m_nMaxScore2 )
		{
			//	AfxMessageBox("kao");
				
				m_nRemainNum2=0;
				m_nTowgoodNum=0;
				m_nMaxScore2=m_nScore2;
				for( int kk=0;kk<m_nRemainNum3;kk++ )
				{
					if( m_bMjSelect[kk]==FALSE )
						m_nRemain2[m_nRemainNum2++]=m_nRemain3[kk];
				}
		}

}

void CThinkPklang::Think1()
{
	
//	if(m_nRemainNum2==0 )
/*	{
		CString a1,a2;
	//	AfxMessageBox("haha==0");
		for( int i=0;i<m_nTowgoodNum;i++ )
		{
			a1.Format("%d,",m_nTowgood[i]);
			a2=a2+a1;
		}
		AfxMessageBox(a2);


	}*/
	
			int pai,k=0;
			int good[17],ret[17];
			int min=33;
		
			for( int n=0;n<m_nRemainNum2;n++ )
			{
				pai=m_nRemain2[n];
				if( pai>=27 )good[n]=2;
				else if( pai%9==0 || pai%9==8 )good[n]=6;
				else good[n]=10;
				good[n]+=AddGood(pai);
				if( good[n]< min )
				{
					min=good[n];
					m_nBadly=pai;
					
				}
			}

		/*

			CString str1="",str2;
			for( n=0;n<m_nRemainNum2;n++ )
			{
				str2.Format("%d--",m_nRemain2[n] );
				str1+=str2;

			}
			str1+="\r\n";
			for( n=0;n<m_nRemainNum2;n++ )
			{
				str2.Format("%d==",good[n] );
				str1+=str2;
			}
			str2.Format("<<%d>>",m_nBadly );
			str1+=str2;
		
			AfxMessageBox(str1);
*/
}



void CThinkPklang::SortMj(int *who,int end)
{
	int i,j,k,t;
	for( i = 0 ; i < end-1 ; i++ )
	{
		for( k = i ,j = i+1 ; j <end ; j++ )
            if( who[k] > who[j] )
				k = j;
		if( k != i )
		{
			t =  who[i];
			who[i]=who[k];
			who[k]= t;
		}
	}


}

BOOL CThinkPklang::IsEdge(int pai1,int pai2)
{

	if( pai1%9==0 || pai2%9==8 )
		return TRUE;
	return FALSE;
}

int CThinkPklang::AddGood(int pai)
{
	int ret=0;

	if( pai>=27 )return 0;
	if( pai%9!=0 && FindPos(pai-1)!=-1 )
	{
		if( pai%9!=1 )ret+=3;
		else ret+=1;
	}
	
	if( pai%9!=8 && FindPos(pai+1)!=-1 )
	{
		if( pai%9!=7 )ret+=3;
		else ret+=1;
	}
	////////////
	if( pai%9>1 && FindPos(pai-2)!=-1 )
	{
		ret+=2;
		
	}
	if( pai%9<7 && FindPos(pai+2)!=-1 )
	{
		ret+=1;
		
	}

	if( pai%9>0 && pai%9<8 )
	{
		if( FindPos(pai-1)!=-1 && FindPos(pai+1)!=-1 )ret=0;
	}
/*
	if( pai%9!=0 )
	{
		if( pai%9!=1 )ret+=2;
		else ret+=1;
	}
		&& pai%9!=1 )
	if( FindPos(pai-1) )ret+=2;
	if( pai%9!=8 )
	if( FindPos(pai+1) )ret+=2;
	
	if( pai%9!=1 && pai%9!=0 )
	if( FindPos(pai-2) )ret+=1;
	if( pai%9!=7 && pai%9!=8 )
	if( FindPos(pai+2) )ret+=1;
*/	return ret;
}


int CThinkPklang::GetBadly()
{

	return m_nBadly;

}

BOOL CThinkPklang::IsNeed()
{

	for( int i=0;i<m_nRemainNum2;i++ )
		if( m_nNeedPai==m_nRemain2[i] )return FALSE;

	return TRUE;

}

void CThinkPklang::SetTeamPai( int type)
{


	switch( type )
	{
	case 0:
		m_nGoodTeam3[0]=m_nGoodTeam3[1]=m_nGoodTeam3[2]=m_nNeedPai;
		break;
	case 1:
		m_nGoodTeam3[0]=m_nNeedPai;
		m_nGoodTeam3[1]=m_nNeedPai+1;
		m_nGoodTeam3[2]=m_nNeedPai+2;
		break;
	case 2:
		m_nGoodTeam3[0]=m_nNeedPai-1;
		m_nGoodTeam3[1]=m_nNeedPai;
		m_nGoodTeam3[2]=m_nNeedPai+1;
		break;
	case 3:
		m_nGoodTeam3[0]=m_nNeedPai-2;
		m_nGoodTeam3[1]=m_nNeedPai-1;
		m_nGoodTeam3[2]=m_nNeedPai;
		break;
	}

}

void CThinkPklang::GetGoodTeam(int &pai1, int &pai2, int &pai3)
{

	pai1=m_nGoodTeam3[0];
	pai2=m_nGoodTeam3[1];
	pai3=m_nGoodTeam3[2];
}

BOOL CThinkPklang::IsSameTeam3(int start, UINT pai,int &pos1,int &pos2)
{

	pos1=FindPos(pai,start+1);
	if( pos1==-1 )return FALSE;
	pos2=FindPos(pai,pos1+1);
	if( pos2==-1 )return FALSE;
    return TRUE;

}

BOOL CThinkPklang::IsStepTeam3(int start, UINT pai,int &pos1,int &pos2)
{
    if( pai>=27 ) return FALSE;
	int d1=pai+1;
	int d2=pai+2;

	if( d1/9!=d2/9 || d1/9!=pai/9 || d1>=27 || d2>=27 )return FALSE;

	pos1=FindPos(d1,start+1);
	if( pos1==-1 )return FALSE;
	pos2=FindPos(d2,pos1+1);
	if( pos2==-1 )return FALSE;
    return TRUE;

}

void CThinkPklang::ThinkGood(int start)
{
	int pos1,pos2;
	if( start>m_nMjNum-2 )
	{
		if( m_nScore3>=m_nMaxScore3 )
		{
			m_nMaxScore3=m_nScore3;
			ThinkGood2(0);
			if( m_nRemainNum2==0 )
				CheckOnlyTwo();
		}
		m_nScore3=0;
		return;
	}
	if( m_bMjSelect[start]==TRUE ) ThinkGood(start+1 );
	else
	{
		if( IsSameTeam3(start,m_nMajiang[start],pos1,pos2 ) ) 
		{
			m_bMjSelect[start]=TRUE;
			m_bMjSelect[pos1]=TRUE;
			m_bMjSelect[pos2]=TRUE;
			m_nScore3+=300;
			ThinkGood(start+1);
			m_bMjSelect[start]=FALSE;
			m_bMjSelect[pos1]=FALSE;
			m_bMjSelect[pos2]=FALSE;
			m_nScore3-=300;
		}
		if( IsStepTeam3(start,m_nMajiang[start],pos1,pos2 ) ) 
		{
			m_bMjSelect[start]=TRUE;
			m_bMjSelect[pos1]=TRUE;
			m_bMjSelect[pos2]=TRUE;
			m_nScore3+=300;
			ThinkGood(start+1);
			m_bMjSelect[start]=FALSE;
			m_bMjSelect[pos1]=FALSE;
			m_bMjSelect[pos2]=FALSE;
			m_nScore3-=300;
		}
		ThinkGood(start+1);
	}	
}

void CThinkPklang::ThinkGood2(int start)
{
	int pos1;
	if( start>m_nMjNum-1 )
	{
		if( m_nScore2>m_nMaxScore2 )
		{
			m_nMaxScore2=m_nScore2;
				
			m_nRemainNum2=0;
			for( int kk=0;kk<m_nMjNum;kk++ )
			{
					if( m_bMjSelect[kk]==FALSE )
						m_nRemain2[m_nRemainNum2++]=m_nMajiang[kk];
			}

		}
		m_nScore2=0;
		return;
	}
	if( m_bMjSelect[start]==TRUE ) ThinkGood2(start+1 );
	else
	{

		if( IsSameTeam2(m_nMajiang[start],pos1,start+1 ) )
		{
			int sam=90;
			if( m_bHaveDui==FALSE )
			{
				sam=120;
				m_bHaveDui=TRUE;
			}
			m_nScore2+=sam;
			m_bMjSelect[start]=TRUE;
			m_bMjSelect[pos1]=TRUE;
			ThinkGood2(start+1);
			m_bMjSelect[start]=FALSE;
			m_bMjSelect[pos1]=FALSE;
			if( sam==120 )
				m_bHaveDui=FALSE;
			m_nScore2-=sam;
		}
		for( int k=1;k<=2;k++ )
		{
			if( IsStepTeam2(m_nMajiang[start],k,pos1,start+1 ) )
			{
				int ste;
				if( k>=2 )
				{
					if( IsEdge( m_nMajiang[start],m_nMajiang[start]+2 ) )
						ste=70;
					else ste=90;
				}
				else
				{
					if( IsEdge( m_nMajiang[start],m_nMajiang[start]+1 ) )
						ste=80;
					else ste=100;
				}
				m_nScore2+=ste;
				m_bMjSelect[start]=TRUE;
				m_bMjSelect[pos1]=TRUE;
				ThinkGood2(start+1);
				m_bMjSelect[start]=FALSE;
				m_bMjSelect[pos1]=FALSE;
				m_nScore2-=ste;
			}
		}
			ThinkGood2(start+1);
	}
}

void CThinkPklang::CheckOnlyTwo()
{
	int two[16],num=0;
	BOOL bSameHave=FALSE;
	int max=100;
	int score[8];
	BOOL bTeamHave=FALSE;
//	for( int i=0;i<m_nMjNum;i++ )
//		if( m_bMjSelect[i]==FALSE )
//			two[num++]=m_nMajiang[i];
	for( int i=0;i<m_nRemainNum3;i++ )
		two[num++]=m_nRemain3[i];
		if( num==2 )
		{
			if( two[0]==two[1] ){m_nBadly=1000;return;}
			for( int j=0;j<m_nMjNum-2;j++ )
			{
				if( m_nMajiang[j]==m_nMajiang[j+1] && m_nMajiang[j]==m_nMajiang[j+2] )
				{
					bTeamHave=TRUE;
					m_nBadly=m_nMajiang[j];
					break;
				}

			}//endof
			if( bTeamHave==FALSE )
			{
				m_nRemainNum2=2;
				m_nRemain2[0]=two[0];
				m_nRemain2[1]=two[1];
				Think1();

			}
		}//end num=2
		else
		{
			for( int j=0;j<num/2;j++ )
			{
				if( two[j*2]==two[j*2+1] )//
				{
					if( bSameHave )
						score[j]=6;
					else 
					{
						score[j]=8;
						bSameHave=TRUE;
					}
				}
				else if( two[j*2]==two[j*2+1]-1 )
				{
					if( two[j*2]%9==0 || two[j*2+1]%9==8 )
						score[j]=4;
					else
						score[j]=7;

				}
				else
				{
					if( two[j*2]%9==0 || two[j*2+1]%9==8 )
						score[j]=3;
					else
						score[j]=5;

				}
				
			}//for end 

			for( int k=0;k<num/2;k++ )
				if( score[k]<max )
				{
					max=score[k];
					if( two[k*2]%9==0 )m_nBadly=two[k*2];
					else if( two[k*2+1]%9==8 )m_nBadly=two[k*2+1];
					else m_nBadly=two[k*2+rand()%2];
				}

		}//else num!=2
	
}

void CThinkPklang::GetHupaiTeam(int *p)
{
	for( int i=0;i<15;i++ )
		p[i]=m_nPaiTeam3[i];//=m_nMajiang[i];



}

BOOL CThinkPklang::IsOnlySameTwo()
{

	int a[2],n=0;
	for( int kk=0;kk<m_nMjNum;kk++ )
	{
		if( m_bMjSelect[kk]==FALSE )
		{
			if( n>1 )return FALSE;
			a[n++]=m_nMajiang[kk];
		}
	}
	if( a[0]==a[1] ){AfxMessageBox("kao");return TRUE;}
	else return FALSE;
}
