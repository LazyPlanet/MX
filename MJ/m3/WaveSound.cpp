// WaveSound.cpp: implementation of the WaveSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveSound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WaveSound::WaveSound()
{
}

WaveSound::WaveSound(LPSTR FileName)
{
	LoadSound(FileName);
}

WaveSound::~WaveSound()
{
	Release();
}

BOOL WaveSound::OpenWaveFile()
{
	if(!(m_hmmio=mmioOpen(szFileName,NULL,MMIO_READ|MMIO_ALLOCBUF))) 
	{ 
		//File open Error 
		AfxMessageBox("Failed to open the file.");
		return false; 
	}
	mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if(mmioDescend(m_hmmio, (LPMMCKINFO)&mmckinfoParent,NULL,MMIO_FINDRIFF)) 
	{ 
		//NOT WAVE FILE AND QUIT 
		AfxMessageBox("Not Wave File!");
		return false;
	} 
	return true;
}

BOOL WaveSound::LookForFmt()
{
	mmckinfoSubChunk.ckid =mmioFOURCC('f','m','t',' '); 
	if(mmioDescend(m_hmmio,&mmckinfoSubChunk,&mmckinfoParent,MMIO_FINDCHUNK)) 
	{ 
		//Can't find 'fmt' chunk 
		AfxMessageBox("Can't find 'fmt' chunk!");
		return false;
	} 
	dwFmtSize=mmckinfoSubChunk.cksize ; 
	m_hFormat=LocalAlloc(LMEM_MOVEABLE,LOWORD(dwFmtSize)); 
	if(!m_hFormat) 
	{ 
		//failed alloc memory 
		AfxMessageBox("Failed Alloc Memory!");
		return false;
	} 
	lpFormat=(WAVEFORMATEX*)LocalLock(m_hFormat); 
	if(!lpFormat) 
	{ 
		//failed to lock the memory 
		AfxMessageBox("Failed to lock memory!");
		return false;
	} 
	if((unsigned long)mmioRead(m_hmmio,(HPSTR)lpFormat,dwFmtSize)!=dwFmtSize) 
	{ 
		//failed to read format chunk 
		AfxMessageBox("Failed to read format chunk!");
		return false;
	} 
	//离开 fmt 块 
	mmioAscend(m_hmmio,&mmckinfoSubChunk,0); 
	return true;
}

BOOL WaveSound::LookForData()
{
	mmckinfoSubChunk.ckid=mmioFOURCC('d','a','t','a'); 
	if(mmioDescend(m_hmmio,&mmckinfoSubChunk,&mmckinfoParent,MMIO_FINDCHUNK)) 
	{ 
		//Can't find 'data' chunk 
		AfxMessageBox("Can't find 'data' chunk!");
		return false;
	} 
	//获得 'data'块的大小 
	m_dwDataSize=mmckinfoSubChunk.cksize ; 
	m_dwDataOffset =mmckinfoSubChunk.dwDataOffset ; 
	if(m_dwDataSize==0L) 
	{ 
		//no data in the 'data' chunk
		AfxMessageBox("No data in the 'data' chunk!");
		return false;
	} 
	return true;
}

BOOL WaveSound::AskMem()
{
	lpData=new char[m_dwDataSize]; 
	if(!lpData)
	{ 
		//faile 
		AfxMessageBox("Failed to ask memory!");
		return false;
	} 
	if(mmioSeek(m_hmmio,m_dwDataOffset/*SoundOffset*/,SEEK_SET)<0) 
	{ 
		//Failed to read the data chunk 
		AfxMessageBox("Failed to read the data chunk!");
		return false;
	} 
	m_WaveLong=mmioRead(m_hmmio,lpData,m_dwDataSize/*SoundLong*/); 
	if(m_WaveLong<0) 
	{ 
		//Failed to read the data chunk 
		AfxMessageBox("Failed to read the data chunk!");
		return false;
	} 
	return true;
}

BOOL WaveSound::CheckDevice()
{
	if(waveOutOpen(&hWaveOut,WAVE_MAPPER/*DevsNum*/,lpFormat,NULL,NULL,CALLBACK_NULL)!=0) 
	{ 
		//Failed to OPEN the wave out devices 
		AfxMessageBox("Failed to OPEN the wave out devices!");
		return false;
	} 
	//准备待播放的数据 
	pWaveOutHdr.lpData =(HPSTR)lpData; 
	pWaveOutHdr.dwBufferLength =m_WaveLong; 
	pWaveOutHdr.dwFlags =0; 
	if(waveOutPrepareHeader(hWaveOut,&pWaveOutHdr,sizeof(WAVEHDR))!=0) 
	{ 
		//Failed to prepare the wave data buffer 
		AfxMessageBox("Failed to prepare the wave data buffer!");
		return false;
	}
	return true;
}


void WaveSound::PlayWaveSound()
{
	if(waveOutWrite(hWaveOut,&pWaveOutHdr,sizeof(WAVEHDR))!=0) 
	{ 
		//Failed to write the wave data buffer  
		AfxMessageBox("Failed to write the wave data buffer!");
	} 
}

BOOL WaveSound::LoadSound(LPSTR FileName)
{
	szFileName = FileName;
	Release();
	if(!(OpenWaveFile()&&
		LookForFmt()&&
		LookForData()&&
		AskMem()&&
		CheckDevice()))
		return false;
	return true;
}

void WaveSound::PlayWaveSound(LPSTR FileName)
{
	LoadSound(FileName);
	PlayWaveSound();	
}


void WaveSound::RePlay()
{
	waveOutReset(hWaveOut);
	PlayWaveSound();
}

void WaveSound::Stop()
{
	if(waveOutPause(hWaveOut))
	{
		AfxMessageBox("Can't stop!");
	}
}

void WaveSound::Play()
{
	waveOutRestart(hWaveOut);
}

void WaveSound::Release()
{
	if(hWaveOut!=NULL)
	{
		waveOutReset(hWaveOut); 
		waveOutClose(hWaveOut); 
		if(m_hFormat!=NULL)
		{
			LocalUnlock(m_hFormat); 
			LocalFree(m_hFormat); 
		}
		if(lpData!=NULL)
			delete [] lpData;
	}
}