/*
 * Copyright (c) 2015-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

///////////////////////////////////////////////////////////////
//spitimeframevisualizervoronoitransparentwin32.cpp : Defines the entry point for the application.
//
//using tonic synth to process the input
//
//by stephane.poirier@oifii.org
//
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "spitimeframevisualizervoronoitransparentwin32.h"
#include "FreeImage.h"
#include <shellapi.h> //for CommandLineToArgW()
#include <mmsystem.h> //for timeSetEvent()
#include <stdio.h> //for swprintf()
#include <assert.h>
#include "spiwavsetlib.h"


#include "portaudio.h"

#ifdef WIN32
#if PA_USE_ASIO
#include "pa_asio.h"
#endif
#endif


//#include "DspFilters/Dsp.h"

#include "Tonic.h"
//#include "RtAudio.h"

/*
#include "ControlSwitcherTestSynth.h"
#include "ControlSwitcherExpSynth.h"
#include "BasicSynth.h"
#include "SimpleInstrumentSynth.h"
#include "StepSequencerSynth.h"
#include "StepSequencerExpSynth.h"
#include "StepSequencerBufferPlayerExpSynth.h"
#include "StepSequencerBufferPlayerEffectExpSynth.h"
//#include "EventsSynth.h"
#include "EventsExpSynth.h"
#include "BufferPlayerExpSynth.h"
#include "ArbitraryTableLookupSynth.h"
#include "BandlimitedOscillatorTestSynth.h"
#include "CompressorDuckingTestSynth.h"
#include "CompressorTestSynth.h"
#include "CompressorExpSynth.h"
#include "ControlSnapToScaleTestSynth.h"
#include "DelayTestSynth.h"
#include "FilteredNoiseSynth.h"
#include "FilterExpSynth.h"
#include "FMDroneSynth.h"
#include "InputDemoSynth.h"
#include "InputExpSynth.h"
#include "InputProcessSynth.h"
#include "InputProcessFlangerSynth.h"
#include "LFNoiseTestSynth.h"
#include "ReverbTestSynth.h"
#include "SimpleStepSeqSynth.h"
#include "SineSumSynth.h"
#include "StereoDelayTestSynth.h"
#include "SynthsAsGeneratorsDemoSynth.h"
#include "XYSpeedSynth.h"
*/
#include "InputProcessFlangerSynth.h"
using namespace Tonic;


#include <string>
#include <map>
using namespace std;



#include <GL/gl.h>
#include <GL/glu.h>
#include "ChannelBuffersHandler.h"
#include "SpectrumDisplay.h"
#include "AutocorrDisplay.h"
#include "WaveformDisplay.h"
#include "AutocorrHandler.h"
#include "defs.h"
#include <math.h>
#include <pthread.h>
#include "oifiilib.h" //note: oifiilib.lib/.dll is an MFC extension and resource DLL
#include "AutoRescaleOw2doc.h"


#include "direct.h"




RingBufferWriter global_RingBufferWriter;

// Static smart pointer for our Synth
/*
static Synth synth;
*/
//static ControlSwitcherTestSynth synth;
//static ControlSwitcherExpSynth synth;
//static BasicSynth synth;
//static SimpleInstrumentSynth synth;
//static StepSequencerSynth synth;
//static StepSequencerExpSynth synth;
//static StepSequencerBufferPlayerExpSynth synth;
//static StepSequencerBufferPlayerEffectExpSynth synth;
//static EventsSynth synth;
//static EventsExpSynth synth;
//static BufferPlayerExpSynth synth;
//static ArbitraryTableLookupSynth synth;
//static BandlimitedOscillatorTestSynth synth;
//static CompressorDuckingTestSynth synth;
//static CompressorTestSynth synth;
//static CompressorExpSynth synth;
//static ControlSnapToScaleTestSynth synth;
//static DelayTestSynth synth;
//static FilteredNoiseSynth synth;
//static FilterExpSynth synth;
//static FMDroneSynth synth;
//static InputDemoSynth* global_pSynth;
//static InputExpSynth* global_pSynth;
//static InputProcessSynth* global_pSynth;
static InputProcessFlangerSynth* global_pSynth;
//static LFNoiseTestSynth synth;
//static ReverbTestSynth synth;
//static SimpleStepSeqSynth synth;
//static SineSumSynth synth;
//static StereoDelayTestSynth synth;
//static SynthsAsGeneratorsDemoSynth synth;
//static XYSpeedSynth synth;





std::map<string,int> global_inputdevicemap;
std::map<string,int> global_outputdevicemap;

/*
RtAudio global_dac;
RtAudio::StreamParameters global_rtParams;
*/

PaStream* global_stream;
PaStreamParameters global_inputParameters;
PaStreamParameters global_outputParameters;
PaError global_err;
string global_audioinputdevicename="";
string global_audiooutputdevicename="";
int global_inputAudioChannelSelectors[2];
int global_outputAudioChannelSelectors[2];
PaAsioStreamInfo global_asioInputInfo;
PaAsioStreamInfo global_asioOutputInfo;

FILE* pFILE= NULL;

// Global Variables:

#define MAX_LOADSTRING 100
FIBITMAP* global_dib;
HFONT global_hFont;
HWND global_hwnd=NULL;
MMRESULT global_timer=0;
//#define MAX_GLOBALTEXT	4096
//WCHAR global_text[MAX_GLOBALTEXT+1];
//int global_delay_ms=5000; //default to 5 seconds delay
float global_cutfreq_hz=1000.0f; //default to 1000.0 Hz
float global_duration_sec=180;
int global_x=100;
int global_y=200;
int global_xwidth=400;
int global_yheight=400;
BYTE global_alpha=200;
int global_fontheight=24;
int global_fontwidth=-1; //will be computed within WM_PAINT handler
int global_staticalignment = 0; //0 for left, 1 for center and 2 for right
int global_staticheight=-1; //will be computed within WM_SIZE handler
int global_staticwidth=-1; //will be computed within WM_SIZE handler 
//COLORREF global_statictextcolor=RGB(0xFF, 0xFF, 0xFF); //white
COLORREF global_statictextcolor=RGB(0xFF, 0x00, 0x00); //red
//spi, begin
int global_imageheight=-1; //will be computed within WM_SIZE handler
int global_imagewidth=-1; //will be computed within WM_SIZE handler 
//spi, end
int global_titlebardisplay=1; //0 for off, 1 for on
int global_acceleratoractive=0; //0 for off, 1 for on
int global_menubardisplay=0; //0 for off, 1 for on

DWORD global_startstamp_ms;
//FILE* global_pFILE=NULL;
string global_line;
std::ifstream global_ifstream;

#define IDC_MAIN_EDIT	100
#define IDC_MAIN_STATIC	101

HINSTANCE hInst;								// current instance
//TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
//TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szTitle[1024]={L"spitimeframevisualizervoronoitransparentwin32title"};					// The title bar text
TCHAR szWindowClass[1024]={L"spitimeframevisualizervoronoitransparentwin32class"};				// the main window class name

//new parameters
int global_statictextcolor_red=255;
int global_statictextcolor_green=0;
int global_statictextcolor_blue=0;
string global_begin="begin.ahk";
string global_end="end.ahk";


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

bool global_abort=false;

bool global_skipredraw = false;
string global_imagefolder = "."; //local folder of the image files
string global_imageextension = ".jpg";
vector<string> global_txtfilenames;
int global_imageid = 0;

float RandomFloat(float a, float b) 
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

//COW2Doc* global_pOW2Doc=NULL;
CAutoRescaleOW2Doc* global_pOW2Doc = NULL;
COW2View* global_pOW2View = NULL;

struct Point3D {

	GLfloat x;
	GLfloat y;
	GLfloat z;

};

int global_thetacounter = 0;
void displayFunc(HDC hdc);
void reshapeFunc(GLsizei width, GLsizei height);
//void switchTo2D();
//void switchTo3D();
HDC global_hDC; //returned by opengl InitGL()
void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC);
void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC);

//Global buffers -------------------------
ChannelBuffersHandler* callbackFilledAudioBuffers;
ChannelBuffersHandler* drawnFromAudioBuffers;

//Waterfall Objects ------------------------
SpectrumDisplay* specDisplayMix;
float** waterfallDisplayYValuesHistory;
int waterfallCount;
int waterfallReadIndex;
int waterfallWriteIndex;

//Other Display Objects ------------------------
AutocorrDisplay* autocorrDisplayL;
WaveformDisplay* waveformDisplayL;

AutocorrHandler* autocorrHandler;
SAMPLE* autocorrBuffer;
int trailCount;
int trailReadIndex;
int trailWriteIndex;
int autocorrSkip1;
int autocorrSkip2;
GLfloat scaleFactor3D;
GLfloat scaleFactor2D;

Point3D** displayPointsHistory1;
Point3D** displayPointsHistory2;

float theta;
bool showWindows;
bool show3D;

//Global thread locking / management variables --------------
pthread_mutex_t g_mutex;
GLboolean g_ready = false;


//spi, begin, frames modif
bool skipsaveframes = true;
string global_outputfoldername = "frames";
int global_frameid = 0;
void saveframetobmpfile(HWND hwnd, HDC hdc)
{
	//HDC hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	// Retrieve the handle to a display device context for the client 
	// area of the window. 
	//hdcScreen = GetDC(NULL);
	hdcWindow = hdc; //hdcWindow = GetDC(g_hWnd);

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);
	if (!hdcMemDC)
	{
		return;
	}
	RECT rcClient;
	GetClientRect(hwnd, &rcClient); //GetClientRect(g_hWnd, &rcClient);
	hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top);
	if (!hbmScreen)
	{
		DeleteObject(hdcMemDC);
		return;
	}

	HBITMAP oldhbitmap = (HBITMAP)SelectObject(hdcMemDC, hbmScreen);

	if (!BitBlt(hdcMemDC,
		0, 0,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		hdcWindow,
		0, 0,
		SRCCOPY))
	{
		SelectObject(hdcMemDC, oldhbitmap);
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		return;
	}
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 *
		bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcWindow, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);

	// A file is created, this is where we will save the screen capture.
	string filename = global_outputfoldername;
	filename += "\\frame_";
	global_frameid++;
	char buf[256];
	sprintf(buf, "%06d", global_frameid);
	filename += buf;
	filename += ".bmp";
	HANDLE hFile = CreateFileA(filename.c_str(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER)+
		sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+
		(DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFile);

	//Clean up
	/*
	done:
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	//ReleaseDC(NULL,hdcScreen);
	//ReleaseDC(g_hWnd,hdcWindow);
	*/
	SelectObject(hdcMemDC, oldhbitmap);
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	return;

}
//spi, end

static int renderCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData );

static int gNumNoInputs = 0;
// This routine will be called by the PortAudio engine when audio is needed.
// It may be called at interrupt level on some machines so don't do anything
// that could mess up the system like calling malloc() or free().
//
static int renderCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    SAMPLE *out = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    unsigned int i;
    (void) timeInfo; // Prevent unused variable warnings.
    (void) statusFlags;
    (void) userData;

	if(global_abort==true) return paAbort;

	
	if( inputBuffer == NULL )
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = 0;  // left - silent
            *out++ = 0;  // right - silent 
        }
        gNumNoInputs += 1;
    }
    else
    {
		/*
		for (i = 0; i<framesPerBuffer; i++)
		{
			*out++ = *in++;  // left - unprocessed 
			*out++ = *in++;  // right - unprocessed
		}
		*/
		global_RingBufferWriter.write((float*)inputBuffer, framesPerBuffer, NUM_CHANNELS);
		global_pSynth->fillBufferOfFloats((float*)outputBuffer, framesPerBuffer, NUM_CHANNELS);

		if(0) pthread_mutex_lock(&g_mutex);
		callbackFilledAudioBuffers->acquireAndSeparateNewBuffer((float*)outputBuffer);
		g_ready = true;
		if(0) pthread_mutex_unlock(&g_mutex);

	}
    
	
    return paContinue;
}

/*
int renderCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *userData)
{
	synth.fillBufferOfFloats((float*)outputBuffer, nBufferFrames, NUM_CHANNELS);
	return 0;
}
*/



bool SelectAudioInputDevice()
{
	const PaDeviceInfo* deviceInfo;
    int numDevices = Pa_GetDeviceCount();
    for( int i=0; i<numDevices; i++ )
    {
        deviceInfo = Pa_GetDeviceInfo( i );
		string devicenamestring = deviceInfo->name;
		global_inputdevicemap.insert(pair<string,int>(devicenamestring,i));
		if(pFILE) fprintf(pFILE,"id=%d, name=%s\n", i, devicenamestring.c_str());
	}

	int deviceid = Pa_GetDefaultInputDevice(); // default input device 
	std::map<string,int>::iterator it;
	it = global_inputdevicemap.find(global_audioinputdevicename);
	if(it!=global_inputdevicemap.end())
	{
		deviceid = (*it).second;
		//printf("%s maps to %d\n", global_audiodevicename.c_str(), deviceid);
		deviceInfo = Pa_GetDeviceInfo(deviceid);
		//assert(inputAudioChannelSelectors[0]<deviceInfo->maxInputChannels);
		//assert(inputAudioChannelSelectors[1]<deviceInfo->maxInputChannels);
	}
	else
	{
		//Pa_Terminate();
		//return -1;
		//printf("error, audio device not found, will use default\n");
		//MessageBox(win,"error, audio device not found, will use default\n",0,0);
		deviceid = Pa_GetDefaultInputDevice();
	}


	global_inputParameters.device = deviceid; 
	if (global_inputParameters.device == paNoDevice) 
	{
		//MessageBox(win,"error, no default input device.\n",0,0);
		return false;
	}
	//global_inputParameters.channelCount = 2;
	global_inputParameters.channelCount = NUM_CHANNELS;
	global_inputParameters.sampleFormat =  PA_SAMPLE_TYPE;
	global_inputParameters.suggestedLatency = Pa_GetDeviceInfo( global_inputParameters.device )->defaultLowOutputLatency;
	//inputParameters.hostApiSpecificStreamInfo = NULL;

	//Use an ASIO specific structure. WARNING - this is not portable. 
	//PaAsioStreamInfo asioInputInfo;
	global_asioInputInfo.size = sizeof(PaAsioStreamInfo);
	global_asioInputInfo.hostApiType = paASIO;
	global_asioInputInfo.version = 1;
	global_asioInputInfo.flags = paAsioUseChannelSelectors;
	global_asioInputInfo.channelSelectors = global_inputAudioChannelSelectors;
	if(deviceid==Pa_GetDefaultInputDevice())
	{
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else if(Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paASIO) 
	{
		global_inputParameters.hostApiSpecificStreamInfo = &global_asioInputInfo;
	}
	else if(Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paWDMKS) 
	{
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else
	{
		//assert(false);
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	return true;
}



bool SelectAudioOutputDevice()
{
	const PaDeviceInfo* deviceInfo;
    int numDevices = Pa_GetDeviceCount();
    for( int i=0; i<numDevices; i++ )
    {
        deviceInfo = Pa_GetDeviceInfo( i );
		string devicenamestring = deviceInfo->name;
		global_outputdevicemap.insert(pair<string,int>(devicenamestring,i));
		if(pFILE) fprintf(pFILE,"id=%d, name=%s\n", i, devicenamestring.c_str());
	}

	int deviceid = Pa_GetDefaultOutputDevice(); // default output device 
	std::map<string,int>::iterator it;
	it = global_outputdevicemap.find(global_audiooutputdevicename);
	if(it!=global_outputdevicemap.end())
	{
		deviceid = (*it).second;
		//printf("%s maps to %d\n", global_audiodevicename.c_str(), deviceid);
		deviceInfo = Pa_GetDeviceInfo(deviceid);
		//assert(inputAudioChannelSelectors[0]<deviceInfo->maxInputChannels);
		//assert(inputAudioChannelSelectors[1]<deviceInfo->maxInputChannels);
	}
	else
	{
		//Pa_Terminate();
		//return -1;
		//printf("error, audio device not found, will use default\n");
		//MessageBox(win,"error, audio device not found, will use default\n",0,0);
		deviceid = Pa_GetDefaultOutputDevice();
	}


	global_outputParameters.device = deviceid; 
	if (global_outputParameters.device == paNoDevice) 
	{
		//MessageBox(win,"error, no default output device.\n",0,0);
		return false;
	}
	//global_inputParameters.channelCount = 2;
	global_outputParameters.channelCount = NUM_CHANNELS;
	global_outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
	global_outputParameters.suggestedLatency = Pa_GetDeviceInfo( global_outputParameters.device )->defaultLowOutputLatency;
	//outputParameters.hostApiSpecificStreamInfo = NULL;

	//Use an ASIO specific structure. WARNING - this is not portable. 
	//PaAsioStreamInfo asioInputInfo;
	global_asioOutputInfo.size = sizeof(PaAsioStreamInfo);
	global_asioOutputInfo.hostApiType = paASIO;
	global_asioOutputInfo.version = 1;
	global_asioOutputInfo.flags = paAsioUseChannelSelectors;
	global_asioOutputInfo.channelSelectors = global_outputAudioChannelSelectors;
	if(deviceid==Pa_GetDefaultOutputDevice())
	{
		global_outputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else if(Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paASIO) 
	{
		global_outputParameters.hostApiSpecificStreamInfo = &global_asioOutputInfo;
	}
	else if(Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paWDMKS) 
	{
		global_outputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else
	{
		//assert(false);
		global_outputParameters.hostApiSpecificStreamInfo = NULL;
	}
	return true;
}




// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

char buffer[1024];
void CALLBACK StartGlobalProcess(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	WavSetLib_Initialize(global_hwnd, IDC_MAIN_STATIC, global_staticwidth, global_staticheight, global_fontwidth, global_fontheight, global_staticalignment);

	DWORD nowstamp_ms = GetTickCount();
	while( (global_duration_sec<0.0f) || ((nowstamp_ms-global_startstamp_ms)/1000.0f)<global_duration_sec )
	{
		//Sleep((int)(1000));
		Sleep(1);
		if (!global_skipredraw) InvalidateRect(global_hwnd, NULL, false);

		/*
		//dynamically change filter parameter
		int random_integer;
		int lowest=1, highest=10; 
		int range=(highest-lowest)+1;
		random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
		float cutfreq_hz = global_cutfreq_hz * random_integer*1.0f/range;
		sprintf(buffer, "%f Hz\n", cutfreq_hz);
		StatusAddText(buffer);
		*/

		/*
		///////////////////////////////
		//simple instrument synth begin
		static int twoOctavePentatonicScale[10] = { 0, 2, 4, 7, 9, 12, 14, 16, 19, 21 };

		// set a parameter that we created when we defined the synth
		synth.setParameter("midiNumber", 44 + twoOctavePentatonicScale[random_integer-1]);

		// simply setting the value of a parameter causes that parameter to send a "trigger" message to any
		// using them as triggers
		synth.setParameter("trigger", 1);
		//simple instrument synth end
		/////////////////////////////
		*/

		//////////////////////////////////
		//control switcher exp synth begin
		/*
		if(random_integer>5)
		synth.setParameter("clickVol", 0.0, true);
		else
		synth.setParameter("clickVol", 1.0, true);
		*/
		//synth.setParameter("bpm", cutfreq_hz);
		//synth.setParameter("freq", cutfreq_hz);
		//synth.setParameter("addtlFM", random_integer*10);
		//control switcher exp synth end
		//////////////////////////////////

		///////////////////////
		//fm drone synth begin
		//synth.setParameter("carrierPitch", 20+random_integer );
		//synth.setParameter("modIndex", random_integer/10.f);
		//synth.setParameter("lfoAmt", random_integer / 10.f);
		//fm drone synth end
		///////////////////////
		
		//////////////////
		//input demo synth
		//global_pSynth->setParameter("ringFreq", cutfreq_hz);
		//global_pSynth->setParameter("ringFreq", random_integer);
		//input demo synth
		//////////////////


		nowstamp_ms = GetTickCount();
	}
	PostMessage(global_hwnd, WM_DESTROY, 0, 0);
}




PCHAR*
    CommandLineToArgvA(
        PCHAR CmdLine,
        int* _argc
        )
    {
        PCHAR* argv;
        PCHAR  _argv;
        ULONG   len;
        ULONG   argc;
        CHAR   a;
        ULONG   i, j;

        BOOLEAN  in_QM;
        BOOLEAN  in_TEXT;
        BOOLEAN  in_SPACE;

        len = strlen(CmdLine);
        i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

        argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
            i + (len+2)*sizeof(CHAR));

        _argv = (PCHAR)(((PUCHAR)argv)+i);

        argc = 0;
        argv[argc] = _argv;
        in_QM = FALSE;
        in_TEXT = FALSE;
        in_SPACE = TRUE;
        i = 0;
        j = 0;

        while( a = CmdLine[i] ) {
            if(in_QM) {
                if(a == '\"') {
                    in_QM = FALSE;
                } else {
                    _argv[j] = a;
                    j++;
                }
            } else {
                switch(a) {
                case '\"':
                    in_QM = TRUE;
                    in_TEXT = TRUE;
                    if(in_SPACE) {
                        argv[argc] = _argv+j;
                        argc++;
                    }
                    in_SPACE = FALSE;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if(in_TEXT) {
                        _argv[j] = '\0';
                        j++;
                    }
                    in_TEXT = FALSE;
                    in_SPACE = TRUE;
                    break;
                default:
                    in_TEXT = TRUE;
                    if(in_SPACE) {
                        argv[argc] = _argv+j;
                        argc++;
                    }
                    _argv[j] = a;
                    j++;
                    in_SPACE = FALSE;
                    break;
                }
            }
            i++;
        }
        _argv[j] = '\0';
        argv[argc] = NULL;

        (*_argc) = argc;
        return argv;
    }

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	global_startstamp_ms = GetTickCount();

	//LPWSTR *szArgList;
	LPSTR *szArgList;
	int nArgs;
	int i;

	//szArgList = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	szArgList = CommandLineToArgvA(GetCommandLineA(), &nArgs);
	if( NULL == szArgList )
	{
		//wprintf(L"CommandLineToArgvW failed\n");
		return FALSE;
	}
	LPWSTR *szArgListW;
	int nArgsW;
	szArgListW = CommandLineToArgvW(GetCommandLineW(), &nArgsW);
	if( NULL == szArgListW )
	{
		//wprintf(L"CommandLineToArgvW failed\n");
		return FALSE;
	}

	global_audioinputdevicename="E-MU ASIO"; //"Wave (2- E-MU E-DSP Audio Proce"
	if(nArgs>1)
	{
		//global_filename = szArgList[1];
		global_audioinputdevicename = szArgList[1]; 
	}
	global_inputAudioChannelSelectors[0] = 0; // on emu patchmix ASIO device channel 1 (left)
	global_inputAudioChannelSelectors[1] = 1; // on emu patchmix ASIO device channel 2 (right)
	//global_inputAudioChannelSelectors[0] = 2; // on emu patchmix ASIO device channel 3 (left)
	//global_inputAudioChannelSelectors[1] = 3; // on emu patchmix ASIO device channel 4 (right)
	//global_inputAudioChannelSelectors[0] = 8; // on emu patchmix ASIO device channel 9 (left)
	//global_inputAudioChannelSelectors[1] = 9; // on emu patchmix ASIO device channel 10 (right)
	//global_inputAudioChannelSelectors[0] = 10; // on emu patchmix ASIO device channel 11 (left)
	//global_inputAudioChannelSelectors[1] = 11; // on emu patchmix ASIO device channel 12 (right)
	if(nArgs>2)
	{
		global_inputAudioChannelSelectors[0]=atoi((LPCSTR)(szArgList[2])); //0 for first asio channel (left) or 2, 4, 6, etc.
	}
	if(nArgs>3)
	{
		global_inputAudioChannelSelectors[1]=atoi((LPCSTR)(szArgList[3])); //1 for second asio channel (right) or 3, 5, 7, etc.
	}
	global_audiooutputdevicename="E-MU ASIO"; //"Wave (2- E-MU E-DSP Audio Proce"
	if(nArgs>4)
	{
		//global_filename = szArgList[1];
		global_audiooutputdevicename = szArgList[4]; 
	}
	global_outputAudioChannelSelectors[0] = 0; // on emu patchmix ASIO device channel 1 (left)
	global_outputAudioChannelSelectors[1] = 1; // on emu patchmix ASIO device channel 2 (right)
	//global_outputAudioChannelSelectors[0] = 2; // on emu patchmix ASIO device channel 3 (left)
	//global_outputAudioChannelSelectors[1] = 3; // on emu patchmix ASIO device channel 4 (right)
	//global_outputAudioChannelSelectors[0] = 8; // on emu patchmix ASIO device channel 9 (left)
	//global_outputAudioChannelSelectors[1] = 9; // on emu patchmix ASIO device channel 10 (right)
	//global_outputAudioChannelSelectors[0] = 10; // on emu patchmix ASIO device channel 11 (left)
	//global_outputAudioChannelSelectors[1] = 11; // on emu patchmix ASIO device channel 12 (right)
	if(nArgs>5)
	{
		global_outputAudioChannelSelectors[0]=atoi((LPCSTR)(szArgList[5])); //0 for first asio channel (left) or 2, 4, 6, etc.
	}
	if(nArgs>6)
	{
		global_outputAudioChannelSelectors[1]=atoi((LPCSTR)(szArgList[6])); //1 for second asio channel (right) or 3, 5, 7, etc.
	}

	
	if(nArgs>7)
	{
		global_cutfreq_hz = atof(szArgList[7]); 
	}
	if(nArgs>8)
	{
		global_duration_sec = atof(szArgList[8]);
	}


	if(nArgs>9)
	{
		global_x = atoi(szArgList[9]);
	}
	if(nArgs>10)
	{
		global_y = atoi(szArgList[10]);
	}
	if(nArgs>11)
	{
		global_xwidth = atoi(szArgList[11]);
	}
	if(nArgs>12)
	{
		global_yheight = atoi(szArgList[12]);
	}
	if(nArgs>13)
	{
		global_alpha = atoi(szArgList[13]);
	}
	if(nArgs>14)
	{
		global_titlebardisplay = atoi(szArgList[14]);
	}
	if(nArgs>15)
	{
		global_menubardisplay = atoi(szArgList[15]);
	}
	if(nArgs>16)
	{
		global_acceleratoractive = atoi(szArgList[16]);
	}
	if(nArgs>17)
	{
		global_fontheight = atoi(szArgList[17]);
	}

	//new parameters
	if(nArgs>18)
	{
		global_statictextcolor_red = atoi(szArgList[18]);
	}
	if(nArgs>19)
	{
		global_statictextcolor_green = atoi(szArgList[19]);
	}
	if(nArgs>20)
	{
		global_statictextcolor_blue = atoi(szArgList[20]);
	}
	if(nArgs>21)
	{
		wcscpy(szWindowClass, szArgListW[21]); 
	}
	if(nArgs>22)
	{
		wcscpy(szTitle, szArgListW[22]); 
	}
	if(nArgs>23)
	{
		global_begin = szArgList[23]; 
	}
	if (nArgs > 24)
	{
		global_end = szArgList[24];
	}
	if (nArgs>25)
	{
		global_imagefolder = szArgList[25];
	}
	if (nArgs > 26)
	{
		global_imageextension = szArgList[26];
	}
	
	global_statictextcolor=RGB(global_statictextcolor_red, global_statictextcolor_green, global_statictextcolor_blue);
	LocalFree(szArgList);
	LocalFree(szArgListW);

	//spi, begin, frames modif
	//////////////////////////////////////////////
	//if don't exist, create dir for output frames 
	//////////////////////////////////////////////
	_mkdir(global_outputfoldername.c_str());
	//spi, end

	int nShowCmd = false;
	//ShellExecuteA(NULL, "open", "begin.bat", "", NULL, nShowCmd);
	ShellExecuteA(NULL, "open", global_begin.c_str(), "", NULL, nCmdShow);


	//////////////////////////
	//initialize random number
	//////////////////////////
	srand((unsigned)time(0));



	pFILE = fopen("devices.txt","w");
	
	///////////////////////
	//initialize port audio
	///////////////////////
    global_err = Pa_Initialize();
    if( global_err != paNoError )
	{
		//MessageBox(0,"portaudio initialization failed",0,MB_ICONERROR);
		if(pFILE) fprintf(pFILE, "portaudio initialization failed.\n");
		fclose(pFILE);
		return 1;
	}
	

	
	////////////////////////
	//audio device selection
	////////////////////////
	SelectAudioInputDevice();
	SelectAudioOutputDevice();
	
	/*
	fclose(pFILE);
	pFILE = NULL;
	*/

	/*
	// Create an api map.
	std::map<int, std::string> apiMap;
	apiMap[RtAudio::MACOSX_CORE] = "OS-X Core Audio";
	apiMap[RtAudio::WINDOWS_ASIO] = "Windows ASIO";
	apiMap[RtAudio::WINDOWS_DS] = "Windows Direct Sound";
	apiMap[RtAudio::UNIX_JACK] = "Jack Client";
	apiMap[RtAudio::LINUX_ALSA] = "Linux ALSA";
	apiMap[RtAudio::LINUX_OSS] = "Linux OSS";
	apiMap[RtAudio::RTAUDIO_DUMMY] = "RtAudio Dummy";

	std::vector< RtAudio::Api > apis;
	RtAudio::getCompiledApi(apis);

	fprintf(pFILE, "\nCompiled APIs:\n");
	for (unsigned int i = 0; i<apis.size(); i++)
		fprintf(pFILE, " %s\n ", apiMap[apis[i]] );


	fprintf(pFILE, "\nCurrent API: %s\n", apiMap[global_dac.getCurrentApi()]);

	unsigned int devices = global_dac.getDeviceCount();
	fprintf(pFILE, "\nFound %i device(s) ...\n", devices);
	*/

	/*
	// Configure RtAudio
	global_rtParams.deviceId = global_dac.getDefaultOutputDevice();
	global_rtParams.nChannels = NUM_CHANNELS;
	unsigned int sampleRate = SAMPLE_RATE;
	unsigned int bufferFrames = 512; // 512 sample frames
	*/

	// You don't necessarily have to do this - it will default to 44100 if not set.
	Tonic::setSampleRate(SAMPLE_RATE);

	//tonic ringbuffer initialization
	global_RingBufferWriter = RingBufferWriter("input", FRAMES_PER_BUFFER, NUM_CHANNELS);
	//tonic synth creation
	//global_pSynth = new(InputDemoSynth);
	//global_pSynth = new(InputExpSynth);
	//global_pSynth = new(InputProcessSynth);
	global_pSynth = new(InputProcessFlangerSynth);


	////////////////////////////
	//init time frame visualizer
	////////////////////////////
	unsigned int bufferFrames = FRAMES_PER_BUFFER;
	// Initialize global buffers and objects
	callbackFilledAudioBuffers = new ChannelBuffersHandler(bufferFrames, NUM_CHANNELS);
	drawnFromAudioBuffers = new ChannelBuffersHandler(bufferFrames, NUM_CHANNELS);

	waveformDisplayL = new WaveformDisplay(bufferFrames, 10, 380, 10, 80, 0, bufferFrames, -1.0, 1.0);
	autocorrDisplayL = new AutocorrDisplay(bufferFrames, 10, 380, 10, 80, 0, bufferFrames, -8, 8);
	specDisplayMix = new SpectrumDisplay(bufferFrames, 2 * bufferFrames, "Hamming", "log", -7, 14, -5, 9, 0, SRATE / 2, -50.0, 20.0);

	//Initialize watefall variables
	//waterfallCount = 20; //original
	waterfallCount = 2; //spi
	waterfallDisplayYValuesHistory = new float*[waterfallCount];
	for (int i = 0; i < waterfallCount; i++)
	{
		waterfallDisplayYValuesHistory[i] = new float[2 * bufferFrames];
		memset(waterfallDisplayYValuesHistory[i], -30, 2 * bufferFrames * sizeof(float));
	}

	waterfallReadIndex = 1;
	waterfallWriteIndex = 0;

	// Initialize the coolness variables
	autocorrHandler = new AutocorrHandler(bufferFrames);
	//	trailCount = 10;	//2048 buffersize
	//trailCount = 20; //original
	trailCount = 2; //spi
	autocorrBuffer = new SAMPLE[bufferFrames];
	memset(autocorrBuffer, 0.0, bufferFrames * sizeof(SAMPLE));

	displayPointsHistory1 = new Point3D*[trailCount];
	for (int i = 0; i < trailCount; i++)
	{
		displayPointsHistory1[i] = new Point3D[bufferFrames];
		memset(displayPointsHistory1[i], 0, bufferFrames * sizeof(Point3D));
	}

	displayPointsHistory2 = new Point3D*[trailCount];
	for (int i = 0; i < trailCount; i++)
	{
		displayPointsHistory2[i] = new Point3D[bufferFrames];
		memset(displayPointsHistory2[i], 0, bufferFrames * sizeof(Point3D));
	}

	trailReadIndex = 1;
	trailWriteIndex = 0;
	autocorrSkip1 = 10; //original
	//autocorrSkip1 = 1; //spi
	autocorrSkip2 = 80; //original
	//autocorrSkip2 = 2; //spi

	//Set display variables
	scaleFactor3D = 0.015* 4.0;
	scaleFactor2D = 2.4*4.0;
	theta = 0.0;
	showWindows = false; //original
	//showWindows = true; //spi
	show3D = true;

	
	
	

	pthread_mutex_init(&g_mutex, NULL);

	//////////////
    //setup stream  
	//////////////
    global_err = Pa_OpenStream(
        &global_stream,
		&global_inputParameters, //NULL, //&global_inputParameters,
        &global_outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        0, //paClipOff,      // we won't output out of range samples so don't bother clipping them
        renderCallback,  
        NULL ); //no callback userData
    if( global_err != paNoError ) 
	{
		char errorbuf[2048];
        sprintf(errorbuf, "Unable to open stream: %s\n", Pa_GetErrorText(global_err));
		//MessageBox(0,errorbuf,0,MB_ICONERROR);
		if (pFILE)
		{
			fprintf(pFILE, "%s\n", errorbuf);
			fclose(pFILE);
		}
        return 1;
    }
	

	
	//////////////
    //start stream  
	//////////////
    global_err = Pa_StartStream( global_stream );
    if( global_err != paNoError ) 
	{
		char errorbuf[2048];
        sprintf(errorbuf, "Unable to start stream: %s\n", Pa_GetErrorText(global_err));
		//MessageBox(0,errorbuf,0,MB_ICONERROR);
		if (pFILE)
		{
			fprintf(pFILE, "%s\n", errorbuf);
			fclose(pFILE);
		}
		return 1;
    }
	
	fclose(pFILE);
	pFILE = NULL;

	/*
	///////////////////////
	//Enable alpha blending
	///////////////////////
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	*/


	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadString(hInstance, IDC_SPIWAVWIN32, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	if(global_acceleratoractive)
	{
		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPIWAVWIN32));
	}
	else
	{
		hAccelTable = NULL;
	}
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	//wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPIWAVWIN32));
	wcex.hIcon			= (HICON)LoadImage(NULL, L"background_32x32x16.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	//wcex.hIcon          = (HICON)LoadImage(NULL, L"background_32x32xrgb-new.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);

	if(global_menubardisplay)
	{
		wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SPIWAVWIN32); //original with menu
	}
	else
	{
		wcex.lpszMenuName = NULL; //no menu
	}
	wcex.lpszClassName	= szWindowClass;
	//wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hIconSm		= (HICON)LoadImage(NULL, L"background_16x16x16.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	//wcex.hIconSm        = (HICON)LoadImage(NULL, L"background_16x16xrgb-new.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	return RegisterClassEx(&wcex);
}

void SwitchVoronoiDiagram()
{
	if (global_pOW2Doc == NULL) return;
	if (global_pOW2View == NULL) return;
	if (global_skipredraw == true) return;

	//if at least one image in the folder
	if (global_txtfilenames.size() > 0)
	{
		//reset image id if end of file
		if (global_imageid >= global_txtfilenames.size()) global_imageid = 0;

		CAutoRescaleOW2Doc* pOldOW2Doc = global_pOW2Doc;
		COW2View* pOldOW2View = global_pOW2View;

		global_skipredraw = true;
		global_pOW2Doc = NULL;
		global_pOW2View = NULL;

		/////////////////////////
		//compute voronoi diagram
		/////////////////////////
		//global_pOW2Doc = new COW2Doc(global_hwnd);
		CAutoRescaleOW2Doc* pNewOW2Doc = new CAutoRescaleOW2Doc(global_hwnd);
		COW2View* pNewOW2View = new COW2View(global_hwnd, (COWDocument*)pNewOW2Doc);
		pNewOW2View->OnCreate(NULL);
		if (pNewOW2Doc != NULL && pNewOW2View != NULL)
		{
			wstring mywstring = utf8_decode(global_txtfilenames[global_imageid]);
			bool bresult = pNewOW2Doc->OnOpenDocument(mywstring.c_str());

			if (bresult)
			{
			
				pNewOW2View->OnInitialUpdate();

			
				//(global_hwnd, NULL, false);
				pNewOW2Doc->OnAnalysisTextureextraction();

				pNewOW2View->m_pViewDataSettings->bDrawPointset = FALSE;
				pNewOW2View->m_pViewDataSettings->bDrawVoronoiset = TRUE;
				//InvalidateRect(global_hwnd, NULL, false);

				//spi, avril 2015, begin
				//string fullpath = "c:\\temp\\";
				//_mkdir(fullpath.c_str());
				//string base_filename = global_txtfilenames[global_imageid].substr(global_txtfilenames[global_imageid].find_last_of("/\\") + 1);
				//fullpath = fullpath + base_filename;
				pNewOW2Doc->SaveVoronoiDiagramColoredWithPixelsetAsImagefile(L"output.jpg", pNewOW2View);
				//spi, avril 2015, end
			

				delete pOldOW2Doc;
				delete pOldOW2View;
				global_pOW2Doc = pNewOW2Doc;
				global_pOW2View = pNewOW2View;
			}
			else
			{
				delete pNewOW2Doc;
				delete pNewOW2View;
				global_pOW2Doc = pOldOW2Doc;
				global_pOW2View = pOldOW2View;
			}
			global_skipredraw = false;
		}
		global_imageid++;
	}

}
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	global_dib = FreeImage_Load(FIF_JPEG, "background.jpg", JPEG_DEFAULT);
	//global_dib = FreeImage_Load(FIF_BMP, "background.bmp", BMP_DEFAULT);

	
	FIBITMAP* local_16x16xrgbdib = FreeImage_Rescale(global_dib, 16, 16, FILTER_BICUBIC);
	FreeImage_Save(FIF_ICO, local_16x16xrgbdib, "background_16x16xrgb-new.ico");
	FreeImage_Unload(local_16x16xrgbdib);

	FIBITMAP* local_32x32xrgbdib = FreeImage_Rescale(global_dib, 32, 32, FILTER_BICUBIC);
	FreeImage_Save(FIF_ICO, local_32x32xrgbdib, "background_32x32xrgb-new.ico");
	FreeImage_Unload(local_32x32xrgbdib);

	FIBITMAP* local_48x48xrgbdib = FreeImage_Rescale(global_dib, 48, 48, FILTER_BICUBIC);
	FreeImage_Save(FIF_ICO, local_48x48xrgbdib, "background_48x48xrgb-new.ico");
	FreeImage_Unload(local_48x48xrgbdib);
	

	//global_hFont=CreateFontW(global_fontheight,0,0,0,FW_NORMAL,0,0,0,0,0,0,2,0,L"SYSTEM_FIXED_FONT");
	global_hFont=CreateFontW(global_fontheight,0,0,0,FW_NORMAL,0,0,0,0,0,0,2,0,L"Segoe Script");

	if(global_titlebardisplay)
	{
		hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, //original with WS_CAPTION etc.
			global_x, global_y, global_xwidth, global_yheight, NULL, NULL, hInstance, NULL);
	}
	else
	{
		hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP | WS_VISIBLE, //no WS_CAPTION etc.
			global_x, global_y, global_xwidth, global_yheight, NULL, NULL, hInstance, NULL);
	}
	if (!hWnd)
	{
		return FALSE;
	}
	global_hwnd = hWnd;

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, 0, global_alpha, LWA_ALPHA);

	//0) execute cmd line to get all folder's image filenames
	string quote = "\"";
	string pathfilter;
	string path = global_imagefolder;
	//pathfilter = path + "\\*.bmp";
	pathfilter = path + "\\*" + global_imageextension;
	string systemcommand;
	//systemcommand = "DIR " + quote + pathfilter + quote + "/B /O:N > wsic_filenames.txt"; //wsip tag standing for wav set (library) instrumentset (class) populate (function)
	systemcommand = "DIR " + quote + pathfilter + quote + "/B /S /O:N > spitfvvt_filenames.txt"; // /S for adding path into "wsic_filenames.txt"
	system(systemcommand.c_str());

	//2) load in all "spiss_filenames.txt" file
	//vector<string> global_txtfilenames;
	ifstream ifs("spitfvvt_filenames.txt");
	string temp;
	while (getline(ifs, temp))
	{
		//txtfilenames.push_back(path + "\\" + temp);
		global_txtfilenames.push_back(temp);
	}

	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	

	/////////////////////////
	//compute voronoi diagram
	/////////////////////////
	//global_pOW2Doc = new COW2Doc(global_hwnd);
	global_pOW2Doc = new CAutoRescaleOW2Doc(global_hwnd);
	global_pOW2View = new COW2View(global_hwnd, (COWDocument*)global_pOW2Doc);
	global_pOW2View->OnCreate(NULL);
	if (global_pOW2Doc != NULL && global_pOW2View != NULL)
	{
		bool bresult = global_pOW2Doc->OnOpenDocument(L"background.jpg");

		if (bresult)
		{
			global_pOW2View->OnInitialUpdate();

			InvalidateRect(global_hwnd, NULL, false);
			global_pOW2Doc->OnAnalysisTextureextraction();

			//global_pOW2View->m_pViewDataSettings->bDrawPointset = TRUE;
			global_pOW2View->m_pViewDataSettings->bDrawPointset = FALSE;
			global_pOW2View->m_pViewDataSettings->bDrawVoronoiset = TRUE;
			InvalidateRect(global_hwnd, NULL, false);

			//spi, avril 2015, begin
			//string fullpath = "c:\\temp\\";
			//_mkdir(fullpath.c_str());
			//string base_filename = global_txtfilenames[global_imageid].substr(global_txtfilenames[global_imageid].find_last_of("/\\") + 1);
			//fullpath = fullpath + base_filename;
			global_pOW2Doc->SaveVoronoiDiagramColoredWithPixelsetAsImagefile(L"output.jpg", global_pOW2View);
			//spi, avril 2015, end
		}
	}


	global_timer=timeSetEvent(500,25,(LPTIMECALLBACK)&StartGlobalProcess,0,TIME_ONESHOT);
	return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HGDIOBJ hOldBrush;
	HGDIOBJ hOldPen;
	int iOldMixMode;
	COLORREF crOldBkColor;
	COLORREF crOldTextColor;
	int iOldBkMode;
	HFONT hOldFont, hFont;
	TEXTMETRIC myTEXTMETRIC;

	static HGLRC hRC; //returned by opengl InitGL()


	switch (message)
	{
	case WM_CREATE:
		{
			/*
			HWND hStatic = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTER,  
				0, 100, 100, 100, hWnd, (HMENU)IDC_MAIN_STATIC, GetModuleHandle(NULL), NULL);
			*/
			HWND hStatic = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"", WS_CHILD | WS_VISIBLE | global_staticalignment,  
				0, 100, 100, 100, hWnd, (HMENU)IDC_MAIN_STATIC, GetModuleHandle(NULL), NULL);
			if(hStatic == NULL)
				MessageBox(hWnd, L"Could not create static text.", L"Error", MB_OK | MB_ICONERROR);
			SendMessage(hStatic, WM_SETFONT, (WPARAM)global_hFont, MAKELPARAM(FALSE, 0));

			InitGL(hWnd, global_hDC, hRC);
		}
		break;
	case WM_SIZE:
		{
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			HWND hStatic = GetDlgItem(hWnd, IDC_MAIN_STATIC);
			global_staticwidth = rcClient.right - 0;
			//global_staticheight = rcClient.bottom-(rcClient.bottom/2);
			global_staticheight = rcClient.bottom-0;
			//spi, begin
			global_imagewidth = rcClient.right - 0;
			global_imageheight = rcClient.bottom - 0; 
			WavSetLib_Initialize(global_hwnd, IDC_MAIN_STATIC, global_staticwidth, global_staticheight, global_fontwidth, global_fontheight, global_staticalignment);
			//spi, end
			//SetWindowPos(hStatic, NULL, 0, rcClient.bottom/2, global_staticwidth, global_staticheight, SWP_NOZORDER);
			SetWindowPos(hStatic, NULL, 0, 0, global_staticwidth, global_staticheight, SWP_NOZORDER);

			reshapeFunc(LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_CTLCOLOREDIT:
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
			SetTextColor((HDC)wParam, RGB(0xFF, 0xFF, 0xFF));
			return (INT_PTR)::GetStockObject(NULL_PEN);
		}
		break;
	case WM_CTLCOLORSTATIC:
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
			//SetTextColor((HDC)wParam, RGB(0xFF, 0xFF, 0xFF));
			SetTextColor((HDC)wParam, global_statictextcolor);
			return (INT_PTR)::GetStockObject(NULL_PEN);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_ERASEBKGND:
		{
		}
		break;
	case WM_KEYDOWN:
		if (wParam == 'N')
		{
			SwitchVoronoiDiagram();
			return TRUE;
		}
		else if (wParam == 'S')
		{
			//toggle skipsaveframes
			if (skipsaveframes) skipsaveframes = false;
			else skipsaveframes = true;
		}
		break;	
	case WM_PAINT:
		
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		/*
		SetStretchBltMode(hdc, COLORONCOLOR);
		//spi, begin
		StretchDIBits(hdc, 0, 0, global_imagewidth, global_imageheight,
						0, 0, FreeImage_GetWidth(global_dib), FreeImage_GetHeight(global_dib),
						FreeImage_GetBits(global_dib), FreeImage_GetInfo(global_dib), DIB_RGB_COLORS, SRCCOPY);
		//spi, end
		hOldBrush = SelectObject(hdc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		hOldPen = SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
		//iOldMixMode = SetROP2(hdc, R2_MASKPEN);
		iOldMixMode = SetROP2(hdc, R2_MERGEPEN);
		//Rectangle(hdc, 100, 100, 200, 200);

		crOldBkColor = SetBkColor(hdc, RGB(0xFF, 0x00, 0x00));
		crOldTextColor = SetTextColor(hdc, RGB(0xFF, 0xFF, 0xFF));
		iOldBkMode = SetBkMode(hdc, TRANSPARENT);
		//hFont=CreateFontW(70,0,0,0,FW_BOLD,0,0,0,0,0,0,2,0,L"SYSTEM_FIXED_FONT");
		//hOldFont=(HFONT)SelectObject(hdc,global_hFont);
		hOldFont=(HFONT)SelectObject(hdc,global_hFont);
		GetTextMetrics(hdc, &myTEXTMETRIC);
		global_fontwidth = myTEXTMETRIC.tmAveCharWidth;
		//TextOutW(hdc, 100, 100, L"test string", 11);

		SelectObject(hdc, hOldBrush);
		SelectObject(hdc, hOldPen);
		SetROP2(hdc, iOldMixMode);
		SetBkColor(hdc, crOldBkColor);
		SetTextColor(hdc, crOldTextColor);
		SetBkMode(hdc, iOldBkMode);
		SelectObject(hdc,hOldFont);
		//DeleteObject(hFont);
		*/
		displayFunc(hdc);

		//spi, begin, frames modif
		if (!skipsaveframes) saveframetobmpfile(hWnd, hdc);
		//spi, end

		EndPaint(hWnd, &ps);
		


		break;
	case WM_DESTROY:
		{
			if (global_timer) timeKillEvent(global_timer);
			global_abort = true;
			//Sleep(2000); //should be the same as the sleep time in the StartGlobalProcess() callback function
			WavSetLib_Terminate();
			FreeImage_Unload(global_dib);
			DeleteObject(global_hFont);
			//if(global_pFILE) fclose(global_pFILE);
			global_ifstream.close();

			if (pFILE) fclose(pFILE); //added by spi


			if (global_pSynth)
			{
				delete global_pSynth;
			}
			
			if (global_pOW2Doc) delete global_pOW2Doc;
			if (global_pOW2View) delete global_pOW2View;
			
			CloseGL(hWnd, global_hDC, hRC);

			delete callbackFilledAudioBuffers;
			delete drawnFromAudioBuffers;
			delete specDisplayMix;
			delete waveformDisplayL;
			delete autocorrDisplayL;
			delete autocorrHandler;
			delete[] autocorrBuffer;
			for (int i = 0; i < trailCount; i++)
			{
				delete[] displayPointsHistory1[i];
				delete[] displayPointsHistory2[i];
			}
			delete[] displayPointsHistory1;
			delete[] displayPointsHistory2;
			for (int i = 0; i < waterfallCount; i++)
			{
				delete[] waterfallDisplayYValuesHistory[i];
			}
			delete[] waterfallDisplayYValuesHistory;


			//int nShowCmd = false;
			//ShellExecuteA(NULL, "open", "end.bat", "", NULL, nShowCmd);
			ShellExecuteA(NULL, "open", global_end.c_str(), "", NULL, 0);
			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc(GLsizei w, GLsizei h)
{
	// save the new window size
	//g_windowWidth = w; g_windowHeight = h;
	global_xwidth = w; global_yheight = h;
	// map the view port to the client area
	glViewport(0, 0, w, h);

	if (show3D)
	{
		// set the matrix mode to project
		glMatrixMode(GL_PROJECTION);
		// load the identity matrix
		glLoadIdentity();
		// create the viewing frustum
		gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 300.0);
		// set the matrix mode to modelview
		glMatrixMode(GL_MODELVIEW);
		// load the identity matrix
		glLoadIdentity();
		// position the view point
		gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	}
	else
	{
		/* //no glutSwapBuffers() function available
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glutSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glutSwapBuffers();
		glMatrixMode(GL_PROJECTION);
		// load the identity matrix
		glLoadIdentity();
		// set to ortho for 2D version
		gluOrtho2D(-w / 2, w / 2, -h / 2, h / 2);
		// set the matrix mode to modelview
		glMatrixMode(GL_MODELVIEW);
		// load the identity matrix
		glLoadIdentity();
		// position the view point
		gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		*/
	}
}


//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc(HDC hdc)
{
	// ---------------- Wait and Grab New Audio  ---------------------- 

	//Don't do anything until we have new audio data    
	//while( !g_ready ) usleep( 1000 );
	//while (!g_ready) Sleep(10); //spi
	while( !g_ready ) Sleep( 10 ); //spi

	//Lock while copying so the contents of the callbackFilledAudioBuffers don't change during copy
	if(0) pthread_mutex_lock(&g_mutex);

	//Copy the latest audio buffers filled by the audio callback function
	drawnFromAudioBuffers->copyInBuffersFromHandler(callbackFilledAudioBuffers);
	//mark that we don't have new audio data yet
	g_ready = false;

	if(0) pthread_mutex_unlock(&g_mutex);


	// -------------------- Draw 3D  -------------------------- 	
	if (show3D)
	{

		// -------------------- Draw the Coolness  -------------------------- 	

		//Reset viewport and projection if we're drawing windows
		if (showWindows)
		{
			// map the view port to the client area
			glViewport(0, 0, global_xwidth, global_yheight);
			// set the matrix mode to project
			glMatrixMode(GL_PROJECTION);
			// load the identity matrix
			glLoadIdentity();
			// create the viewing frustum
			gluPerspective(45.0, (GLfloat)global_xwidth / (GLfloat)global_yheight, 1.0, 300.0);
		}

		// set the matrix mode to modelview
		glMatrixMode(GL_MODELVIEW);
		// load the identity matrix
		glLoadIdentity();
		// position the view point - view rotates around
		gluLookAt(5.0*sin(theta), 5.0*sin(theta), 10.0*cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		//theta += MY_PI / (6*43.06); //original
		//theta = rand()*MY_PI / (6*43.06); //spi
		//theta += 0; //spi
		theta += MY_PI / (6 * 10 * 43.06); //spi
		global_thetacounter++;
		if (global_thetacounter > 150)
		{
			theta = rand()*MY_PI / (6 * 43.06); //spi
			global_thetacounter = 0;
		}
		// clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glPushMatrix();

		//copy out Left channel buffer
		drawnFromAudioBuffers->copyOutChannelBuffer(autocorrBuffer, 0);

		//update read index and and calculate autocorrelation
		trailReadIndex = trailWriteIndex;
		autocorrHandler->autocorr(autocorrBuffer, autocorrBuffer);

		//Calculate the plot point locations for the lines to be drawn, given the just calculated autocorrelation function
		//Spacing 1 (white color)
		for (GLint i = 0; i < autocorrHandler->getN() - autocorrSkip1 * 5 - 4; i = i + 2)
		{
			displayPointsHistory1[trailWriteIndex][i].x = scaleFactor3D * autocorrBuffer[i];
			displayPointsHistory1[trailWriteIndex][i].y = -scaleFactor3D * autocorrBuffer[i + autocorrSkip1];
			displayPointsHistory1[trailWriteIndex][i].z = scaleFactor3D * autocorrBuffer[i + 2 * autocorrSkip1];

			displayPointsHistory1[trailWriteIndex][i + 1].x = scaleFactor3D * autocorrBuffer[i + 3 * autocorrSkip1];
			displayPointsHistory1[trailWriteIndex][i + 1].y = -scaleFactor3D * autocorrBuffer[i + 4 * autocorrSkip1];
			displayPointsHistory1[trailWriteIndex][i + 1].z = scaleFactor3D * autocorrBuffer[i + 5 * autocorrSkip1];
		}

		//Spacing 2 (blue color)
		for (GLint i = 0; i < autocorrHandler->getN() - autocorrSkip2 * 5 - 4; i = i + 2)
		{
			displayPointsHistory2[trailWriteIndex][i].x = scaleFactor3D * autocorrBuffer[i];
			displayPointsHistory2[trailWriteIndex][i].y = -scaleFactor3D * autocorrBuffer[i + autocorrSkip2];
			displayPointsHistory2[trailWriteIndex][i].z = scaleFactor3D * autocorrBuffer[i + 2 * autocorrSkip2];

			displayPointsHistory2[trailWriteIndex][i + 1].x = scaleFactor3D * autocorrBuffer[i + 3 * autocorrSkip2];
			displayPointsHistory2[trailWriteIndex][i + 1].y = -scaleFactor3D * autocorrBuffer[i + 4 * autocorrSkip2];
			displayPointsHistory2[trailWriteIndex][i + 1].z = scaleFactor3D * autocorrBuffer[i + 5 * autocorrSkip2];
		}

		//Decrement write pointer (displayPointsHistory keeps a circular buffer)
		trailWriteIndex--;
		if (trailWriteIndex < 0)
			trailWriteIndex = trailCount - 1;

		//Set line width
		glLineWidth(1.1);	//original 	
		//glLineWidth( 5.5 );	//spi 	
		//glLineWidth( 0.5 );	//spi 	
		//glLineWidth( 20.0 );	//spi 	
		//glLineWidth( 100.0 );	//spi 	
		//glPointSize(50.0f); //spi
		//glPointSize(10.0f); //spi
		glPointSize(1.0f); //spi

		glPushMatrix();

		//spi, begin
		Point3D displaypointshistory1maxamplitude = { 0.0, 0.0, 0.0 };
		Point3D displaypointshistory2maxamplitude = { 0.0, 0.0, 0.0 };
		//spi, end

		int drawIndex;

		//draw from oldest to newest (actually just need to make sure we set the alpha to the lowest on the oldest plot)
		for (int plotNum = trailCount - 1; plotNum > -1; plotNum--)
		{
			if (trailReadIndex + plotNum >= trailCount)
				drawIndex = trailReadIndex + plotNum - trailCount;
			else
				drawIndex = trailReadIndex + plotNum;

			//Set color to blue, alpha geometrically decreasing for older plots
			//	   			glColor4f(0.0 , 136/255.0, 158/255.0, pow(0.8, plotNum)*0.7); //2048 buffersize
			//	   			glColor4f(0.0 , 136/255.0, 158/255.0, pow(0.89, plotNum)*0.7); //1024 buffersize //original
			glColor4f(136 / 255.0, 0.0, 158 / 255.0, pow(0.89, plotNum)*0.7); //1024 buffersize //spi

			//Plot the blue timeSkip2 lines - these points were ordered previously when calculating points from the autocorrelation function
			//glBegin( GL_LINES ); //original
			//glBegin(GL_POINTS); //spi
			for (GLint i = 0; i < autocorrHandler->getN() - autocorrSkip2 * 5 - 4; i++)
			{
				//glVertex3f(displayPointsHistory2[drawIndex][i].x, displayPointsHistory2[drawIndex][i].y, displayPointsHistory2[drawIndex][i].z); //original
				//spi, begin
				if (displayPointsHistory2[drawIndex][i].x>displaypointshistory2maxamplitude.x) displaypointshistory2maxamplitude.x = displayPointsHistory2[drawIndex][i].x;
				if (displayPointsHistory2[drawIndex][i].y>displaypointshistory2maxamplitude.y) displaypointshistory2maxamplitude.y = displayPointsHistory2[drawIndex][i].y;
				if (displayPointsHistory2[drawIndex][i].z>displaypointshistory2maxamplitude.z) displaypointshistory2maxamplitude.z = displayPointsHistory2[drawIndex][i].z;
				//spi, end
			}
			//glEnd();

			//Set color to white, alpha geometrically decreasing for older plots
			//	   			glColor4f(1.0, 1.0, 1.0, pow(0.8, plotNum)*0.8);	//2048 buffersize
			//	   			glColor4f(1.0, 1.0, 1.0, pow(0.88, plotNum)*0.8); //original
			glColor4f(0.0, 0.0, 1.0, pow(0.88, plotNum)*0.8); //spi

			//Plot the white timeSkip1 lines - these points were ordered previously when calculating points from the autocorrelation function   			
			//glBegin( GL_LINES ); //original
			//glBegin(GL_POINTS); //spi
			for (GLint i = 0; i < autocorrHandler->getN() - autocorrSkip1 * 5 - 4; i++)
			{
				//glVertex3f(displayPointsHistory1[drawIndex][i].x, displayPointsHistory1[drawIndex][i].y, displayPointsHistory1[drawIndex][i].z); //original
				//spi, begin
				if (displayPointsHistory1[drawIndex][i].x>displaypointshistory1maxamplitude.x) displaypointshistory1maxamplitude.x = displayPointsHistory1[drawIndex][i].x;
				if (displayPointsHistory1[drawIndex][i].y>displaypointshistory1maxamplitude.y) displaypointshistory1maxamplitude.y = displayPointsHistory1[drawIndex][i].y;
				if (displayPointsHistory1[drawIndex][i].z>displaypointshistory1maxamplitude.z) displaypointshistory1maxamplitude.z = displayPointsHistory1[drawIndex][i].z;
				//spi, end
			}
			//glEnd();
		}

		//spi, begin
		//draw voronoi (only is pointset present)
		POINTSET* pPS = NULL;
		if (global_pOW2Doc && global_pOW2Doc->m_pPointset && global_pOW2Doc->m_pPointset->m_pPOINTSET)
		{
			pPS = global_pOW2Doc->m_pPointset->m_pPOINTSET;
		}
		if (pPS)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			//////////////////////
			//draw voronoi regions
			//////////////////////
			int numtrifound, numneighborfound, itriseed;
			int p_arraytri[200];
			int p_arrayneighbor[200];

			// the triangle seed must be an existing triangle index 
			itriseed = 0;
			for (int ivertex = 0; ivertex < (pPS->npts); ivertex++)
			{
				//skip drawing one out of 2 times
				if (0)
				{
					//int dodraw = rand() % 2;
					//int dodraw = rand() % 200;
					int dodraw = rand() % 5;
					if (dodraw != 1) continue;
				}
				//glColor3f(pPS->color_r[ivertex], pPS->color_g[ivertex], pPS->color_b[ivertex]);
				glColor4f(pPS->color_r[ivertex], pPS->color_g[ivertex], pPS->color_b[ivertex], 150./255.);
				glBegin(GL_POLYGON);
				//
				//Pour chacun des points qui n'ont aucun triangle adjacents
				//touchant directement la frontiere convexe ...
				//
				float randomz = RandomFloat(0.0, 1.0)*displaypointshistory1maxamplitude.z;
				if (FindAllValidTriSurroundingVertex(pPS,
					ivertex,
					&itriseed,
					&numtrifound,
					p_arraytri,
					&numneighborfound,
					p_arrayneighbor) == TRUE)
				{
					//if all surrounding triangles are valid,		 
					// build voronoi polygon using each adjtri's center 
					//ASSERT(numtrifound<200); //development-time error, if allowed, define p_arraytri[n] with n>200
					for (int j = 0; j < numtrifound; j++)
					{
						//glVertex3d(pPS->ctx[p_arraytri[j]] / 192.0, pPS->cty[p_arraytri[j]] / 108.0, 0.0); //static
						//glVertex3d(pPS->ctx[p_arraytri[j]] * displaypointshistory1maxamplitude.x / 192.0, pPS->cty[p_arraytri[j]] * displaypointshistory1maxamplitude.y / 108.0, displaypointshistory1maxamplitude.z); //dynamic in x, y and z
						//glVertex3d((pPS->ctx[p_arraytri[j]]-pPS->xmax/2.0) * displaypointshistory1maxamplitude.x / 192.0, (pPS->cty[p_arraytri[j]]-pPS->ymax/2.0) * displaypointshistory1maxamplitude.y / 108.0, 0.0); //dynamic in x and y
						//for a background.jpg of 3648x2736
						//glVertex3d((pPS->ctx[p_arraytri[j]] - pPS->xmax / 2.0) * scaleFactor3D / 19.20, (pPS->cty[p_arraytri[j]] - pPS->ymax / 2.0) * scaleFactor3D / 10.80 / 2.0, 0.0); //dynamic in x and y
						//for a background.jpg of arbitrary size
						//glVertex3d((pPS->ctx[p_arraytri[j]] - pPS->xmax / 2.0) * scaleFactor3D / (20.0*pPS->xmax / 3648.0), (pPS->cty[p_arraytri[j]] - pPS->ymax / 2.0) * scaleFactor3D / (20.0*pPS->ymax / 2736.0), 0.0); //dynamic in x and y
						glVertex3d((pPS->ctx[p_arraytri[j]] - pPS->xmax / 2.0) * scaleFactor3D / (20.0*pPS->xmax / 3648.0), (pPS->cty[p_arraytri[j]] - pPS->ymax / 2.0) * scaleFactor3D / (20.0*pPS->ymax / 2736.0), RandomFloat(0.0, 1.0)*displaypointshistory1maxamplitude.z); //dynamic in x and y
						//glVertex3d((pPS->ctx[p_arraytri[j]] - pPS->xmax / 2.0) * scaleFactor3D / (20.0*pPS->xmax / 3648.0), (pPS->cty[p_arraytri[j]] - pPS->ymax / 2.0) * scaleFactor3D / (20.0*pPS->ymax / 2736.0), randomz); //dynamic in x and y
						//glVertex3d((pPS->ctx[p_arraytri[j]] - pPS->xmax / 2.0) * scaleFactor3D / (20.0*pPS->xmax / 3648.0), (pPS->cty[p_arraytri[j]] - pPS->ymax / 2.0) * scaleFactor3D / (20.0*pPS->ymax / 2736.0), displaypointshistory1maxamplitude.z); //dynamic in x and y
					}
				}
				else
				{
				}
				glEnd();
			}
		}
		//spi, end

		//spi, begin
		////////////////////////////////////
		//now, draw isolated voronoi polygon
		////////////////////////////////////
		if (pPS)
		{
			glColor4f(1.0f, 0.0f, 0.0f, 10.f);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			//////////////////////
			//draw voronoi regions
			//////////////////////
			int numtrifound, numneighborfound, itriseed;
			int p_arraytri[200];
			int p_arrayneighbor[200];

			// the triangle seed must be an existing triangle index 
			itriseed = 0;

			//draw from oldest to newest (actually just need to make sure we set the alpha to the lowest on the oldest plot)
			for (int plotNum = trailCount - 1; plotNum > -1; plotNum--)
			{
				if (trailReadIndex + plotNum >= trailCount)
					drawIndex = trailReadIndex + plotNum - trailCount;
				else
					drawIndex = trailReadIndex + plotNum;

				//Set color to blue, alpha geometrically decreasing for older plots
				//glColor4f(0.0 , 136/255.0, 158/255.0, pow(0.8, plotNum)*0.7); //2048 buffersize
				//glColor4f(0.0 , 136/255.0, 158/255.0, pow(0.89, plotNum)*0.7); //1024 buffersize //original
				//glColor4f(136 / 255.0, 0.0, 158 / 255.0, pow(0.89, plotNum)*0.7); //1024 buffersize //spi

				//Plot the blue timeSkip2 lines - these points were ordered previously when calculating points from the autocorrelation function
				//glBegin( GL_LINES ); //original
				//glBegin(GL_POINTS); //spi
				for (GLint i = 0; i < autocorrHandler->getN() - autocorrSkip2 * 5 - 4; i++)
				{
					//glVertex3f(displayPointsHistory2[drawIndex][i].x, displayPointsHistory2[drawIndex][i].y, displayPointsHistory2[drawIndex][i].z);
					//now, instead of drawing each point ...
					//draw nearest voronoi polygon
					//get nearest pointset object (idvertex, also equals the idstartsegment)
					//idPointsetObject = global_pOW2Doc->GetNearestPointsetObject(displayPointsHistory2[drawIndex][i].x, displayPointsHistory2[drawIndex][i].y, &itriseed);
					int ivertex = global_pOW2Doc->GetNearestPointsetObject((displayPointsHistory2[drawIndex][i].x+0.5)*pPS->xmax, (displayPointsHistory2[drawIndex][i].y+0.5)*pPS->ymax, &itriseed);
					if (ivertex != -1)
					{
						//glColor4f(pPS->color_r[ivertex], pPS->color_g[ivertex], pPS->color_b[ivertex], pow(0.89, plotNum)*0.7);
						//glColor4f(pPS->color_r[ivertex], pPS->color_g[ivertex], pPS->color_b[ivertex], 100./255.);
						glColor4f(pPS->color_r[ivertex], pPS->color_g[ivertex], pPS->color_b[ivertex], 50. / 255.);
						glBegin(GL_POLYGON);
						//
						//Pour chacun des points qui n'ont aucun triangle adjacents
						//touchant directement la frontiere convexe ...
						//

						if (FindAllValidTriSurroundingVertex(pPS,
							ivertex,
							&itriseed,
							&numtrifound,
							p_arraytri,
							&numneighborfound,
							p_arrayneighbor) == TRUE)
						{
							//if all surrounding triangles are valid,		 
							// build voronoi polygon using each adjtri's center 
							//ASSERT(numtrifound<200); //development-time error, if allowed, define p_arraytri[n] with n>200
							for (int j = 0; j < numtrifound; j++)
							{
								//glVertex3d(pPS->ctx[p_arraytri[j]] / 192.0, pPS->cty[p_arraytri[j]] / 108.0, 0.0); //static
								//glVertex3d(pPS->ctx[p_arraytri[j]] * displaypointshistory1maxamplitude.x / 192.0, pPS->cty[p_arraytri[j]] * displaypointshistory1maxamplitude.y / 108.0, displaypointshistory1maxamplitude.z); //dynamic in x, y and z
								//glVertex3d((pPS->ctx[p_arraytri[j]]-pPS->xmax/2.0) * displaypointshistory1maxamplitude.x / 192.0, (pPS->cty[p_arraytri[j]]-pPS->ymax/2.0) * displaypointshistory1maxamplitude.y / 108.0, 0.0); //dynamic in x and y
								//for a background.jpg of 3648x2736
								//glVertex3d((pPS->ctx[p_arraytri[j]] - pPS->xmax / 2.0) * scaleFactor3D / 19.20, (pPS->cty[p_arraytri[j]] - pPS->ymax / 2.0) * scaleFactor3D / 10.80 / 2.0, 0.0); //dynamic in x and y
								//glVertex3d((pPS->ctx[p_arraytri[j]] - pPS->xmax / 2.0) * scaleFactor3D / 19.20, (pPS->cty[p_arraytri[j]] - pPS->ymax / 2.0) * scaleFactor3D / 10.80 / 2.0, displayPointsHistory2[drawIndex][i].z); //dynamic in x and y
								//for a background.jpg of arbitrary size
								glVertex3d((pPS->ctx[p_arraytri[j]] - pPS->xmax / 2.0) * scaleFactor3D / (20.0*pPS->xmax / 3648.0), (pPS->cty[p_arraytri[j]] - pPS->ymax / 2.0) * scaleFactor3D / (20.0*pPS->ymax / 2736.0), displayPointsHistory2[drawIndex][i].z); //dynamic in x and y

							}
						}
						else
						{
						}
						glEnd();
					}

				}
				//glEnd();

				/*
				//Set color to white, alpha geometrically decreasing for older plots
				//glColor4f(1.0, 1.0, 1.0, pow(0.8, plotNum)*0.8);	//2048 buffersize
				//glColor4f(1.0, 1.0, 1.0, pow(0.88, plotNum)*0.8); //original
				glColor4f(0.0, 0.0, 1.0, pow(0.88, plotNum)*0.8); //spi

				//Plot the white timeSkip1 lines - these points were ordered previously when calculating points from the autocorrelation function   			
				//glBegin( GL_LINES ); //original
				glBegin(GL_POINTS); //spi
				for (GLint i = 0; i < autocorrHandler->getN() - autocorrSkip1 * 5 - 4; i++)
				{
					glVertex3f(displayPointsHistory1[drawIndex][i].x, displayPointsHistory1[drawIndex][i].y, displayPointsHistory1[drawIndex][i].z);
				}
				glEnd();
				*/
			}

		}
		//spi, end

		glPopMatrix();

		glPopMatrix();


		// ---------------------- Coolness end ---------------------------	 	  


		// -------------------- Draw windows --------------------------
		if (showWindows)
		{

			// ---------------- Autocorrelation window ----------------
			glPushMatrix();

			// map the view port to the autocorr window location
			glViewport(20, 10, 400, 100);
			// set the matrix mode to project
			glMatrixMode(GL_PROJECTION);
			// load the identity matrix
			glLoadIdentity();
			// working in ortho mode for this line plot
			gluOrtho2D(0, 400, 0, 100);
			// set the matrix mode to modelview
			glMatrixMode(GL_MODELVIEW);
			// load the identity matrix
			glLoadIdentity();
			// position the view point
			gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

			// Draw black mostly opaque background
			glColor4f(0.0, 0.0, 0.0, 0.85);

			glBegin(GL_QUADS);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 100.0, 0.0);
			glVertex3f(400.0, 100.0, 0.0);
			glVertex3f(400.0, 0.0, 0.0);
			glEnd();

			//Grab left channel audio from the buffer and generate the display from this buffer
			//  (calculates autocorrelation and scales the output to fit the given space, with specified clipping limits)
			drawnFromAudioBuffers->copyOutChannelBuffer(autocorrDisplayL->realValuedInputBuffer, 0);
			autocorrDisplayL->generateDisplayFromBuffer();

			//Get pointers to the display coordinate values found above
			const float* xVals = autocorrDisplayL->getDisplayXValuesPointer();
			const float* yVals = autocorrDisplayL->getDisplayYValuesPointer();

			glColor4f(1.0, 1.0, 1.0, 1.0);

			//Plot the line
			glPushMatrix();
			glBegin(GL_LINE_STRIP);
			for (GLint i = 0; i < autocorrDisplayL->getPlotLength(); i++)
				glVertex2f(*xVals++, *yVals++);
			glEnd();
			glPopMatrix();

			glPopMatrix();

			// ----------------- Autocorr window  end --------------------	

			// ------------------ Waveform window ------------------------

			glPushMatrix();

			// map the view port to the client area
			glViewport(440, 10, 400, 100);
			// set the matrix mode to project
			glMatrixMode(GL_PROJECTION);
			// load the identity matrix
			glLoadIdentity();
			// working in ortho mode for this line plot
			gluOrtho2D(0, 400, 0, 100);
			// set the matrix mode to modelview
			glMatrixMode(GL_MODELVIEW);
			// load the identity matrix
			glLoadIdentity();
			// position the view point
			gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

			// Draw black mostly opaque background
			glColor4f(0.0, 0.0, 0.0, 0.85);
			glBegin(GL_QUADS);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 100.0, 0.0);
			glVertex3f(400.0, 100.0, 0.0);
			glVertex3f(400.0, 0.0, 0.0);
			glEnd();


			//Grab left channel audio from the buffer and generate the display from this buffer
			//  (scales the waveform to fit the given space, with specified clipping limits)
			drawnFromAudioBuffers->copyOutChannelBuffer(waveformDisplayL->realValuedInputBuffer, 0);
			waveformDisplayL->generateDisplayFromBuffer();

			//Get pointers to the display coordinate values found above			
			xVals = waveformDisplayL->getDisplayXValuesPointer();
			yVals = waveformDisplayL->getDisplayYValuesPointer();


			glColor3f(1.0, 1.0, 1.0);

			//Plot the line
			glPushMatrix();
			glBegin(GL_LINE_STRIP);
			for (GLint i = 0; i < waveformDisplayL->getPlotLength(); i++)
				glVertex2f(*xVals++, *yVals++);
			glEnd();
			glPopMatrix();

			glPopMatrix();

			// --------------- Spectrum window end ---------------------	

			// ---------------- Waterfall window -----------------------	

			glPushMatrix();

			// map the view port to the client area
			glViewport(860, 10, 400, 100);
			// set the matrix mode to project
			glMatrixMode(GL_PROJECTION);
			// load the identity matrix
			glLoadIdentity();
			// working in ortho for this line plot
			gluOrtho2D(0, 400, 0, 100);
			// set the matrix mode to modelview
			glMatrixMode(GL_MODELVIEW);
			// load the identity matrix
			glLoadIdentity();
			// position the view point
			gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

			// Draw black mostly opaque background
			glColor4f(0.0, 0.0, 0.0, 0.85);
			glBegin(GL_QUADS);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 100.0, 0.0);
			glVertex3f(400.0, 100.0, 0.0);
			glVertex3f(400.0, 0.0, 0.0);
			glEnd();


			glPushMatrix();

			glViewport(870, 13, 380, 88);
			//Switch back to perspective mode, within this window, in order to plot the waterfall with 3D perspective
			glMatrixMode(GL_PROJECTION);
			// load the identity matrix
			glLoadIdentity();
			// create the viewing frustum
			gluPerspective(45.0, (GLfloat)global_xwidth / (GLfloat)global_yheight, 1.0, 300.0);
			// set the matrix mode to modelview
			glMatrixMode(GL_MODELVIEW);
			// load the identity matrix
			glLoadIdentity();
			// position the view point
			gluLookAt(0.0f, 3.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);


			//Update the read index and store the last display  values in the circular history buffer
			waterfallReadIndex = waterfallWriteIndex;
			specDisplayMix->copyDisplayYValues(waterfallDisplayYValuesHistory[waterfallWriteIndex--]);

			if (waterfallWriteIndex < 0)
				waterfallWriteIndex = waterfallCount - 1;

			//Get a mix of the channels and generate the spectral display from it with the specified parameters
			SAMPLE mixCoeffs[2] = { 0.707, 0.707 };
			drawnFromAudioBuffers->copyOutMixOfChannelBuffers(specDisplayMix->realValuedInputBuffer, mixCoeffs);
			specDisplayMix->generateDisplayFromBuffer();

			//Get pointers to the display coordinate values found above	
			xVals = specDisplayMix->getDisplayXValuesPointer();
			yVals = specDisplayMix->getDisplayYValuesPointer();

			//Plot current spectrum
			glBegin(GL_LINE_STRIP);
			for (GLint i = 0; i < specDisplayMix->getPlotLength(); i++)
			{
				glVertex3f(xVals[i], yVals[i], 0.0);
			}
			glEnd();


			//Plot waterfall - plot from oldest to newest so we can get the darkness of the lines right
			int drawIndex;
			for (int plotNum = waterfallCount - 1; plotNum >= 0; plotNum--)
			{
				if (waterfallReadIndex + plotNum >= waterfallCount)
					drawIndex = waterfallReadIndex + plotNum - waterfallCount;
				else
					drawIndex = waterfallReadIndex + plotNum;

				//Older plots are darker
				glColor3f(1.0 - 2.0*plotNum*(1.0 / waterfallCount), 1.0 - 2.0*plotNum*(1.0 / waterfallCount), 1.0 - 2.0*plotNum*(1.0 / waterfallCount));

				glBegin(GL_LINE_STRIP);
				for (GLint i = 0; i < specDisplayMix->getPlotLength(); i++)
				{
					glVertex3f(xVals[i], waterfallDisplayYValuesHistory[drawIndex][i], (plotNum + 1) * -1.0);
				}
				glEnd();
			}

			glPopMatrix();

			glPopMatrix();

			// ---------------- Waterfall window end ----------------------						
		}

		// ------------------ Draw windows end ------------------------

		// flush!
		glFlush();
		
		/* //not available without glut
		// swap the double buffer
		glutSwapBuffers();
		*/
		SwapBuffers(hdc);

	}
	// ------------------ Draw 3D end ------------------------				  

	// -------------------- Draw 2D --------------------------
	else
	{
		//We're doing alpha fading by drawing an mostly transparent black rectangle over the whole scene between frames.
		//	This really should use single buffering, but since the program should be double buffered to support the 3D
		//	version, we just draw the same thing twice here to put it on both buffers. This seems to work okay.
		for (int buffer = 0; buffer < 2; buffer++)
		{

			//alpha fade what's there
			//		2048 buffer -> 0.15
			glColor4f(0.0, 0.0, 0.0, 0.04);
			glBegin(GL_QUADS);
			glVertex3f(-global_xwidth / 2, -global_yheight / 2, 0.0);
			glVertex3f(-global_xwidth / 2, global_yheight / 2, 0.0);
			glVertex3f(global_xwidth / 2, global_yheight / 2, 0.0);
			glVertex3f(global_xwidth / 2, -global_yheight / 2, 0.0);
			glEnd();

			// ------------------- Draw 2D Coolness -------------------------	 	  		

			//copy out Left channel buffer and calculate autocorrelation
			drawnFromAudioBuffers->copyOutChannelBuffer(autocorrBuffer, 0);
			autocorrHandler->autocorr(autocorrBuffer, autocorrBuffer);

			glPushMatrix();

			//Plot blue lines
			glLineWidth(1.0);
			glColor4f(0.0, 136 / 255.0, 158 / 255.0, 0.4);

			glBegin(GL_LINES);
			for (GLint i = 0; i < autocorrHandler->getN() - autocorrSkip2 * 3 - 4; i = i + 2)
			{
				glVertex3f(scaleFactor2D * autocorrBuffer[i], -scaleFactor2D * autocorrBuffer[i + autocorrSkip2], 0.0);
				glVertex3f(scaleFactor2D * autocorrBuffer[i + 2 * autocorrSkip2], -scaleFactor2D * autocorrBuffer[i + 3 * autocorrSkip2], 0.0);
			}
			glEnd();

			glPopMatrix();


			glPushMatrix();

			//Plot white lines
			glLineWidth(1.0);
			glColor4f(1.0, 1.0, 1.0, 1.0);

			glBegin(GL_LINES);
			for (GLint i = 0; i < autocorrHandler->getN() - autocorrSkip2 * 3 - 4; i = i + 2)
			{
				glVertex3f(scaleFactor2D * autocorrBuffer[i], -scaleFactor2D * autocorrBuffer[i + autocorrSkip1], 0.0);
				glVertex3f(scaleFactor2D * autocorrBuffer[i + 2 * autocorrSkip1], -scaleFactor2D * autocorrBuffer[i + 3 * autocorrSkip1], 0.0);
			}
			glEnd();

			glPopMatrix();

			// -------------------- 2D Coolness end --------------------------	 	  	  

			// flush!
			glFlush();
			/* //not available without glut
			glutSwapBuffers();
			*/
			SwapBuffers(hdc);
		}

	}

}

// Initialize OpenGL
static void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC)
{
	/*
	///////////////////////
	//Enable alpha blending
	///////////////////////
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	*/

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof pfd);
	pfd.nSize = sizeof pfd;
	pfd.nVersion = 1;
	//pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL; //blaine's
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;

	hDC = GetDC(hWnd);

	int i = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, i, &pfd);

	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

}

// Shut down OpenGL
static void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);

	ReleaseDC(hWnd, hDC);
}


