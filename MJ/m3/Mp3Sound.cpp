
#include "stdafx.h"
#include "Mp3Sound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Mp3Sound::Mp3Sound()
{
	
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, '\\'))[1] = '\0'; //ɾ���ļ�����ֻ���·��
	strcpy(szFilePath+strlen(szFilePath), "sound\\backmusic.mp3");
	Open(szFilePath);
}

Mp3Sound::~Mp3Sound()
{
	Close();
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�Open
* �������ܣ���mci�����MP3�ļ�
* ���������
            CString strPath, MP3�ļ���·��
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::Open(const CString &strPath)
{
	MCI_OPEN_PARMS mciOP;
	DWORD dwReturn;

	ASSERT(m_wID == NULL);
	//��mci�ж��ļ�����
	mciOP.lpstrDeviceType=NULL;
	mciOP.lpstrElementName=strPath;
	dwReturn=mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT | MCI_OPEN_SHAREABLE,
		(DWORD)(LPVOID)&mciOP);
	if (dwReturn==0)
	{	m_wID=mciOP.wDeviceID;
		if (this->GetLength(&m_dwLength)!=0)
			m_dwLength=0;
	}
	else
		m_wID=NULL;
	return dwReturn;

}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�Close
* �������ܣ���mci����رյ�ǰ�򿪵�MP3�ļ�
* �����������
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::Close()
{
	MCI_GENERIC_PARMS mciGP;
	DWORD dwReturn;

	dwReturn = mciSendCommand(m_wID, MCI_CLOSE, MCI_NOTIFY|MCI_WAIT,
		(DWORD)(LPVOID)&mciGP);
	m_wID = NULL;
	return dwReturn;
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�Play
* �������ܣ���mci������Ѵ򿪣���ǰ����MP3�ļ�
* �����������
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::Play()
{
	MCI_PLAY_PARMS mciPP;
	return mciSendCommand(m_wID, MCI_PLAY, MCI_NOTIFY,
		(DWORD)(LPVOID)&mciPP);
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�Pause
* �������ܣ���mci������ͣ����
* �����������
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::Pause()
{
	MCI_GENERIC_PARMS mciGP;
	return mciSendCommand(m_wID, MCI_PAUSE, MCI_NOTIFY|MCI_WAIT,
		(DWORD)(LPVOID)&mciGP);
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�Resume
* �������ܣ���mci����ָ�����
* �����������
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::Resume()
{
	MCI_GENERIC_PARMS mciGP;
	return mciSendCommand(m_wID, MCI_RESUME, MCI_NOTIFY|MCI_WAIT,
		(DWORD)(LPVOID)&mciGP);
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�Stop
* �������ܣ���mci����ֹͣ���ţ�Ч����Pause��ͬ��
* �����������
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::Stop()
{
	MCI_GENERIC_PARMS mciGP;
	return mciSendCommand(m_wID, MCI_STOP, MCI_NOTIFY|MCI_WAIT,
		(DWORD)(LPVOID)&mciGP);
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�SeekTo
* �������ܣ���mci��������ָ��λ��
* ���������
*           DWORD dwTo, ָ��λ�ã���λΪ��
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::SeekTo(DWORD dwTo)
{
	DWORD dwReturn;
	if (dwTo>0 && dwTo<m_dwLength)
	{	MCI_SEEK_PARMS mciSP;
		mciSP.dwTo=dwTo*1000;
		dwReturn = mciSendCommand(m_wID, MCI_SEEK, MCI_WAIT|MCI_NOTIFY|MCI_TO,
			(DWORD)(LPVOID)&mciSP);
	}else if (dwTo>=m_dwLength)
		dwReturn = SeekToEnd();
	else
		dwReturn = SeekToStart();
	return dwReturn;
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�SeekToStart
* �������ܣ���mci����������ǰMP3�ļ���ʼ��
* �����������
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::SeekToStart()
{
	MCI_SEEK_PARMS mciSP;
	return mciSendCommand(m_wID, MCI_SEEK, MCI_WAIT|MCI_NOTIFY|MCI_SEEK_TO_START,
		(DWORD)(LPVOID)&mciSP);
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�SeekToEnd
* �������ܣ�������ǰMP3�ļ�ĩβ��
* �����������
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::SeekToEnd()
{
	MCI_SEEK_PARMS mciSP;
	return mciSendCommand(m_wID, MCI_SEEK, MCI_WAIT|MCI_NOTIFY|MCI_SEEK_TO_END,
		(DWORD)(LPVOID)&mciSP);
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�GetLength
* �������ܣ���mci����ȡ�õ�ǰ�򿪵�MP3�ļ��ĳ���
* �����������
* ���������
*           DWORD *dwLength, ���MP3�ļ��ĳ��ȣ���λΪ��
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::GetLength(DWORD *dwLength)
{
	MCI_STATUS_PARMS mciSP;
	mciSP.dwItem=MCI_STATUS_LENGTH;
	DWORD dwReturn=mciSendCommand(m_wID, MCI_STATUS, MCI_STATUS_ITEM,
		(DWORD)(LPVOID)&mciSP);
	if (dwReturn==0)
		*dwLength=mciSP.dwReturn/1000;
	return dwReturn;
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�GetPos
* �������ܣ���mci����ȡ�õ�ǰ�򿪵�MP3�ļ��ĵ�ǰλ��
* �����������
* ���������
*           DWORD *dwPos, ���MP3�ļ��ĵ�ǰλ�ã���λΪ��
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::GetPos(DWORD *dwPos)
{
	MCI_STATUS_PARMS mciSP;
	mciSP.dwItem=MCI_STATUS_POSITION;
	DWORD dwReturn=mciSendCommand(m_wID, MCI_STATUS, MCI_STATUS_ITEM,
		(DWORD)(LPVOID)&mciSP);
	if (dwReturn==0)
		*dwPos=mciSP.dwReturn/1000;
	return dwReturn;
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�SetMute
* �������ܣ���mci�������þ�����ȡ������
* ���������
*           BOOL bMute, ���þ���(bMute=true)��ȡ������(bMute=false)
* �����������
* ����ֵ  ��mci����Ĵ�����Ϣ
*/
DWORD Mp3Sound::SetMute(BOOL bMute)
{
	MCI_SET_PARMS mciSP;
	mciSP.dwAudio=MCI_SET_AUDIO_ALL;
	DWORD dwFlags;
	if (bMute)
		dwFlags=MCI_WAIT|MCI_SET_AUDIO|MCI_SET_OFF;
	else
		dwFlags=MCI_WAIT|MCI_SET_AUDIO|MCI_SET_ON;
	return mciSendCommand(m_wID, MCI_SET, dwFlags,
		(DWORD)(LPVOID)&mciSP);
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�IsPlaying
* �������ܣ���mci����ѯ���Ƿ����ڲ���
* �����������
* �����������
* ����ֵ  ��BOOL, �������ڲ���(true)�������ڲ���(false)
*/
BOOL Mp3Sound::IsPlaying()
{
	MCI_STATUS_PARMS mciSP;
	mciSP.dwItem=MCI_STATUS_MODE;
	DWORD dwReturn=mciSendCommand(m_wID, MCI_STATUS, MCI_STATUS_ITEM,
		(DWORD)(LPVOID)&mciSP);
	if (dwReturn==0)
		if (mciSP.dwReturn==MCI_MODE_PLAY)
			return true;
	return false;
}

////////////////////////////////////////////////////////////////////////
/*
* �������ƣ�IsPlaying
* �������ܣ���mci����ѯ���з��ļ���
* �����������
* �����������
* ����ֵ  ��BOOL, ���������ļ���(true)��û���ļ���(false)
*/
BOOL Mp3Sound::IsOpened()
{
	MCI_STATUS_PARMS mciSP;
	mciSP.dwItem=MCI_STATUS_MODE;
	DWORD dwReturn=mciSendCommand(m_wID, MCI_STATUS, MCI_STATUS_ITEM,
		(DWORD)(LPVOID)&mciSP);
	if (dwReturn==(DWORD)0)
		if (mciSP.dwReturn==MCI_MODE_OPEN ||
			mciSP.dwReturn==MCI_MODE_PLAY ||
			mciSP.dwReturn==MCI_MODE_PAUSE ||
			mciSP.dwReturn==MCI_MODE_STOP ||
			mciSP.dwReturn==MCI_MODE_SEEK
			)
			return true;
	return false;
}


