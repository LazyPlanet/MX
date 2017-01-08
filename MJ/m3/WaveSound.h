// WaveSound.h: interface for the WaveSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVESOUND_H__9845ABE6_08A6_410F_8E29_DF8674A127C5__INCLUDED_)
#define AFX_WAVESOUND_H__9845ABE6_08A6_410F_8E29_DF8674A127C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class WaveSound  
{
public:
	void Play();
	WaveSound();
	WaveSound(LPSTR FileName);
	BOOL LoadSound(LPSTR FileName);
	void PlayWaveSound(LPSTR FileName);
	void PlayWaveSound();
	void RePlay();
	void Stop();
	~WaveSound();
private:
	BOOL OpenWaveFile();
	BOOL LookForFmt();
	BOOL LookForData();
	BOOL AskMem();
	BOOL CheckDevice();
	void Release();
private:
	LPSTR szFileName;//声音文件名 
	MMCKINFO mmckinfoParent; 
	MMCKINFO mmckinfoSubChunk; 
	DWORD dwFmtSize; 
	HMMIO m_hmmio;//音频文件句柄 
	DWORD m_WaveLong; 
	HPSTR lpData;//音频数据 
	HANDLE m_hData; 
	HANDLE m_hFormat; 
	WAVEFORMATEX * lpFormat; 
	DWORD m_dwDataOffset; 
	DWORD m_dwDataSize; 
	WAVEHDR pWaveOutHdr; 
	WAVEOUTCAPS pwoc; 
	HWAVEOUT hWaveOut; 
};

#endif // !defined(AFX_WAVESOUND_H__9845ABE6_08A6_410F_8E29_DF8674A127C5__INCLUDED_)
