// DrawView.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "AccViewSensor.h"
#include "DrawView.h"
#include "AccSensor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDrawView

IMPLEMENT_DYNAMIC(CDrawView, CWnd)

CDrawView::CDrawView(AccSensor* Sensor) : m_pSensor(Sensor), trace_stylo(true), clean(false), Colors(RGB(255, 255, 255))/*, cur(0, 0)*/, pre(0, 0), trace_pre(false)//, nb_point(0)
{
 
}

CDrawView::~CDrawView()
{
}


BEGIN_MESSAGE_MAP(CDrawView, CWnd)
  ON_WM_KEYUP()
  ON_WM_TIMER()
  ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CDrawView::ShowFull(CWnd* m_pWnd){
  if(m_hWnd != 0) return true;

  BOOL created;
  int cx = GetSystemMetrics(SM_CXSCREEN);
  int cy = GetSystemMetrics(SM_CYSCREEN);

  created = CreateEx(0, AfxRegisterWndClass(0, ::LoadCursor(0, IDC_CROSS), ::CreateSolidBrush(0x000000), 0), _T("DrawView"), WS_POPUP | WS_VISIBLE, 0, 0, cx, cy, m_hWnd, 0);

  //creation et demarrage d'un timer chaque 100ms = 0,1s = 10Hz
  //Initialisation des points, j'initialise ici car c'est là que l'on crée la fenetre
  if(created){ 
    pre.x = cx / 2;
    pre.y = cy / 2;
    trace_pre = false; 
    SetTimer(1, 100, NULL); //ID = 1
  }

  return created;
}

// Gestionnaires de messages de CDrawView

void CDrawView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

  CWnd::OnKeyUp(nChar, nRepCnt, nFlags);

  switch(nChar){
    case VK_F1:
      Colors = RGB(255, 0, 0);
      break;
    case VK_F2:
      Colors = RGB(0, 255, 0);
      break;
    case VK_F3:
      Colors = RGB(0, 0, 255);
      break;
    case VK_F12:
      trace_stylo = !trace_stylo;
      if (!trace_stylo) trace_pre = false;
      break;
    case VK_DELETE: //Suppr
      clean = true;
      Invalidate(); //pour redessiner
      break;
    case VK_ESCAPE:
      KillTimer(1);
      //PostMessage(WM_CLOSE);
      DestroyWindow();
      break;
    default:
      break;
  }
}

void CDrawView::OnTimer(UINT_PTR nIDEvent)
{
  // TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

  if (nIDEvent == 1){ //ID = 1
    if (m_pSensor){ //si m_pSensor non null 

      short axes[3];
     
      if (m_pSensor->ReadAcc(axes)){
        //On recupere ax et ay
        short ax = axes[0]; //ax
        short ay = axes[1]; //ay

        //on recupere les dimension de l'ecran et on cherche le centre pour l'origone du curseur
        CRect r;
        GetWindowRect(&r);
        int width = r.Width();
        int height = r.Height();
        int cx = width  / 2;
        int cy = height / 2;

        short g = AccSensor::G_ACC;
        double max_val = 2*g; //amplitude max 
        CPoint cur;
        cur.x = cx + (int)((ax / max_val) * (width  / 2));
        cur.y = cy - (int)((ay / max_val) * (height  / 2));

        //on trace directement sur la fenetre
        CClientDC dc(this);
        CPen pen(PS_SOLID, 5, Colors);
        CPen* pOldPen = dc.SelectObject(&pen);
        if (trace_stylo && trace_pre){
          dc.MoveTo(pre);
          dc.LineTo(cur);
        }
        dc.SelectObject(pOldPen);
        if (trace_stylo){
          pre = cur;
          trace_pre = true;
        } else {
          trace_pre = false;
        }
      }
    }
  }

  CWnd::OnTimer(nIDEvent);
}

void CDrawView::PostNcDestroy(){
  CWnd::PostNcDestroy();
  delete this;
}


void CDrawView::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  // TODO : ajoutez ici le code de votre gestionnaire de messages
  // N'appelez pas CWnd::OnPaint() pour la peinture des messages
  
  if (clean){ //si clean on remets nb_point à 0 et clean à false
    clean = false;
    trace_pre = false;
    return;
  }

}
