#include "audio.h"

HMIDIOUT device;
int device_flag;


void CALLBACK WaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    WAVEHDR *whdr;
    switch(uMsg) {
    case WOM_OPEN:
        break;
    case WOM_DONE:
        whdr = (WAVEHDR*)dwParam1;
        if(whdr->dwUser)
            waveOutWrite(hwo, whdr, sizeof(WAVEHDR));
        else {
            waveOutUnprepareHeader(hwo, whdr, sizeof(WAVEHDR));
            free(whdr);
        }
        break;
    case WOM_CLOSE:
        break;
    }
    dwInstance = dwInstance;
    dwParam2 = dwParam2;
}


void openAudioFile(LPSTR szFileName, Audio *audio)
{
    FILE *f;
    UINT devid;

    f = fopen(szFileName, "rb");
    if(!f)
        return;

    fseek(f, 0, SEEK_END);
    audio->file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    audio->lpfile = (char *)malloc(sizeof(char)* audio->file_size );
    fread(audio->lpfile, 1, audio->file_size , f);
    fclose(f);

    audio->wfex.wFormatTag         = *((WORD* )(audio->lpfile + OFFSET_FORMATTAG     ));
    audio->wfex.nChannels          = *((WORD* )(audio->lpfile + OFFSET_CHANNELS      ));
    audio->wfex.nSamplesPerSec     = *((DWORD*)(audio->lpfile + OFFSET_SAMPLESPERSEC ));
    audio->wfex.nAvgBytesPerSec    = *((DWORD*)(audio->lpfile + OFFSET_AVGBYTESPERSEC));
    audio->wfex.nBlockAlign        = *((WORD* )(audio->lpfile + OFFSET_BLOCKALIGN    ));
    audio->wfex.wBitsPerSample     = *((WORD* )(audio->lpfile + OFFSET_BITSPERSAMPLE ));

    audio->loop = 0;

    audio->whdr = NULL;

    for(devid = 0; devid < waveOutGetNumDevs(); devid++) {
        if(devid == waveOutGetNumDevs()) {
            audio->whdr = NULL;
            return;
        }
        if(waveOutOpen(&(audio->hwo), WAVE_MAPPER, &(audio->wfex), (DWORD)WaveOutProc, 0, CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
            break;
    }

    return;
}



void playAudio(Audio *audio)
{

    WAVEHDR* whdr;

    whdr = (WAVEHDR*)malloc(sizeof(WAVEHDR));

    ZeroMemory((void*)whdr, sizeof(WAVEHDR));
    whdr->dwFlags = 0L;
    whdr->lpData = audio->lpfile+HEADER_SIZE;
    whdr->dwBufferLength = audio->file_size-HEADER_SIZE;
    whdr->dwUser = (DWORD) audio->loop;


    if(waveOutPrepareHeader(audio->hwo, whdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        audio->whdr = NULL;
        return;
    }
    if(waveOutWrite(audio->hwo, whdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        audio->whdr = NULL;
        return;
    }


    audio->whdr = whdr;
}



void pauseAudio(Audio *audio)
{
    waveOutPause(audio->hwo);
}

void restartAudio(Audio *audio)
{
    waveOutRestart(audio->hwo);
}


long getPosition(Audio *audio)
{
    MMTIME mmtime;

    mmtime.wType = TIME_MS;
    if (audio->whdr != NULL) {
        waveOutGetPosition(audio->hwo, &mmtime, sizeof(mmtime));
        return mmtime.u.ms;
    } else {
        return 0;
    }
}


void openMidi(void)
{
    device_flag = midiOutOpen(&device, 0, 0, 0, CALLBACK_NULL);
}

void closeMidi(void)
{
    if (device_flag == MMSYSERR_NOERROR) {
        midiOutReset(device);
        midiOutClose(device);
    }
    device_flag = -1000;
}

void playMidi(int piano_or_drum, int pitch, int volume)
{

    union {
        unsigned long word;
        unsigned char data[4];
    } message;

    if (piano_or_drum == 0)
        message.data[0] = 0x90;
    else
        message.data[0] = 0x99;
    message.data[1] = pitch;
    message.data[2] = volume;
    message.data[3] = 0;

    if (device_flag == MMSYSERR_NOERROR)
        midiOutShortMsg(device, message.word);

}
