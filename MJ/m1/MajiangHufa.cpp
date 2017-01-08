// MajiangHufa.cpp: implementation of the CMajiangHufa class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "MajiangHufa.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMajiangHufa::CMajiangHufa()
{
	
	

}

CMajiangHufa::~CMajiangHufa()
{

}

void CMajiangHufa::ThingHupai()
{

	int d1,i;
	if(m_MajiaList.GetSize()<=4)
	{
		Thing4Only();
		return;
	}
	for(i=0;i<m_MajiaList.GetSize();i++)
	{
		d1=m_MajiaList.GetAt(i);
		m_MajiaList.RemoveAt(i);
		if(IsSameTeam(d1))
		{
			DeleteSameTeam(d1);
			ThingHupai();
			m_MajiaList.Add(d1);
			m_MajiaList.Add(d1);
			
		}

		if(IsStepTeam(d1,1))
		{
			DeleteStepTeam(d1,1);
			ThingHupai();
			m_MajiaList.Add(d1-1);
			m_MajiaList.Add(d1+1);
					
		}
		if(IsStepTeam(d1,2))
		{
			DeleteStepTeam(d1,2);
			ThingHupai();
			m_MajiaList.Add(d1+1);
			m_MajiaList.Add(d1+2);
	
		}
		if(IsStepTeam(d1,3))
		{
			DeleteStepTeam(d1,3);
			ThingHupai();
			m_MajiaList.Add(d1-1);
			m_MajiaList.Add(d1-2);
	
		}
		m_MajiaList.Add(d1);
	}
	
}

BOOL CMajiangHufa::IsSameTeam(UINT pai)
{

	int i,j,k;
	for(i=0;i<m_MajiaList.GetSize();i++)
	{
		if(pai==m_MajiaList.GetAt(i))
		{
			for(j=i+1;j<m_MajiaList.GetSize();j++)
			{
				k=m_MajiaList.GetAt(j);
				if(pai==k )
					return TRUE;
			}
		}
	}
	return FALSE;


}

BOOL CMajiangHufa::IsStepTeam(UINT pai,int type)
{

	int d1,d2;
	if(type==1)
	{
		d1=pai-1;
		d2=pai+1;
	}
	else if(type==2)
	{
		d1=pai+1;
		d2=pai+2;
	}
	else
	{
		d1=pai-1;
		d2=pai-2;
	}


	if( d1/9!=d2/9 || d1/9!=pai/9 || d1>=27 || d2>=27  )return FALSE;
//	if( m_MajiangPai[d1].type != m_MajiangPai[d2].type)
//		return FALSE;
	BOOL b1=FALSE,b2=FALSE;
	int i,n,k;
	n=m_MajiaList.GetSize();
	for(i=0;i<n;i++)
	{
		k=m_MajiaList.GetAt(i);
		if(d1==k )
		{
			b1=TRUE;
			break;
		}
	}
	for(i=0;i<n;i++)
	{
		k=m_MajiaList.GetAt(i);
		if(d2==k )
		{
			b2=TRUE;
			break;
		}
	}
    return (b1 && b2);


}



void CMajiangHufa::AddHupai(UINT pai)
{
 /*
	CString str;
   str.Format("------- %d ---------------",pai);
	   
   	m_fp.WriteString(str);

	m_fp.WriteString("\r\n");
*/
	if(pai<0 || pai>33)return;
	int k;
	int i;
	for(i=0;i<m_HupaiList.GetSize();i++)
	{
		k=m_HupaiList.GetAt(i);
		if(pai==k )
			return;
		if(pai < k)break;
	}
   m_HupaiList.InsertAt(i,pai);

 

}

void CMajiangHufa::DeleteSameTeam(UINT pai)
{

	int i,k;
	  for(i=0;i<m_MajiaList.GetSize();i++)
	  {
		  k=m_MajiaList.GetAt(i);
		if(pai==k )
		{
			m_MajiaList.RemoveAt(i);
			break;
		}
	  }
	  for(i=0;i<m_MajiaList.GetSize();i++)
	  {
		  k=m_MajiaList.GetAt(i);
		if(pai==k )
		{
			m_MajiaList.RemoveAt(i);
			break;
		}
	  }
}

void CMajiangHufa::DeleteStepTeam(UINT pai, int type)
{
	int d1,d2,k;
	if(type==1)
	{
		
		d1=pai-1;
		d2=pai+1;
	}
	else if(type==2)
	{
		d1=pai+1;
		d2=pai+2;
	}
	else
	{
		d1=pai-1;
		d2=pai-2;
	}
	int i;
	  for(i=0;i<m_MajiaList.GetSize();i++)
	  {
		  k=m_MajiaList.GetAt(i);
		if(d1==k )
		{
			m_MajiaList.RemoveAt(i);
			break;
		}
	  }
	  for(i=0;i<m_MajiaList.GetSize();i++)
	  {
		  k=m_MajiaList.GetAt(i);
		if(d2==k )
		{
			m_MajiaList.RemoveAt(i);
			break;
		}
	  }
}

void CMajiangHufa::Thing4Only()
{
/*
	CString str;
	str.Format("%d-%d-%d-%d",
		m_MajiaList.GetAt(0),
		m_MajiaList.GetAt(1),
		m_MajiaList.GetAt(2),
		m_MajiaList.GetAt(3));
	m_fp.WriteString(str);

	m_fp.WriteString("\r\n");

*/

	int d1,d2,d3,i,j,k;
	for(i=0;i<4;i++)
	{
		d1=m_MajiaList.GetAt(i);
		m_MajiaList.RemoveAt(i);
	
		
		if(IsSameTeam(d1))
		{
			DeleteSameTeam(d1);
			AddHupai(m_MajiaList.GetAt(0));
			m_MajiaList.Add(d1);
			m_MajiaList.Add(d1);
		
		}

		if(IsStepTeam(d1,1))
		{
			DeleteStepTeam(d1,1);
			AddHupai(m_MajiaList.GetAt(0));
			m_MajiaList.Add(d1-1);
			m_MajiaList.Add(d1+1);
				
		}
		if(IsStepTeam(d1,2))
		{
			DeleteStepTeam(d1,2);
			AddHupai(m_MajiaList.GetAt(0));
			m_MajiaList.Add(d1+1);
			m_MajiaList.Add(d1+2);
	
		}
		m_MajiaList.Add(d1);
	}
	
	for(i=0;i<4;i++)
	{
		d1=m_MajiaList.GetAt(i);
		m_MajiaList.RemoveAt(i);
		for(j=0;j<3;j++)
		{
			k=m_MajiaList.GetAt(j);
			if(d1 == k  )
			{
				m_MajiaList.RemoveAt(j);
				d2=m_MajiaList.GetAt(0);
				d3=m_MajiaList.GetAt(1);
			//	if( m_MajiangPai[d2].type == m_MajiangPai[d3].type)
				{
					
					if(d2 == d3  )
					{
						AddHupai(d1);
						AddHupai(d2);
					}
					if( d2/9==d3/9 && d2<27 && d3<27 )
					{
					switch(d2 -d3)
					{
					case 1:
						if((d2-8)%9)
							AddHupai(d2+1);
						if((d2-1)%9)
							AddHupai(d2-2);
						break;
					case -1:
						if(d2%9)
							AddHupai(d2-1);
						if((d2+1-8)%9)
							AddHupai(d2+2);
						break;
					case 2:
						if( d2%9)
							AddHupai(d2-1);
						break;
					case -2:
						if( (d2-8)%9)
							AddHupai(d2+1);
						break;
					default:
						break;
					}
					}
				}
				m_MajiaList.Add(d1);
			}
		}
		m_MajiaList.Add(d1);	
	}
}

void CMajiangHufa::SetMjPaiArray(int *nPailist, int nNum)
{

//	m_fp.Open("g:\\a.txt",CFile::modeCreate|CFile::modeWrite);
	for(int i=0;i<nNum;i++)
		m_MajiaList.Add(nPailist[i]);

}

void CMajiangHufa::SetMjPaiArray(CUIntArray *nPai)
{

	for(int i=0;i<nPai->GetSize();i++)
		m_MajiaList.Add(nPai->GetAt(i));
}

void CMajiangHufa::GetHupai(CUIntArray *arr)
{
//	return;

    
	if((m_MajiaList.GetSize()-1)%3)return;
	if(m_MajiaList.GetSize()==1)
	{
		arr->Add(m_MajiaList.GetAt(0));
		return;
	}
	ThingHupai();
    for(int i=0;i<m_HupaiList.GetSize();i++)
		arr->Add(m_HupaiList.GetAt(i));
	//arr= &m_HupaiList;
}


