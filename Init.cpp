
#include "main.h"

//  ���ļ��д�������
void CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID)
{
	AUX_RGBImageRec *pBitmap = NULL;
	
	if(!strFileName)									// ����޴��ļ�����ֱ�ӷ���
		return;

	pBitmap = auxDIBImageLoad(strFileName);				// װ��λͼ������������
	
	if(pBitmap == NULL)									// ���װ��λͼʧ�ܣ����˳�
		exit(0);

	// ��������
	glGenTextures(1, &textureArray[textureID]);

	// �������ض����ʽ
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitmap->sizeX, pBitmap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	if (pBitmap)										// �ͷ�λͼռ�õ���Դ
	{
		if (pBitmap->data)						
		{
			free(pBitmap->data);				
		}

		free(pBitmap);					
	}
}

void ChangeToFullScreen()
{
	DEVMODE dmSettings;								

	memset(&dmSettings,0,sizeof(dmSettings));		

	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= SCREEN_WIDTH;		
	dmSettings.dmPelsHeight	= SCREEN_HEIGHT;	
	
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	if(result != DISP_CHANGE_SUCCESSFUL)
	{

		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}

HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance) 
                                        
{
	HWND hWnd;
	WNDCLASS wndclass;
	
	memset(&wndclass, 0, sizeof(WNDCLASS));			
	wndclass.style = CS_HREDRAW | CS_VREDRAW;		
	wndclass.lpfnWndProc = WinProc;				
	wndclass.hInstance = hInstance;				
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wndclass.lpszClassName = "GameTutorials";		

	RegisterClass(&wndclass);						
	
	if(bFullScreen && !dwStyle) 					
	{												
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen();					
		ShowCursor(FALSE);						
	}
	else if(!dwStyle)							
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	g_hInstance = hInstance;						

	RECT rWindow;
	rWindow.left	= 0;				
	rWindow.right	= width;			
	rWindow.top	    = 0;					
	rWindow.bottom	= height;					
	AdjustWindowRect( &rWindow, dwStyle, false);	

												
	hWnd = CreateWindow("GameTutorials", strWindowName, dwStyle, 0, 0,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hWnd) return NULL;					

	ShowWindow(hWnd, SW_SHOWNORMAL);	
	UpdateWindow(hWnd);					

	SetFocus(hWnd);					

	return hWnd;
}

bool bSetupPixelFormat(HDC hdc)                //������Ļ��ʽ
{ 
    PIXELFORMATDESCRIPTOR pfd; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			
    pfd.nVersion = 1;								
													
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;			
    pfd.iPixelType = PFD_TYPE_RGBA;				
    pfd.cColorBits = SCREEN_DEPTH;				
    pfd.cDepthBits = SCREEN_DEPTH;				
    pfd.cAccumBits = 0;						
    pfd.cStencilBits = 0;				
 
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE;							
}

void SizeOpenGLScreen(int width, int height)	//��ʼ��ģ���ӵ����
{
	if (height==0)										
	{
		height=1;										
	}

	glViewport(0,0,width,height);						

	glMatrixMode(GL_PROJECTION);		
	glLoadIdentity();						
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, .5f ,150.0f);

	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();									
}


void InitializeOpenGL(int width, int height)     //������Ļ��С
{  
    g_hDC = GetDC(g_hWnd);							
											
    if (!bSetupPixelFormat(g_hDC))			
        PostQuitMessage (0);					

    g_hRC = wglCreateContext(g_hDC);			
    wglMakeCurrent(g_hDC, g_hRC);			

	glEnable(GL_TEXTURE_2D);				
	glEnable(GL_DEPTH_TEST);				

	SizeOpenGLScreen(width, height);	
}

void DeInit()                                    //ж��OpenGL
{
	if (g_hRC)											
	{
		wglMakeCurrent(NULL, NULL);						
		wglDeleteContext(g_hRC);						
	}
	
	if (g_hDC) 
		ReleaseDC(g_hWnd, g_hDC);						
		
	if(g_bFullScreen)									
	{
		ChangeDisplaySettings(NULL,0);					
		ShowCursor(TRUE);							
	}

	UnregisterClass("GameTutorials", g_hInstance);	

	PostQuitMessage (0);							
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;

	// �ж��û��Ƿ���Ҫȫ����ʾ
	if(MessageBox(NULL, "Click Yes to go to full screen (Recommended)", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
		g_bFullScreen = false;
	
	hWnd = CreateMyWindow(" 3DS Loader", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	if(hWnd == NULL) return true;

	// ��ʼ��OpenGL
	Init(hWnd);													
	return MainLoop();						
}



