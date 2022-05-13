#include <windows.h>
#include <math.h>
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MainProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GameProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL RectContains(LPRECT,int x,int y);
void DrawBitmap(HDC hdc,int x,int y,HBITMAP hBit);
void DrawID(int x,int y,char *ID,int IDlen);
/*전역 변수 선언*/
enum MODE{MAIN,INDIVIDUAL,TEAM};
enum MODE mode;
enum TEAM {NONE,RED,BLUE,GREEN};
HINSTANCE hinstance;
HWND mainWnd;
HDC hdc;
RECT clientRect;
int sizeX,sizeY;
HBRUSH whiteBrush;
HBRUSH blueBrush;
RECT mainIndividualBtn={10,10,500,50};
RECT mainTeamBtn={10,60,500,110};
PAINTSTRUCT ps;
HBITMAP doubleBufferBitmap=NULL;
HDC doubleBufferDC; 
HBITMAP OldBit;

CellGroup master;//지금 이것을 플레이 하고 있는 사람의 세포 

char test[64];
/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "AgarIO 세포키우기",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           700,                 /* The programs width */
           700,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    mainWnd=hwnd;
    hinstance=hThisInstance;

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            DeleteObject(doubleBufferBitmap);
            DeleteObject(whiteBrush);
            DeleteObject(blueBrush);
            KillTimer(hwnd,1);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case WM_CREATE:
             whiteBrush=CreateSolidBrush(RGB(0xFF,0xFF,0xFF));
             blueBrush=CreateSolidBrush(RGB(0,0,0xFF));
             mode=MAIN;
             break;
        case WM_SIZE:
        case WM_TIMER:
        case WM_PAINT:
        case WM_KEYDOWN:
        case WM_LBUTTONDOWN:
                if(mode==MAIN)
                {
                     return MainProc (hwnd, message, wParam, lParam);
                }
                else
                {
                     return GameProc(hwnd,message,wParam,lParam);
                }
        default:
                return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
LRESULT CALLBACK MainProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_LBUTTONDOWN:
             {
                            int x=LOWORD(lParam);//x위치 
                            int y=HIWORD(lParam);//y위치 
                            if(RectContains(&mainIndividualBtn,x,y))mode=INDIVIDUAL;
                            else if(RectContains(&mainTeamBtn,x,y))mode=TEAM;
                            if(mode!=MAIN){
                                           SetTimer(hwnd,1,20,NULL);
                                           doubleBufferBitmap=NULL;
                                           FillRect(hdc=GetDC(hwnd),&clientRect,whiteBrush);
                                           ReleaseDC(hwnd,hdc);
                            }
             }
             break;
        case WM_PAINT:
             GetClientRect(hwnd,&clientRect);
             hdc=BeginPaint(hwnd,&ps);
             FillRect(hdc,&clientRect,whiteBrush);
             FillRect(hdc,&mainIndividualBtn,blueBrush);
             FillRect(hdc,&mainTeamBtn,blueBrush);
             TextOut(hdc,30,11,TEXT("개인전"),6);
             TextOut(hdc,30,62,TEXT("팀전"),4);
             EndPaint(hwnd,&ps); 
             break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

class Cell
{
      public:
      BOOL alive;
      int x,y;//비트맵 용 
      int score;
      int centerX,centerY;//충돌 검사용 
      int radius;
      Cell(int cX,int cY,int iscore)
      {
               score=iscore;
               centerX=cX;
               centerY=cY;
      }
      void Render(HDC hdc,COLORREF colorref)
      {
                 //x,y에 둥근 원을 그린다. 반지름은 root score
                 if(score<0||!alive)return;
                 radius=sqrt(score);
                 HBRUSH hbrush=CreateSolidBrush(colorref);
                 HBRUSH oldbrush=(HBRUSH)SelectObject(hdc,hbrush);
                 Ellipse(hdc,x,y,x+2*radius,y+2*radius);
                 SelectObject(hdc,oldbrush);
                 DeleteObject(hbrush); 
      }
      void Render(HDC hdc,HBITMAP texture)
      {
                 //x,y에 마스크 준비된 텍스쳐를 투명하게 그린다. 
      }
      
};
class CellGroup
{
      public:
      BOOL alive; 
      int scoreSum;
      COLORREF colorref;
      HBITMAP texture;
      Cell * components[8];
      char ID[32];//NULL까지 32글자 
      int IDlen;//ID 글자 수 
      int x,y;//비트맵 그릴 때 쓸 수. 
      int centerX,centerY;//내 생각에는 움직일 때마다 업데이트 해야 함. 
      enum TEAM team;
      CellGroup(){             //디폴트 초기화 함수 
                  alive=FALSE;
                  scoreSum=0;
                  colorref=0;
                  texture=NULL;
                  for(int i=0;i<8;i++)components[i]=NULL;
                  memset(ID,0,32);
                  IDlen=0;
                  x=0;
                  y=0;
                  centerX=0;
                  centerY=0;
                  team=NONE;
      }
      CellGroup(int spawnx,int spawny,COLORREF colorref) //중립세포용 
      {
                  CellGroup();
                  centerX=spawnx;
                  centerY=spawny;
                  scoreSum=10;                           //하나 먹을 때마다 10씩 는다.
                  components[0]=new Cell(centerX,centerY,10); //디폴트 하나의 세포
                  alive=TRUE;       //생성과 동시에 바로 보임 
      }
      CellGroup(int spawnX,int spawnY,COLORREF colorref,char *pID)//ID가 있는 보통의 세포. COLORREF가 있는 애 TEAM이 없는 보통 세포 
      {
                    CellGroup(spawnX,spawnY,colorref);
                    strncpy(ID,pID,32);
                    scoreSum=100;
                    components[0]->score=100;
      }
      CellGroup(int spawnX,int spawnY,HBITMAP text,char *pID)//ID가 있는 보통의 세포, Texture가 있음, Team 없음 
      {
                    CellGroup(spawnX,spawnY,colorref,pID);
                    colorref=0;
                    texture=text;
      }
      CellGroup(int spawnX,int spawnY,COLORREF colorref,char *pID,enum TEAM t)//팀이 있는 컬러 있는 일반 세포
      {
                    CellGroup(spawnX,spawnY,colorref,pID);
                    team=t;
      }
      CellGroup(int spawnX,int spawnY,HBITMAP text,char *pID,enum TEAM t)//팀이 있는 텍스쳐 있는 일반 세포
      {
                    CellGroup(spawnX,spawnY,text,pID);
                    team=t;
      }               
      ~CellGroup()
      {
                  for(int i=0;i<8;i++)
                  {
                                  if(components[i]!=NULL)delete components[i];// 분열 세포 삭제와 소멸자 호출
                  }
      } 
      void Render(HDC hdc)
      {
           if(!alive)return;//죽었으므로 그릴 필요 없음 
           for(int i=0;i<8;i++)//루프를 돌며 자식들을 그린다. 
           {
                   if(components[i]!=NULL)//진짜 존재하는 것들만 그린다. 
                   {
                                          if(texture==NULL)//텍스쳐가 1순위 
                                          {
                                                          components[i]->Render(hdc,colorref);//색칠 
                                          }else
                                          {
                                               components[i]->Render(hdc,texture); //텍스쳐
                                          }
                   }
           }
           DrawID(centerX-IDlen/2,centerY,ID,IDlen);//중심에 그려야 하므로 계산 필요함
      }
};

LRESULT CALLBACK GameProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_RBUTTONDOWN:
             if(mode==TEAM)
                         MessageBox(hwnd,"team","팀전",MB_OK);
             if(mode==INDIVIDUAL)
                         MessageBox(hwnd,"개인전","bbb",MB_OK);
             
            break;
        case WM_KEYDOWN:
             switch(wParam)
             {
                           case VK_ESCAPE:
                                ;
                                break;
             }
             break;
        case WM_SIZE:
             GetClientRect(hwnd,&clientRect);
             sizeX=LOWORD(lParam);
             sizeY=HIWORD(lParam);
             if(doubleBufferBitmap!=NULL){
                  DeleteObject(doubleBufferBitmap);
                  doubleBufferBitmap=NULL;
             }
             break;
        case WM_TIMER:
             
             //엔터티 상태를 업데이트 한다.
             
             //이제 더블 버퍼에 뿌린다. 
             hdc=GetDC(hwnd);
             if (doubleBufferBitmap==NULL) {
                doubleBufferBitmap=CreateCompatibleBitmap(hdc,sizeX,sizeY);
             }
             doubleBufferDC=CreateCompatibleDC(hdc);
             OldBit=(HBITMAP)SelectObject(doubleBufferDC,doubleBufferBitmap);
             //Todo: 그리기 코드를 doublelBufferDC를 이용해 그림 
             //배경을 먼저 그린다.
             FillRect(doubleBufferDC,&clientRect,whiteBrush);
             //이제 엔터티들을 그린다.
             //줌에 따라서 그린다. zoom: 클수록 크게 보임 
             //클리핑 영역은 플레이어 위치가 중심이므로 플레이어 위치가 중심이 되도록 한다.
             //그릴 맵처음+그릴 맵끝=2*player_x,y
             //루프를 돌면서 이 구역 안에 들어오는 엔터티들을 그린다.
             //어차피 밖에 나가는 애들은 무시되니까.
             //그리는 방법은 각 엔터티들의 render함수를 doubleBufferDC를 사용하여 호출한다.
             //CellGroup껄 호출해야 겠군. 그러면 알아서 재귀 호출해 줄 것이니까.
             //절대 위치가 아니라 상대 위치로 그려야 함!!
               
             //이제 다 그렸으므로 정리하고 그리기 호출! 
             SelectObject(doubleBufferDC,OldBit);
             DeleteDC(doubleBufferDC);
             ReleaseDC(hwnd,hdc);         
             InvalidateRect(hwnd,NULL,FALSE);
             break;
        case WM_PAINT:
             hdc=BeginPaint(hwnd,&ps);
             DrawBitmap(hdc,0,0,doubleBufferBitmap);
             EndPaint(hwnd,&ps);
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

BOOL RectContains(LPRECT rect,int x,int y){
     if(rect->left<=x&&x<=rect->right&&rect->top<y&&y<rect->bottom)return TRUE;
     return FALSE;
}

void DrawBitmap(HDC hdc,int x,int y,HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx,by;
	BITMAP bit;

	MemDC=CreateCompatibleDC(hdc);
	OldBitmap=(HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;

	BitBlt(hdc,x,y,bx,by,MemDC,0,0,SRCCOPY);

	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}
void DrawID(int x,int y,char *ID,int IDlen)
{
     ;
}
