/*----------------------------------------------------------------------
 *  3DxTest32.c  -- Basic Win32 Program to initinalize a 3D mouse, read
 *                  its data and print it out.
 *
 *  Originally written by Elio Querze
 *
 *  NOTE: MUST LINK WITH SIAPP.LIB
 *
 *----------------------------------------------------------------------
 *
 * Copyright notice:
 * Copyright (c) 1998-2015 3Dconnexion. All rights reserved.
 *
 * This file and source code are an integral part of the "3Dconnexion
 * Software Developer Kit", including all accompanying documentation,
 * and is protected by intellectual property laws. All use of the
 * 3Dconnexion Software Developer Kit is subject to the License
 * Agreement found in the "LicenseAgreementSDK.txt" file.
 * All rights not expressly granted by 3Dconnexion are reserved.
 *
 */

static char CvsId[]="(C) 1997-2015 3Dconnexion: $Id: 3DxTest32.cpp 13022 2016-05-25 15:43:58Z jwick $";


/* Standard Win32 Includes */
#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <crtdbg.h>


/* SpaceWare Specific Includes */
#include "spwmacro.h"  /* Common macros used by SpaceWare functions. */
#include "si.h"        /* Required for any SpaceWare support within an app.*/
#include "siapp.h"     /* Required for siapp.lib symbols */

#include "virtualkeys.hpp"

/* Program Specific Includes */
#include "3DxTest32.h"

/* Global variables */
HDC          hdc;         /* Handle to Device Context used to draw on screen */
HWND         hWndMain;    /* Handle to Main Window */

SiHdl        devHdl;      /* Handle to 3D Mouse Device */
TCHAR devicename[100] = _T("");


/*----------------------------------------------------------------------
* Function: WinMain()
*
* Description:
*    This is the main window function and we use it to initialize data
*    and then call our loop function. This is a std. Win32 function.
*
* Args:
*    HINSTANCE hInstance         // handle to current instance
*    HINSTANCE hPrevInstance     // handle to previous instance
*    LPSTR     lpszCmdLine       // pointer to command line
*    int       nCmdShow          // show state of window
*
* Return Value:
*    int       Returns the return value of DispatchLoopNT
*
*----------------------------------------------------------------------*/
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                LPSTR lpszCmdLine, int nCmdShow )
{
  int  res;            /* SbInits result..if>0 it worked, if=0 it didnt work   */
  int  hsize, vsize;   /* size of window to be created,for each Dimension */

  /* Set Window Size */
  hsize = 280;
  vsize = 215;

  /* create our apps window */
  CreateSPWindow(0, 0, hsize, vsize, _T("3DxTest32"));

  /* update screen */
  InvalidateRect(hWndMain, NULL, FALSE);

  /* Initialize 3D mouse */
  res = SbInit();

  /* if 3D mouse was not detected then print error, close win., exit prog. */
  if (res < 1)
  {
    MessageBox(hWndMain,
      _T("Sorry - No supported 3Dconnexion device available.\n"),
      NULL, MB_OK);
    if (hWndMain != NULL)
    {
      DestroyWindow(hWndMain);    /* destroy window */
    }

    ExitProcess(1);                /* exit program */
  }

  /* Function To be Repeated */
  return(DispatchLoopNT());

}


/*--------------------------------------------------------------------------
* Function: SbInit()
*
* Description:
*    This function initializes the 3D mouse and opens ball for use.
*
*
* Args: None
*
*
* Return Value:
*    int  res         result of SiOpen, =0 if Fail =1 if it Works
*
*--------------------------------------------------------------------------*/
int SbInit()
{
  int res;                             /* result of SiOpen, to be returned  */
  SiOpenData oData;                    /* OS Independent data to open ball  */

  /*init the SpaceWare input library */
  if (SiInitialize() == SPW_DLL_LOAD_ERROR)
  {
    MessageBox(hWndMain,_T("Error: Could not load SiAppDll dll files"),
      NULL, MB_ICONEXCLAMATION);
  }

  SiOpenWinInit (&oData, hWndMain);    /* init Win. platform specific data  */

  /* open data, which will check for device type and return the device handle
  to be used by this function */
  if ( (devHdl = SiOpen ("3DxTest32", SI_ANY_DEVICE, SI_NO_MASK, SI_EVENT, &oData)) == NULL)
  {
    SiTerminate();  /* called to shut down the SpaceWare input library */
    res = 0;        /* could not open device */
    return res;
  }
  else
  {
    SiDeviceName devName;
    SiGetDeviceName(devHdl, &devName);
    _stprintf_s(devicename,SPW_NUM_ELEMENTS_IN(devicename),_T("%S"),devName.name);
    res = 1;        /* opened device succesfully */
	return res;
  }

}


/*--------------------------------------------------------------------------
* Function: DispatchLoopNT()
*
* Description:
*    This function contains the main message loop which constantly checks for
*    3D mouse Events and handles them apropriately.
*
* Args: None
*
*
* Return Value:
*    int  msg.wparam                  // event passed to window
*
*--------------------------------------------------------------------------*/
int DispatchLoopNT()
{
  MSG            msg;      /* incoming message to be evaluated */
  BOOL           handled;  /* is message handled yet */
  SiSpwEvent     Event;    /* SpaceWare Event */
  SiGetEventData EData;    /* SpaceWare Event Data */

  handled = SPW_FALSE;     /* init handled */

  /* start message loop */
  while ( GetMessage( &msg, NULL, 0, 0 ) )
  {
    handled = SPW_FALSE;

    /* init Window platform specific data for a call to SiGetEvent */
    SiGetEventWinInit(&EData, msg.message, msg.wParam, msg.lParam);

    /* check whether msg was a 3D mouse event and process it */
    if (SiGetEvent (devHdl, SI_AVERAGE_EVENTS, &EData, &Event) == SI_IS_EVENT)
    {
      if (Event.type == SI_MOTION_EVENT)
      {
        SbMotionEvent(&Event);        /* process 3D mouse motion event */
      }
      else if (Event.type == SI_ZERO_EVENT)
      {
        SbZeroEvent();                /* process 3D mouse zero event */
      }
      else if (Event.type == SI_BUTTON_PRESS_EVENT)
      {
        SbButtonPressEvent(Event.u.hwButtonEvent.buttonNumber);  /* process button press event */
      }
      else if (Event.type == SI_BUTTON_RELEASE_EVENT)
      {
        SbButtonReleaseEvent(Event.u.hwButtonEvent.buttonNumber); /* process button release event */
      }
      else if (Event.type == SI_DEVICE_CHANGE_EVENT)
      {
        HandleDeviceChangeEvent(&Event); /* process 3D mouse device change event */
      }
      else if (Event.type == SI_CMD_EVENT)
      {
        HandleV3DCMDEvent(&Event); /* V3DCMD_* events */
      }
      else if (Event.type == SI_APP_EVENT)
      {
        HandleAppEvent(&Event); /* V3DCMD_* events */
      }

      handled = SPW_TRUE;              /* 3D mouse event handled */
    }

    /* not a 3D mouse event, let windows handle it */
    if (handled == SPW_FALSE)
    {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
  }

  return( (int) msg.wParam );
}


/*--------------------------------------------------------------------------
* Function: HandleNTEvent
*
* Description:  This is a std. Win32 function to handle various window events
*
*
*
* Args: HWND hWnd                    // handle to window
*       unsigned msg                 // message to process
*       WPARAM wParam                // 32 bit msg parameter
*       LPARAM lParam                // 32 bit msg parameter
*
* Return Value:
*    int  msg.wparam                 // program done
*
*--------------------------------------------------------------------------*/
LRESULT WINAPI HandleNTEvent ( HWND hWnd, unsigned msg, WPARAM wParam,
                              LPARAM lParam )
{
  PAINTSTRUCT ps;           /* used to paint the client area of a window */
  LONG addr;                /* address of our window */

  addr = GetClassLong(hWnd, 0);  /* get address */

  switch ( msg )
  {
  case WM_ACTIVATEAPP:
    hdc = GetDC(hWnd);
    /* print buffers */
    TextOut(hdc,  0 ,  20, devicename, (int)_tcslen(devicename));
    TextOut(hdc, 15 , 100, _T("TXaaa: 0"), 5);
    TextOut(hdc, 15 , 120, _T("TY: 0"), 5);
    TextOut(hdc, 15 , 140, _T("TZ: 0"), 5);
    TextOut(hdc, 15 , 160, _T("RX: 0"), 5);
    TextOut(hdc, 15 , 180, _T("RY: 0"), 5);
    TextOut(hdc, 15 , 200, _T("RZ: 0"), 5);
    TextOut(hdc, 15 , 220, _T(" P: 0"), 5);

    /*release our window handle */
    ReleaseDC(hWnd,hdc);
  case WM_KEYDOWN:
  case WM_KEYUP:
    /* user hit a key to close program */
    if (wParam == VK_ESCAPE)
    {
      SendMessage ( hWndMain, WM_CLOSE, 0, 0l );
    }
    break;

  case WM_PAINT:
    /* time to paint the window */
    if (addr)
    {
      hdc = BeginPaint ( hWndMain, &ps );
      EndPaint ( hWndMain, &ps );
    }

    break;

  case WM_CLOSE:
    /* cleanup the object info created */

    break;

  case WM_DESTROY :
    PostQuitMessage (0);
    return (0);
  }
  return DefWindowProc ( hWnd, msg, wParam, lParam );

}


/*--------------------------------------------------------------------------
* Function: CreateSPWindow
*
* Description:  This creates the window for our app
*
*
*
* Args:  int  atx        // horiz. start point to put window
*        int  aty        // vert.  start point to put window
*        int  hi         // hight of window
*        int  wid        // width of window
*        char *string    // window caption
*
* Return Value:
*    NONE
*
*--------------------------------------------------------------------------*/
void CreateSPWindow (int atx, int aty, int hi, int wid, TCHAR *string)
{
  WNDCLASS  wndclass;     /* our own instance of the window class */
  HINSTANCE hInst;        /* handle to our instance */

  hInst = NULL;             /* init handle */

  /* Register display window class */
  wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
  wndclass.lpfnWndProc   = (WNDPROC)HandleNTEvent ;
  wndclass.cbClsExtra    = 8 ;
  wndclass.cbWndExtra    = 0 ;
  wndclass.hInstance     = hInst;
  wndclass.hIcon         = NULL;
  wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName  = NULL ;
  wndclass.lpszClassName = _T("3DxTest32");

  RegisterClass (&wndclass) ;

  /* create the window */
  hWndMain = CreateWindow ( _T("3DxTest32"),           /*Window class name*/
    string,              /*Window caption*/
    WS_OVERLAPPEDWINDOW, /*Window Style*/
    atx,aty,wid,hi,
    NULL,	               /*parent window handle*/
    NULL,                /*window menu handle*/
    hInst,		         /*program instance handle*/
    NULL);               /*creation parameters*/

  /* display the window */
  ShowWindow ( hWndMain, SW_SHOW );

  /* get handle of our window to draw on */
  hdc = GetDC(hWndMain);

  /* print buffers */
  TextOut(hdc, 0  ,  20, devicename, (int)_tcslen(devicename));
  TextOut(hdc, 15 , 100, _T("TXaaa: 0"), 5);
  TextOut(hdc, 15 , 120, _T("TY: 0"), 5);
  TextOut(hdc, 15 , 140, _T("TZ: 0"), 5);
  TextOut(hdc, 15 , 160, _T("RX: 0"), 5);
  TextOut(hdc, 15 , 180, _T("RY: 0"), 5);
  TextOut(hdc, 15 , 200, _T("RZ: 0"), 5);
  TextOut(hdc, 15 , 220, _T(" P: 0"), 5);

  /*release our window handle */
  ReleaseDC(hWndMain,hdc);

  UpdateWindow ( hWndMain );

} /* end of CreateWindow */


/*----------------------------------------------------------------------
* Function: SbMotionEvent()
*
* Description:
*    This function receives motion information and prints out the info
*    on screen.
*
*
* Args:
*    SiSpwEvent *pEvent   Containts Data from a 3D mouse Event
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void SbMotionEvent(SiSpwEvent *pEvent)
{
  TCHAR buff0[30];                            /* text buffer for TX */
  TCHAR buff1[30];                            /* text buffer for TY */
  TCHAR buff2[30];                            /* text buffer for TZ */
  TCHAR buff3[30];                            /* text buffer for RX */
  TCHAR buff4[30];                            /* text buffer for RY */
  TCHAR buff5[30];                            /* text buffer for RZ */
  TCHAR buff6[30];                            /* text buffer for Period */

  int len0,len1,len2,len3,len4,len5,len6;	   /* length of each buffer */

  /* put the actual ball data into the buffers */
  len0= _stprintf_s( buff0, 30, _T("TXaaa: %d         "), pEvent->u.spwData.mData[SI_TX] );
  len1= _stprintf_s( buff1, 30, _T("TY: %d         "), pEvent->u.spwData.mData[SI_TY] );
  len2= _stprintf_s( buff2, 30, _T("TZ: %d         "), pEvent->u.spwData.mData[SI_TZ] );
  len3= _stprintf_s( buff3, 30, _T("RX: %d         "), pEvent->u.spwData.mData[SI_RX] );
  len4= _stprintf_s( buff4, 30, _T("RY: %d         "), pEvent->u.spwData.mData[SI_RY] );
  len5= _stprintf_s( buff5, 30, _T("RZ: %d         "), pEvent->u.spwData.mData[SI_RZ] );
  len6= _stprintf_s( buff6, 30, _T(" P: %d         "), pEvent->u.spwData.period);

  /* get handle of our window to draw on */
  hdc = GetDC(hWndMain);

  /* print buffers */
  TCHAR *buf = _T("Motion Event              ");
  TextOut(hdc,  0 ,   0, buf, (int)_tcslen(buf));
  TextOut(hdc,  0 ,  20, devicename, (int)_tcslen(devicename));
  TextOut(hdc, 15 , 100, buff0, len0);
  TextOut(hdc, 15 , 120, buff1, len1);
  TextOut(hdc, 15 , 140, buff2, len2);
  TextOut(hdc, 15 , 160, buff3, len3);
  TextOut(hdc, 15 , 180, buff4, len4);
  TextOut(hdc, 15 , 200, buff5, len5);
  TextOut(hdc, 15 , 220, buff6, len6);

  /* Dump to debugger output buffer to get a running log */
  _RPT3(_CRT_WARN,"%S %S %S",  buff0, buff1, buff2);
  _RPT3(_CRT_WARN," %S %S %S", buff3, buff4, buff5);
  _RPT1(_CRT_WARN," %S\n", buff6);

  // Also dump to stdout for a searchable log
  printf("%d %d %d %d %d %d\n",
    pEvent->u.spwData.mData[SI_TX],
    pEvent->u.spwData.mData[SI_TY],
    pEvent->u.spwData.mData[SI_TZ],
    pEvent->u.spwData.mData[SI_RX],
    pEvent->u.spwData.mData[SI_RY],
    pEvent->u.spwData.mData[SI_RZ]);

  /*release our window handle */
  ReleaseDC(hWndMain,hdc);
}


/*----------------------------------------------------------------------
* Function: SbZeroEvent()
*
* Description:
*    This function clears the previous data, no motion data was received
*
*
*
* Args:
*    NONE
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void SbZeroEvent()
{
  /* get handle of our window to draw on */
  hdc = GetDC(hWndMain);

  /* print null data */
  TextOut(hdc,  0 ,   0, _T("Zero Event                  "), 28);
  TextOut(hdc,  0 ,  20, devicename, (int)_tcslen(devicename));
  TextOut(hdc, 15 , 100, _T("TXaaa: 0          "), 15);
  TextOut(hdc, 15 , 120, _T("TY: 0          "), 15);
  TextOut(hdc, 15 , 140, _T("TZ: 0          "), 15);
  TextOut(hdc, 15 , 160, _T("RX: 0          "), 15);
  TextOut(hdc, 15 , 180, _T("RY: 0          "), 15);
  TextOut(hdc, 15 , 200, _T("RZ: 0          "), 15);
  TextOut(hdc, 15 , 220, _T(" P: 0          "), 15);

  /*release our window handle */
  ReleaseDC(hWndMain,hdc);
}

TCHAR *V3DKeyToName(TCHAR *buf, int buflen, V3DKey v3dk)
{
  switch (v3dk)
  {
  case s3dm::V3DK_MENU		: _tcscpy_s(buf, buflen, _T("V3DK_MENU")); break;
  case s3dm::V3DK_FIT		: _tcscpy_s(buf, buflen, _T("V3DK_FIT")); break;
  case s3dm::V3DK_TOP		: _tcscpy_s(buf, buflen, _T("V3DK_TOP")); break;
  case s3dm::V3DK_LEFT		: _tcscpy_s(buf, buflen, _T("V3DK_LEFT")); break;
  case s3dm::V3DK_RIGHT		: _tcscpy_s(buf, buflen, _T("V3DK_RIGHT")); break;
  case s3dm::V3DK_FRONT		: _tcscpy_s(buf, buflen, _T("V3DK_FRONT")); break;
  case s3dm::V3DK_BOTTOM	: _tcscpy_s(buf, buflen, _T("V3DK_BOTTOM")); break;
  case s3dm::V3DK_BACK		: _tcscpy_s(buf, buflen, _T("V3DK_BACK")); break;
  case s3dm::V3DK_ROLL_CW	: _tcscpy_s(buf, buflen, _T("V3DK_ROLL_CW")); break;
  case s3dm::V3DK_ROLL_CCW	: _tcscpy_s(buf, buflen, _T("V3DK_ROLL_CCW")); break;
  case s3dm::V3DK_ISO1		: _tcscpy_s(buf, buflen, _T("V3DK_ISO1")); break;
  case s3dm::V3DK_ISO2		: _tcscpy_s(buf, buflen, _T("V3DK_ISO2")); break;
  case s3dm::V3DK_1			: _tcscpy_s(buf, buflen, _T("V3DK_1")); break;
  case s3dm::V3DK_2			: _tcscpy_s(buf, buflen, _T("V3DK_2")); break;
  case s3dm::V3DK_3			: _tcscpy_s(buf, buflen, _T("V3DK_3")); break;
  case s3dm::V3DK_4			: _tcscpy_s(buf, buflen, _T("V3DK_4")); break;
  case s3dm::V3DK_5			: _tcscpy_s(buf, buflen, _T("V3DK_5")); break;
  case s3dm::V3DK_6			: _tcscpy_s(buf, buflen, _T("V3DK_6")); break;
  case s3dm::V3DK_7			: _tcscpy_s(buf, buflen, _T("V3DK_7")); break;
  case s3dm::V3DK_8			: _tcscpy_s(buf, buflen, _T("V3DK_8")); break;
  case s3dm::V3DK_9			: _tcscpy_s(buf, buflen, _T("V3DK_9")); break;
  case s3dm::V3DK_10		: _tcscpy_s(buf, buflen, _T("V3DK_10")); break;
  case s3dm::V3DK_ESC		: _tcscpy_s(buf, buflen, _T("V3DK_ESC")); break;
  case s3dm::V3DK_ALT		: _tcscpy_s(buf, buflen, _T("V3DK_ALT")); break;
  case s3dm::V3DK_SHIFT		: _tcscpy_s(buf, buflen, _T("V3DK_SHIFT")); break;
  case s3dm::V3DK_CTRL		: _tcscpy_s(buf, buflen, _T("V3DK_CTRL")); break;
  case s3dm::V3DK_ROTATE	: _tcscpy_s(buf, buflen, _T("V3DK_ROTATE")); break;
  case s3dm::V3DK_PANZOOM	: _tcscpy_s(buf, buflen, _T("V3DK_PANZOOM")); break;
  case s3dm::V3DK_DOMINANT	: _tcscpy_s(buf, buflen, _T("V3DK_DOMINANT")); break;
  case s3dm::V3DK_PLUS		: _tcscpy_s(buf, buflen, _T("V3DK_PLUS")); break;
  case s3dm::V3DK_MINUS		: _tcscpy_s(buf, buflen, _T("V3DK_MINUS")); break;
  case s3dm::V3DK_SPIN_CW	: _tcscpy_s(buf, buflen, _T("V3DK_SPIN_CW")); break;
  case s3dm::V3DK_SPIN_CCW	: _tcscpy_s(buf, buflen, _T("V3DK_SPIN_CCW")); break;
  case s3dm::V3DK_TILT_CW	: _tcscpy_s(buf, buflen, _T("V3DK_TILT_CW")); break;
  case s3dm::V3DK_TILT_CCW	: _tcscpy_s(buf, buflen, _T("V3DK_TILT_CCW")); break;
  case V3DK_ENTER			: _tcscpy_s(buf, buflen, _T("V3DK_ENTER")); break;
  case V3DK_DELETE			: _tcscpy_s(buf, buflen, _T("V3DK_DELETE")); break;
  case V3DK_RESERVED0		: _tcscpy_s(buf, buflen, _T("V3DK_RESERVED0")); break;
  case V3DK_RESERVED1		: _tcscpy_s(buf, buflen, _T("V3DK_RESERVED1")); break;
  case V3DK_RESERVED2		: _tcscpy_s(buf, buflen, _T("V3DK_RESERVED2")); break;
  case V3DK_F1				: _tcscpy_s(buf, buflen, _T("V3DK_F1")); break;
  case V3DK_F2				: _tcscpy_s(buf, buflen, _T("V3DK_F2")); break;
  case V3DK_F3				: _tcscpy_s(buf, buflen, _T("V3DK_F3")); break;
  case V3DK_F4				: _tcscpy_s(buf, buflen, _T("V3DK_F4")); break;
  case V3DK_F5				: _tcscpy_s(buf, buflen, _T("V3DK_F5")); break;
  case V3DK_F6				: _tcscpy_s(buf, buflen, _T("V3DK_F6")); break;
  case V3DK_F7				: _tcscpy_s(buf, buflen, _T("V3DK_F7")); break;
  case V3DK_F8				: _tcscpy_s(buf, buflen, _T("V3DK_F8")); break;
  case V3DK_F9				: _tcscpy_s(buf, buflen, _T("V3DK_F9")); break;
  case V3DK_F10				: _tcscpy_s(buf, buflen, _T("V3DK_F10")); break;
  case V3DK_F11				: _tcscpy_s(buf, buflen, _T("V3DK_F11")); break;
  case V3DK_F12				: _tcscpy_s(buf, buflen, _T("V3DK_F12")); break;
  case V3DK_F13				: _tcscpy_s(buf, buflen, _T("V3DK_F13")); break;
  case V3DK_F14				: _tcscpy_s(buf, buflen, _T("V3DK_F14")); break;
  case V3DK_F15				: _tcscpy_s(buf, buflen, _T("V3DK_F15")); break;
  case V3DK_F16				: _tcscpy_s(buf, buflen, _T("V3DK_F16")); break;
  case V3DK_F17				: _tcscpy_s(buf, buflen, _T("V3DK_F17")); break;
  case V3DK_F18				: _tcscpy_s(buf, buflen, _T("V3DK_F18")); break;
  case V3DK_F19				: _tcscpy_s(buf, buflen, _T("V3DK_F19")); break;
  case V3DK_F20				: _tcscpy_s(buf, buflen, _T("V3DK_F20")); break;
  case V3DK_F21				: _tcscpy_s(buf, buflen, _T("V3DK_F21")); break;
  case V3DK_F22				: _tcscpy_s(buf, buflen, _T("V3DK_F22")); break;
  case V3DK_F23				: _tcscpy_s(buf, buflen, _T("V3DK_F23")); break;
  case V3DK_F24				: _tcscpy_s(buf, buflen, _T("V3DK_F24")); break;
  case V3DK_F25				: _tcscpy_s(buf, buflen, _T("V3DK_F25")); break;
  case V3DK_F26				: _tcscpy_s(buf, buflen, _T("V3DK_F26")); break;
  case V3DK_F27				: _tcscpy_s(buf, buflen, _T("V3DK_F27")); break;
  case V3DK_F28				: _tcscpy_s(buf, buflen, _T("V3DK_F28")); break;
  case V3DK_F29				: _tcscpy_s(buf, buflen, _T("V3DK_F29")); break;
  case V3DK_F30				: _tcscpy_s(buf, buflen, _T("V3DK_F30")); break;
  case V3DK_F31				: _tcscpy_s(buf, buflen, _T("V3DK_F31")); break;
  case V3DK_F32				: _tcscpy_s(buf, buflen, _T("V3DK_F32")); break;
  case V3DK_F33				: _tcscpy_s(buf, buflen, _T("V3DK_F33")); break;
  case V3DK_F34				: _tcscpy_s(buf, buflen, _T("V3DK_F34")); break;
  case V3DK_F35				: _tcscpy_s(buf, buflen, _T("V3DK_F35")); break;
  case V3DK_F36				: _tcscpy_s(buf, buflen, _T("V3DK_F36")); break;
  case V3DK_11				: _tcscpy_s(buf, buflen, _T("V3DK_11")); break;
  case V3DK_12				: _tcscpy_s(buf, buflen, _T("V3DK_12")); break;
  case V3DK_13				: _tcscpy_s(buf, buflen, _T("V3DK_13")); break;
  case V3DK_14				: _tcscpy_s(buf, buflen, _T("V3DK_14")); break;
  case V3DK_15				: _tcscpy_s(buf, buflen, _T("V3DK_15")); break;
  case V3DK_16				: _tcscpy_s(buf, buflen, _T("V3DK_16")); break;
  case V3DK_17				: _tcscpy_s(buf, buflen, _T("V3DK_17")); break;
  case V3DK_18				: _tcscpy_s(buf, buflen, _T("V3DK_18")); break;
  case V3DK_19				: _tcscpy_s(buf, buflen, _T("V3DK_19")); break;
  case V3DK_20				: _tcscpy_s(buf, buflen, _T("V3DK_20")); break;
  case V3DK_21				: _tcscpy_s(buf, buflen, _T("V3DK_21")); break;
  case V3DK_22				: _tcscpy_s(buf, buflen, _T("V3DK_22")); break;
  case V3DK_23				: _tcscpy_s(buf, buflen, _T("V3DK_23")); break;
  case V3DK_24				: _tcscpy_s(buf, buflen, _T("V3DK_24")); break;
  case V3DK_25				: _tcscpy_s(buf, buflen, _T("V3DK_25")); break;
  case V3DK_26				: _tcscpy_s(buf, buflen, _T("V3DK_26")); break;
  case V3DK_27				: _tcscpy_s(buf, buflen, _T("V3DK_27")); break;
  case V3DK_28				: _tcscpy_s(buf, buflen, _T("V3DK_28")); break;
  case V3DK_29				: _tcscpy_s(buf, buflen, _T("V3DK_29")); break;
  case V3DK_30				: _tcscpy_s(buf, buflen, _T("V3DK_30")); break;
  case V3DK_31				: _tcscpy_s(buf, buflen, _T("V3DK_31")); break;
  case V3DK_32				: _tcscpy_s(buf, buflen, _T("V3DK_32")); break;
  case V3DK_33				: _tcscpy_s(buf, buflen, _T("V3DK_33")); break;
  case V3DK_34				: _tcscpy_s(buf, buflen, _T("V3DK_34")); break;
  case V3DK_35				: _tcscpy_s(buf, buflen, _T("V3DK_35")); break;
  case V3DK_36				: _tcscpy_s(buf, buflen, _T("V3DK_36")); break;
  case V3DK_VIEW_1			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_1")); break;
  case V3DK_VIEW_2			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_2")); break;
  case V3DK_VIEW_3			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_3")); break;
  case V3DK_VIEW_4			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_4")); break;
  case V3DK_VIEW_5			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_5")); break;
  case V3DK_VIEW_6			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_6")); break;
  case V3DK_VIEW_7			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_7")); break;
  case V3DK_VIEW_8			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_8")); break;
  case V3DK_VIEW_9			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_9")); break;
  case V3DK_VIEW_10			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_10")); break;
  case V3DK_VIEW_11			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_11")); break;
  case V3DK_VIEW_12			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_12")); break;
  case V3DK_VIEW_13			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_13")); break;
  case V3DK_VIEW_14			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_14")); break;
  case V3DK_VIEW_15			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_15")); break;
  case V3DK_VIEW_16			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_16")); break;
  case V3DK_VIEW_17			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_17")); break;
  case V3DK_VIEW_18			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_18")); break;
  case V3DK_VIEW_19			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_19")); break;
  case V3DK_VIEW_20			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_20")); break;
  case V3DK_VIEW_21			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_21")); break;
  case V3DK_VIEW_22			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_22")); break;
  case V3DK_VIEW_23			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_23")); break;
  case V3DK_VIEW_24			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_24")); break;
  case V3DK_VIEW_25			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_25")); break;
  case V3DK_VIEW_26			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_26")); break;
  case V3DK_VIEW_27			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_27")); break;
  case V3DK_VIEW_28			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_28")); break;
  case V3DK_VIEW_29			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_29")); break;
  case V3DK_VIEW_30			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_30")); break;
  case V3DK_VIEW_31			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_31")); break;
  case V3DK_VIEW_32			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_32")); break;
  case V3DK_VIEW_33			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_33")); break;
  case V3DK_VIEW_34			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_34")); break;
  case V3DK_VIEW_35			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_35")); break;
  case V3DK_VIEW_36			: _tcscpy_s(buf, buflen, _T("V3DK_VIEW_36")); break;
  case V3DK_SAVE_VIEW_1		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_1")); break;
  case V3DK_SAVE_VIEW_2		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_2")); break;
  case V3DK_SAVE_VIEW_3		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_3")); break;
  case V3DK_SAVE_VIEW_4		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_4")); break;
  case V3DK_SAVE_VIEW_5		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_5")); break;
  case V3DK_SAVE_VIEW_6		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_6")); break;
  case V3DK_SAVE_VIEW_7		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_7")); break;
  case V3DK_SAVE_VIEW_8		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_8")); break;
  case V3DK_SAVE_VIEW_9		: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_9")); break;
  case V3DK_SAVE_VIEW_10	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_10")); break;
  case V3DK_SAVE_VIEW_11	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_11")); break;
  case V3DK_SAVE_VIEW_12	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_12")); break;
  case V3DK_SAVE_VIEW_13	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_13")); break;
  case V3DK_SAVE_VIEW_14	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_14")); break;
  case V3DK_SAVE_VIEW_15	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_15")); break;
  case V3DK_SAVE_VIEW_16	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_16")); break;
  case V3DK_SAVE_VIEW_17	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_17")); break;
  case V3DK_SAVE_VIEW_18	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_18")); break;
  case V3DK_SAVE_VIEW_19	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_19")); break;
  case V3DK_SAVE_VIEW_20	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_20")); break;
  case V3DK_SAVE_VIEW_21	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_21")); break;
  case V3DK_SAVE_VIEW_22	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_22")); break;
  case V3DK_SAVE_VIEW_23	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_23")); break;
  case V3DK_SAVE_VIEW_24	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_24")); break;
  case V3DK_SAVE_VIEW_25	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_25")); break;
  case V3DK_SAVE_VIEW_26	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_26")); break;
  case V3DK_SAVE_VIEW_27	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_27")); break;
  case V3DK_SAVE_VIEW_28	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_28")); break;
  case V3DK_SAVE_VIEW_29	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_29")); break;
  case V3DK_SAVE_VIEW_30	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_30")); break;
  case V3DK_SAVE_VIEW_31	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_31")); break;
  case V3DK_SAVE_VIEW_32	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_32")); break;
  case V3DK_SAVE_VIEW_33	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_33")); break;
  case V3DK_SAVE_VIEW_34	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_34")); break;
  case V3DK_SAVE_VIEW_35	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_35")); break;
  case V3DK_SAVE_VIEW_36	: _tcscpy_s(buf, buflen, _T("V3DK_SAVE_VIEW_36")); break;
  case V3DK_TAB				: _tcscpy_s(buf, buflen, _T("V3DK_TAB	")); break;
  case V3DK_SPACE			: _tcscpy_s(buf, buflen, _T("V3DK_SPACE")); break;
  case V3DK_MENU_1			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_1")); break;
  case V3DK_MENU_2			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_2")); break;
  case V3DK_MENU_3			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_3")); break;
  case V3DK_MENU_4			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_4")); break;
  case V3DK_MENU_5			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_5")); break;
  case V3DK_MENU_6			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_6")); break;
  case V3DK_MENU_7			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_7")); break;
  case V3DK_MENU_8			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_8")); break;
  case V3DK_MENU_9			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_9")); break;
  case V3DK_MENU_10			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_10")); break;
  case V3DK_MENU_11			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_11")); break;
  case V3DK_MENU_12			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_12")); break;
  case V3DK_MENU_13			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_13")); break;
  case V3DK_MENU_14			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_14")); break;
  case V3DK_MENU_15			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_15")); break;
  case V3DK_MENU_16			: _tcscpy_s(buf, buflen, _T("V3DK_MENU_16")); break;
  case V3DK_USER			: _tcscpy_s(buf, buflen, _T("V3DK_USER")); break;

  default:
    // Put unrecognized buttons in parens
    _stprintf_s(buf, buflen, _T("(unrecognized V3DKey %d)"), v3dk);
    break;
  }

  return buf;
}

/*----------------------------------------------------------------------
* Function: SbButtonPressEvent()
*
* Description:
*    This function receives 3D mouse button information and prints out the
*    info on screen.
*
*
* Args:
*    int     buttonnumber   //Containts number of button pressed
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void SbButtonPressEvent(int buttonnumber)
{
  TCHAR bn[100];

  /* get handle of our window to draw on */
  hdc = GetDC(hWndMain);

  SiButtonName name;
  SiGetButtonName(devHdl, buttonnumber, &name);

  V3DKeyToName(bn, SPW_NUM_ELEMENTS_IN(bn), (V3DKey)buttonnumber);

  // (provide a switch statement for easy cut & paste)
  switch (buttonnumber)
  {
  case s3dm::V3DK_MENU		: break;
  case s3dm::V3DK_FIT		: break;
  case s3dm::V3DK_TOP		: break;
  case s3dm::V3DK_LEFT		: break;
  case s3dm::V3DK_RIGHT		: break;
  case s3dm::V3DK_FRONT		: break;
  case s3dm::V3DK_BOTTOM	: break;
  case s3dm::V3DK_BACK		: break;
  case s3dm::V3DK_ROLL_CW	: break;
  case s3dm::V3DK_ROLL_CCW	: break;
  case s3dm::V3DK_ISO1		: break;
  case s3dm::V3DK_ISO2		: break;
  case s3dm::V3DK_1			: break;
  case s3dm::V3DK_2			: break;
  case s3dm::V3DK_3			: break;
  case s3dm::V3DK_4			: break;
  case s3dm::V3DK_5			: break;
  case s3dm::V3DK_6			: break;
  case s3dm::V3DK_7			: break;
  case s3dm::V3DK_8			: break;
  case s3dm::V3DK_9			: break;
  case s3dm::V3DK_10		: break;
  case s3dm::V3DK_ESC		: break;
  case s3dm::V3DK_ALT		: break;
  case s3dm::V3DK_SHIFT		: break;
  case s3dm::V3DK_CTRL		: break;
  case s3dm::V3DK_ROTATE	: break;
  case s3dm::V3DK_PANZOOM	: break;
  case s3dm::V3DK_DOMINANT	: break;
  case s3dm::V3DK_PLUS		: break;
  case s3dm::V3DK_MINUS		: break;
  case s3dm::V3DK_SPIN_CW	: break;
  case s3dm::V3DK_SPIN_CCW	: break;
  case s3dm::V3DK_TILT_CW	: break;
  case s3dm::V3DK_TILT_CCW	: break;
  case V3DK_ENTER			: break;
  case V3DK_DELETE			: break;
  case V3DK_RESERVED0		: break;
  case V3DK_RESERVED1		: break;
  case V3DK_RESERVED2		: break;
  case V3DK_F1				: break;
  case V3DK_F2				: break;
  case V3DK_F3				: break;
  case V3DK_F4				: break;
  case V3DK_F5				: break;
  case V3DK_F6				: break;
  case V3DK_F7				: break;
  case V3DK_F8				: break;
  case V3DK_F9				: break;
  case V3DK_F10				: break;
  case V3DK_F11				: break;
  case V3DK_F12				: break;
  case V3DK_F13				: break;
  case V3DK_F14				: break;
  case V3DK_F15				: break;
  case V3DK_F16				: break;
  case V3DK_F17				: break;
  case V3DK_F18				: break;
  case V3DK_F19				: break;
  case V3DK_F20				: break;
  case V3DK_F21				: break;
  case V3DK_F22				: break;
  case V3DK_F23				: break;
  case V3DK_F24				: break;
  case V3DK_F25				: break;
  case V3DK_F26				: break;
  case V3DK_F27				: break;
  case V3DK_F28				: break;
  case V3DK_F29				: break;
  case V3DK_F30				: break;
  case V3DK_F31				: break;
  case V3DK_F32				: break;
  case V3DK_F33				: break;
  case V3DK_F34				: break;
  case V3DK_F35				: break;
  case V3DK_F36				: break;
  case V3DK_11				: break;
  case V3DK_12				: break;
  case V3DK_13				: break;
  case V3DK_14				: break;
  case V3DK_15				: break;
  case V3DK_16				: break;
  case V3DK_17				: break;
  case V3DK_18				: break;
  case V3DK_19				: break;
  case V3DK_20				: break;
  case V3DK_21				: break;
  case V3DK_22				: break;
  case V3DK_23				: break;
  case V3DK_24				: break;
  case V3DK_25				: break;
  case V3DK_26				: break;
  case V3DK_27				: break;
  case V3DK_28				: break;
  case V3DK_29				: break;
  case V3DK_30				: break;
  case V3DK_31				: break;
  case V3DK_32				: break;
  case V3DK_33				: break;
  case V3DK_34				: break;
  case V3DK_35				: break;
  case V3DK_36				: break;
  case V3DK_VIEW_1			: break;
  case V3DK_VIEW_2			: break;
  case V3DK_VIEW_3			: break;
  case V3DK_VIEW_4			: break;
  case V3DK_VIEW_5			: break;
  case V3DK_VIEW_6			: break;
  case V3DK_VIEW_7			: break;
  case V3DK_VIEW_8			: break;
  case V3DK_VIEW_9			: break;
  case V3DK_VIEW_10			: break;
  case V3DK_VIEW_11			: break;
  case V3DK_VIEW_12			: break;
  case V3DK_VIEW_13			: break;
  case V3DK_VIEW_14			: break;
  case V3DK_VIEW_15			: break;
  case V3DK_VIEW_16			: break;
  case V3DK_VIEW_17			: break;
  case V3DK_VIEW_18			: break;
  case V3DK_VIEW_19			: break;
  case V3DK_VIEW_20			: break;
  case V3DK_VIEW_21			: break;
  case V3DK_VIEW_22			: break;
  case V3DK_VIEW_23			: break;
  case V3DK_VIEW_24			: break;
  case V3DK_VIEW_25			: break;
  case V3DK_VIEW_26			: break;
  case V3DK_VIEW_27			: break;
  case V3DK_VIEW_28			: break;
  case V3DK_VIEW_29			: break;
  case V3DK_VIEW_30			: break;
  case V3DK_VIEW_31			: break;
  case V3DK_VIEW_32			: break;
  case V3DK_VIEW_33			: break;
  case V3DK_VIEW_34			: break;
  case V3DK_VIEW_35			: break;
  case V3DK_VIEW_36			: break;
  case V3DK_SAVE_VIEW_1		: break;
  case V3DK_SAVE_VIEW_2		: break;
  case V3DK_SAVE_VIEW_3		: break;
  case V3DK_SAVE_VIEW_4		: break;
  case V3DK_SAVE_VIEW_5		: break;
  case V3DK_SAVE_VIEW_6		: break;
  case V3DK_SAVE_VIEW_7		: break;
  case V3DK_SAVE_VIEW_8		: break;
  case V3DK_SAVE_VIEW_9		: break;
  case V3DK_SAVE_VIEW_10	: break;
  case V3DK_SAVE_VIEW_11	: break;
  case V3DK_SAVE_VIEW_12	: break;
  case V3DK_SAVE_VIEW_13	: break;
  case V3DK_SAVE_VIEW_14	: break;
  case V3DK_SAVE_VIEW_15	: break;
  case V3DK_SAVE_VIEW_16	: break;
  case V3DK_SAVE_VIEW_17	: break;
  case V3DK_SAVE_VIEW_18	: break;
  case V3DK_SAVE_VIEW_19	: break;
  case V3DK_SAVE_VIEW_20	: break;
  case V3DK_SAVE_VIEW_21	: break;
  case V3DK_SAVE_VIEW_22	: break;
  case V3DK_SAVE_VIEW_23	: break;
  case V3DK_SAVE_VIEW_24	: break;
  case V3DK_SAVE_VIEW_25	: break;
  case V3DK_SAVE_VIEW_26	: break;
  case V3DK_SAVE_VIEW_27	: break;
  case V3DK_SAVE_VIEW_28	: break;
  case V3DK_SAVE_VIEW_29	: break;
  case V3DK_SAVE_VIEW_30	: break;
  case V3DK_SAVE_VIEW_31	: break;
  case V3DK_SAVE_VIEW_32	: break;
  case V3DK_SAVE_VIEW_33	: break;
  case V3DK_SAVE_VIEW_34	: break;
  case V3DK_SAVE_VIEW_35	: break;
  case V3DK_SAVE_VIEW_36	: break;
  case V3DK_TAB				: break;
  case V3DK_SPACE			: break;
  case V3DK_MENU_1			: break;
  case V3DK_MENU_2			: break;
  case V3DK_MENU_3			: break;
  case V3DK_MENU_4			: break;
  case V3DK_MENU_5			: break;
  case V3DK_MENU_6			: break;
  case V3DK_MENU_7			: break;
  case V3DK_MENU_8			: break;
  case V3DK_MENU_9			: break;
  case V3DK_MENU_10			: break;
  case V3DK_MENU_11			: break;
  case V3DK_MENU_12			: break;
  case V3DK_MENU_13			: break;
  case V3DK_MENU_14			: break;
  case V3DK_MENU_15			: break;
  case V3DK_MENU_16			: break;
  case V3DK_USER			: break;

  default					: break;
  }

  wchar_t fullStr[100];
  wcscpy_s(fullStr, SPW_NUM_ELEMENTS_IN(fullStr), _T("-------------------------------------------------------"));
  TextOut(hdc, 0 , 0, fullStr, (int)_tcslen(fullStr));

  _stprintf_s(fullStr, SPW_NUM_ELEMENTS_IN(fullStr), _T("%s Pressed"), bn);
  TextOut(hdc, 0 , 0, fullStr, (int)_tcslen(fullStr));

  // Output to stdout for tracing
  printf("%S\n", fullStr);
  OutputDebugString(fullStr);

  /*release our window handle */
  ReleaseDC(hWndMain,hdc);
}

/*----------------------------------------------------------------------
* Function: SbButtonReleaseEvent()
*
* Description:
*    This function receives 3D mouse button information and prints out the
*    info on screen.
*
*
* Args:
*    int     buttonnumber   //Containts number of button pressed
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void SbButtonReleaseEvent(int buttonnumber)
{
  TCHAR bn[100];

  /* get handle of our window to draw on */
  hdc = GetDC(hWndMain);

  V3DKeyToName(bn, SPW_NUM_ELEMENTS_IN(bn), (V3DKey)buttonnumber);

  // (provide a switch statement for easy cut & paste)
  switch (buttonnumber)
  {
  case s3dm::V3DK_MENU		: break;
  case s3dm::V3DK_FIT		: break;
  case s3dm::V3DK_TOP		: break;
  case s3dm::V3DK_LEFT		: break;
  case s3dm::V3DK_RIGHT		: break;
  case s3dm::V3DK_FRONT		: break;
  case s3dm::V3DK_BOTTOM	: break;
  case s3dm::V3DK_BACK		: break;
  case s3dm::V3DK_ROLL_CW	: break;
  case s3dm::V3DK_ROLL_CCW	: break;
  case s3dm::V3DK_ISO1		: break;
  case s3dm::V3DK_ISO2		: break;
  case s3dm::V3DK_1			: break;
  case s3dm::V3DK_2			: break;
  case s3dm::V3DK_3			: break;
  case s3dm::V3DK_4			: break;
  case s3dm::V3DK_5			: break;
  case s3dm::V3DK_6			: break;
  case s3dm::V3DK_7			: break;
  case s3dm::V3DK_8			: break;
  case s3dm::V3DK_9			: break;
  case s3dm::V3DK_10		: break;
  case s3dm::V3DK_ESC		: break;
  case s3dm::V3DK_ALT		: break;
  case s3dm::V3DK_SHIFT		: break;
  case s3dm::V3DK_CTRL		: break;
  case s3dm::V3DK_ROTATE	: break;
  case s3dm::V3DK_PANZOOM	: break;
  case s3dm::V3DK_DOMINANT	: break;
  case s3dm::V3DK_PLUS		: break;
  case s3dm::V3DK_MINUS		: break;
  case s3dm::V3DK_SPIN_CW	: break;
  case s3dm::V3DK_SPIN_CCW	: break;
  case s3dm::V3DK_TILT_CW	: break;
  case s3dm::V3DK_TILT_CCW	: break;
  case V3DK_ENTER			: break;
  case V3DK_DELETE			: break;
  case V3DK_RESERVED0		: break;
  case V3DK_RESERVED1		: break;
  case V3DK_RESERVED2		: break;
  case V3DK_F1				: break;
  case V3DK_F2				: break;
  case V3DK_F3				: break;
  case V3DK_F4				: break;
  case V3DK_F5				: break;
  case V3DK_F6				: break;
  case V3DK_F7				: break;
  case V3DK_F8				: break;
  case V3DK_F9				: break;
  case V3DK_F10				: break;
  case V3DK_F11				: break;
  case V3DK_F12				: break;
  case V3DK_F13				: break;
  case V3DK_F14				: break;
  case V3DK_F15				: break;
  case V3DK_F16				: break;
  case V3DK_F17				: break;
  case V3DK_F18				: break;
  case V3DK_F19				: break;
  case V3DK_F20				: break;
  case V3DK_F21				: break;
  case V3DK_F22				: break;
  case V3DK_F23				: break;
  case V3DK_F24				: break;
  case V3DK_F25				: break;
  case V3DK_F26				: break;
  case V3DK_F27				: break;
  case V3DK_F28				: break;
  case V3DK_F29				: break;
  case V3DK_F30				: break;
  case V3DK_F31				: break;
  case V3DK_F32				: break;
  case V3DK_F33				: break;
  case V3DK_F34				: break;
  case V3DK_F35				: break;
  case V3DK_F36				: break;
  case V3DK_11				: break;
  case V3DK_12				: break;
  case V3DK_13				: break;
  case V3DK_14				: break;
  case V3DK_15				: break;
  case V3DK_16				: break;
  case V3DK_17				: break;
  case V3DK_18				: break;
  case V3DK_19				: break;
  case V3DK_20				: break;
  case V3DK_21				: break;
  case V3DK_22				: break;
  case V3DK_23				: break;
  case V3DK_24				: break;
  case V3DK_25				: break;
  case V3DK_26				: break;
  case V3DK_27				: break;
  case V3DK_28				: break;
  case V3DK_29				: break;
  case V3DK_30				: break;
  case V3DK_31				: break;
  case V3DK_32				: break;
  case V3DK_33				: break;
  case V3DK_34				: break;
  case V3DK_35				: break;
  case V3DK_36				: break;
  case V3DK_VIEW_1			: break;
  case V3DK_VIEW_2			: break;
  case V3DK_VIEW_3			: break;
  case V3DK_VIEW_4			: break;
  case V3DK_VIEW_5			: break;
  case V3DK_VIEW_6			: break;
  case V3DK_VIEW_7			: break;
  case V3DK_VIEW_8			: break;
  case V3DK_VIEW_9			: break;
  case V3DK_VIEW_10			: break;
  case V3DK_VIEW_11			: break;
  case V3DK_VIEW_12			: break;
  case V3DK_VIEW_13			: break;
  case V3DK_VIEW_14			: break;
  case V3DK_VIEW_15			: break;
  case V3DK_VIEW_16			: break;
  case V3DK_VIEW_17			: break;
  case V3DK_VIEW_18			: break;
  case V3DK_VIEW_19			: break;
  case V3DK_VIEW_20			: break;
  case V3DK_VIEW_21			: break;
  case V3DK_VIEW_22			: break;
  case V3DK_VIEW_23			: break;
  case V3DK_VIEW_24			: break;
  case V3DK_VIEW_25			: break;
  case V3DK_VIEW_26			: break;
  case V3DK_VIEW_27			: break;
  case V3DK_VIEW_28			: break;
  case V3DK_VIEW_29			: break;
  case V3DK_VIEW_30			: break;
  case V3DK_VIEW_31			: break;
  case V3DK_VIEW_32			: break;
  case V3DK_VIEW_33			: break;
  case V3DK_VIEW_34			: break;
  case V3DK_VIEW_35			: break;
  case V3DK_VIEW_36			: break;
  case V3DK_SAVE_VIEW_1		: break;
  case V3DK_SAVE_VIEW_2		: break;
  case V3DK_SAVE_VIEW_3		: break;
  case V3DK_SAVE_VIEW_4		: break;
  case V3DK_SAVE_VIEW_5		: break;
  case V3DK_SAVE_VIEW_6		: break;
  case V3DK_SAVE_VIEW_7		: break;
  case V3DK_SAVE_VIEW_8		: break;
  case V3DK_SAVE_VIEW_9		: break;
  case V3DK_SAVE_VIEW_10	: break;
  case V3DK_SAVE_VIEW_11	: break;
  case V3DK_SAVE_VIEW_12	: break;
  case V3DK_SAVE_VIEW_13	: break;
  case V3DK_SAVE_VIEW_14	: break;
  case V3DK_SAVE_VIEW_15	: break;
  case V3DK_SAVE_VIEW_16	: break;
  case V3DK_SAVE_VIEW_17	: break;
  case V3DK_SAVE_VIEW_18	: break;
  case V3DK_SAVE_VIEW_19	: break;
  case V3DK_SAVE_VIEW_20	: break;
  case V3DK_SAVE_VIEW_21	: break;
  case V3DK_SAVE_VIEW_22	: break;
  case V3DK_SAVE_VIEW_23	: break;
  case V3DK_SAVE_VIEW_24	: break;
  case V3DK_SAVE_VIEW_25	: break;
  case V3DK_SAVE_VIEW_26	: break;
  case V3DK_SAVE_VIEW_27	: break;
  case V3DK_SAVE_VIEW_28	: break;
  case V3DK_SAVE_VIEW_29	: break;
  case V3DK_SAVE_VIEW_30	: break;
  case V3DK_SAVE_VIEW_31	: break;
  case V3DK_SAVE_VIEW_32	: break;
  case V3DK_SAVE_VIEW_33	: break;
  case V3DK_SAVE_VIEW_34	: break;
  case V3DK_SAVE_VIEW_35	: break;
  case V3DK_SAVE_VIEW_36	: break;
  case V3DK_TAB				: break;
  case V3DK_SPACE			: break;
  case V3DK_MENU_1			: break;
  case V3DK_MENU_2			: break;
  case V3DK_MENU_3			: break;
  case V3DK_MENU_4			: break;
  case V3DK_MENU_5			: break;
  case V3DK_MENU_6			: break;
  case V3DK_MENU_7			: break;
  case V3DK_MENU_8			: break;
  case V3DK_MENU_9			: break;
  case V3DK_MENU_10			: break;
  case V3DK_MENU_11			: break;
  case V3DK_MENU_12			: break;
  case V3DK_MENU_13			: break;
  case V3DK_MENU_14			: break;
  case V3DK_MENU_15			: break;
  case V3DK_MENU_16			: break;
  case V3DK_USER			: break;

  default					: break;
  }

  wchar_t fullStr[100];
  wcscpy_s(fullStr, SPW_NUM_ELEMENTS_IN(fullStr), _T("--------------------------------------------------------"));
  TextOut(hdc, 0 , 0, fullStr, (int)_tcslen(fullStr));

  _stprintf_s(fullStr, SPW_NUM_ELEMENTS_IN(fullStr), _T("%s Released"),bn);
  TextOut(hdc, 0 , 0, fullStr, (int)_tcslen(fullStr));

  // Output to stdout for tracing
  printf("%S\n",fullStr);
  OutputDebugString(fullStr);

  /*release our window handle */
  ReleaseDC(hWndMain,hdc);
}

void HandleDeviceChangeEvent(SiSpwEvent *pEvent)
{
  TCHAR buf[100];
  hdc = GetDC(hWndMain);

  switch(pEvent->u.deviceChangeEventData.type)
  {
  case SI_DEVICE_CHANGE_CONNECT:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("Device ID %d connected"),pEvent->u.deviceChangeEventData.devID);
    TextOut(hdc, 0, 20, buf, (int)_tcslen(buf));
    break;
  case SI_DEVICE_CHANGE_DISCONNECT:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("Device ID %d disconnected"),pEvent->u.deviceChangeEventData.devID);
    TextOut(hdc, 0, 20, buf, (int)_tcslen(buf));
    break;
  default:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("Unknown deviceChangeEvent type: %d"),pEvent->u.deviceChangeEventData.type);
    TextOut(hdc, 0, 20, buf, (int)_tcslen(buf));
    break;
  }
  ReleaseDC(hWndMain,hdc);
}

void HandleV3DCMDEvent(SiSpwEvent *pEvent)
{
  TCHAR buf[100];
  hdc = GetDC(hWndMain);
  switch(pEvent->u.cmdEventData.functionNumber)
  {
  case V3DCMD_MENU_OPTIONS:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MENU_OPTIONS(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_FIT:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_FIT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F1:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F2:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F3:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F4:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F4(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F5:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F5(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F6:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F6(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F7:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F7(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F8:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F8(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F9:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F9(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F10:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F10(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F11:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F11(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_KEY_F12:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F12(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_TOP:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_TOP(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_LEFT:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_LEFT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_RIGHT:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_RIGHT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_FRONT:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_FRONT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_BOTTOM:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_BOTTOM(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_BACK:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_BACK(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_ROLLCW:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ROLLCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_ROLLCCW:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ROLLCCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_ISO1:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ISO1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_ISO2:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ISO2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_1:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_2:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_3:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_SAVE_VIEW_1:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_SAVE_VIEW_1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_SAVE_VIEW_2:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_SAVE_VIEW_2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_SAVE_VIEW_3:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_SAVE_VIEW_3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_QZ_IN:
    _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_QZ_IN(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_VIEW_QZ_OUT:
	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_QZ_OUT(%s)"),pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_MOTIONMACRO_ZOOM:
	  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOM(ZoomFrom=%d, ZoomTo=%d, Scale=%f)"),pEvent->u.cmdEventData.iArgs[0],pEvent->u.cmdEventData.iArgs[1],pEvent->u.cmdEventData.fArgs[0]);
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_MOTIONMACRO_ZOOMIN_CENTERTOCENTER:
	  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMIN_CENTERTOCENTER(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCENTER:
	  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCENTER(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCURSOR:
	  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCURSOR(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_MOTIONMACRO_ZOOMOUT_CENTERTOCENTER:
	  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMOUT_CENTERTOCENTER(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCENTER:
	  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCENTER(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  case V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCURSOR:
	  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCURSOR(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  default:
	  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("Unhandled V3DCMD: number = %d"),pEvent->u.cmdEventData.functionNumber);
    TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
    break;
  }
  ReleaseDC(hWndMain,hdc);
}

void HandleAppEvent(SiSpwEvent *pEvent)
{
  TCHAR buf[100];
  hdc = GetDC(hWndMain);
  _stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("AppCmd: %S %s"), pEvent->u.appCommandData.id.appCmdID,pEvent->u.appCommandData.pressed ? _T(" pressed") : _T(" released"));
  TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
}
