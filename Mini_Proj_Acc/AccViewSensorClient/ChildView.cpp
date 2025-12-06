
// ChildView.cpp : implémentation de la classe CChildView
//

#include "stdafx.h"
#include "AccViewSensor.h"
#include "ChildView.h"
#include "DrawView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView() : m_pSensor(NULL), nb_ech(0), m_zoom(2), m_acq(false), connected(false)
{
  init_ech_a(); //initialisation de ech_a
}

CChildView::~CChildView() 
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
  ON_WM_CREATE()
  ON_WM_LBUTTONDOWN()
  ON_WM_TIMER()
  ON_COMMAND(ID_ZOOM_IN, &CChildView::OnZoomIn)
  ON_COMMAND(ID_ZOOM_OUT, &CChildView::OnZoomOut)
  ON_COMMAND(ID_START, &CChildView::OnStart)
  ON_UPDATE_COMMAND_UI(ID_START, &CChildView::OnUpdateStart)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CChildView::OnUpdateZoomIn)
  ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CChildView::OnUpdateZoomOut)
  ON_COMMAND(ID_SAVE, &CChildView::OnSave)
  ON_COMMAND(ID_LIRE, &CChildView::OnLire)
  ON_UPDATE_COMMAND_UI(ID_SAVE, &CChildView::OnUpdateSave)
  ON_COMMAND(ID_DRAW, &CChildView::OnDraw)
END_MESSAGE_MAP()



// gestionnaires de messages pour CChildView

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // contexte de périphérique pour la peinture
	
	// TODO : ajoutez ici le code de votre gestionnaire de messages
	
	// Ne pas appeler CWnd::OnPaint() pour la peinture des messages

  dc.SetMapMode(MM_ANISOTROPIC); //anisotropique

  short g = AccSensor::G_ACC;
  CRect r;
  GetClientRect(&r);
  
  int width = r.Width();
  int height = r.Height();

  int height_band = height / 3; //on decoupe la longueur en 3 pour les 3 courbes

  double max_val = 2*g; //car ReadAcc : +/-512
  double scaleY = (((height_band / 2.0) - 3.0) / max_val) * m_zoom; //on laisse unpeu de marge avec 3.0

  COLORREF Colors[3] = {RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255)};

  for (int i = 0; i < 3; i++){
    int top    = r.top + (i * height_band);
    int bottom = (i == 2) ? r.bottom : top + height_band;
    int centerY = (top + bottom) / 2;
    int yPlus  = centerY - (int)(3 * g * scaleY);
    int yMoins = centerY + (int)(3 * g * scaleY);
    
    CPen pen_a(PS_DOT, 1, Colors[i]); //PS_DOT pour les pointillés
    CPen* pOldPen = dc.SelectObject(&pen_a);
    dc.MoveTo(r.left,  centerY); //courbe pour les lignes 0g
    dc.LineTo(r.right, centerY);
    dc.MoveTo(r.left,  yPlus); //courbe pour les lignes +1g
    dc.LineTo(r.right, yPlus);
    dc.MoveTo(r.left,  yMoins); //courbe pour les lignes -1g
    dc.LineTo(r.right, yMoins);
    dc.SelectObject(pOldPen);

    if(nb_ech >= 2){ //au moins 2 echantillons
      int   nb_points = nb_ech;

      // Courbe des echantillons
      CPen pen(PS_SOLID, 1, Colors[i]);
      pOldPen = dc.SelectObject(&pen);

      // Premier point
      int x = r.left;
      int y = centerY - (int)(ech_a[i][0] * scaleY); //y est descendant donc -
      dc.MoveTo(x, y);
    
      // Points suivants
      for (int j = 1; j < MAX_ECH; j++){
        x = r.left + (int)(j);
        y = centerY - (int)(ech_a[i][j] * scaleY);
        dc.LineTo(x, y);
      }

      dc.SelectObject(pOldPen);

    }
  }
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  // TODO:  Ajoutez ici votre code de création spécialisé

  int nb_capteur = m_accManager.GetNbSensors();

  if (nb_capteur > 0){
    m_pSensor = m_accManager.GetSensor(0); // On lui donne le 1er capteur
  }else{
    m_pSensor = NULL;
  }

  return 0;
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) // juste pour tester AccManager et AccSensor : en cliquant, on lit (x, y, z)
{
  // TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

  CWnd::OnLButtonDown(nFlags, point);

  if (!m_pSensor){
    MessageBox(_T("Pas de capteur disponible"));
    return;
  }

  short axes[3]; // axes[0] = ax, axes[1] = ay, axes[2] = az

  if (m_pSensor->ReadAcc(axes)){
    CString s;
    s.Format(_T("ax = %d\n ay = %d\n az = %d"), axes[0], axes[1], axes[2]);
    MessageBox(s);
  }else{
    MessageBox(_T("Échec de la lecture de l'accéléromètre"));
  }

}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
  // TODO : ajoutez ici le code de votre gestionnaire de messages et/ou les paramètres par défaut des appels

  if (nIDEvent == 1){ //ID = 1
    if (m_pSensor && (nb_ech < MAX_ECH)){ //si m_pSensor non null et nb_ech < MAX_ECH
      short axes[3];
      if (m_pSensor->ReadAcc(axes))
      {
        ech_a[0][nb_ech] = axes[0]; //ax
        ech_a[1][nb_ech] = axes[1]; //ay
        ech_a[2][nb_ech] = axes[2]; //az
        nb_ech++;

        Invalidate();
      }else{ //si on ne lit pas les données du capteur
        KillTimer(1);
        m_acq = false;
      }
    }else if(nb_ech >= MAX_ECH){
      KillTimer(1);
      m_acq = false;
    }
  }

  CWnd::OnTimer(nIDEvent);
}

void CChildView::init_ech_a(){
  nb_ech = 0;

  for (int i = 0; i < 3; ++i){
    for (int j = 0; j < MAX_ECH; ++j){
      ech_a[i][j] = 0;
    }
  }
}
void CChildView::OnZoomIn()
{
  // TODO : ajoutez ici le code de votre gestionnaire de commande
  if (m_zoom < 8){
    m_zoom *= 2;
    Invalidate();
  }
}

void CChildView::OnZoomOut()
{
  // TODO : ajoutez ici le code de votre gestionnaire de commande
  if (m_zoom > 1){
    m_zoom /= 2;
    Invalidate();
  }
}

void CChildView::OnStart()
{
  // TODO : ajoutez ici le code de votre gestionnaire de commande

  //On utilise le bouton start pour la connexion
  if (connected){
    CAsyncSocket::Close(); //si c'est deja connecté, on se deconnecte 
    connected = false;
    m_acq = false;
    init_ech_a();
    Invalidate(FALSE);
  }else{ //sinon on tente une connexion au serveur
    if (!CAsyncSocket::Create()/*!CAsyncSocket::Create(CLI_PORT)*/){ //creation du socket client 
      MessageBox(_T("Erreur de création du socket client"));
      return;
    }

    CString addr = _T("127.0.0.1");

    // on démarre la connexion vers le serveur
    CAsyncSocket::Connect(addr, SERV_PORT);

  }  
}

void CChildView::OnUpdateStart(CCmdUI *pCmdUI)
{
  // TODO : ajoutez ici le code du gestionnaire d'interface utilisateur de mise à jour des commandes

  pCmdUI->SetCheck(m_acq);
  pCmdUI->Enable(m_pSensor != NULL); //grise le menu si aucun capteur n'est detecté
}

void CChildView::OnUpdateZoomIn(CCmdUI *pCmdUI)
{
  // TODO : ajoutez ici le code du gestionnaire d'interface utilisateur de mise à jour des commandes

  pCmdUI->Enable(m_zoom < 8);
}

void CChildView::OnUpdateZoomOut(CCmdUI *pCmdUI)
{
  // TODO : ajoutez ici le code du gestionnaire d'interface utilisateur de mise à jour des commandes

  pCmdUI->Enable(m_zoom > 1);
}

void CChildView::OnSave()
{
  // TODO : ajoutez ici le code de votre gestionnaire de commande

  if (nb_ech <= 0){
    MessageBox(_T("Aucun échantillon sauvegardé"));
    return;
  }

  // Boîte de dialogue pour "Enregistrer sous"
  CFileDialog dlg(FALSE, _T("ech"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT); //FALSE car on save

  if (dlg.DoModal() != IDOK) return;
  
  // Creation d'un fichier en mode ecriture sous format binaire
  CFile file;
  if (!file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)){
    MessageBox(_T("Impossible d'ouvrir le fichier pour écriture"));
    return;
  }

  //écrire les données, échantillon par échantillon
  for (int i = 0; i < nb_ech; ++i){
      file.Write(&ech_a[0][i], sizeof(short));
      file.Write(&ech_a[1][i], sizeof(short));
      file.Write(&ech_a[2][i], sizeof(short));
  }

  file.Close();
}

void CChildView::OnLire()
{
  // TODO : ajoutez ici le code de votre gestionnaire de commande

  CFileDialog dlg(TRUE, _T("ech"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST);

  if (dlg.DoModal() != IDOK) return;
  
  // Fichier en mode lecture sous format binaire
  CFile file;
  if (!file.Open(dlg.GetPathName(), CFile::modeRead  | CFile::typeBinary)){
    MessageBox(_T("Impossible d'ouvrir le fichier pour lire"));
    return;
  }

  //Recuperer la taille du fichier, s'assurer que x, y et z sont bien disponible sinon on close
  
  ULONGLONG size_file = file.GetLength();
  const ULONGLONG bytesPerSample = 3 * sizeof(short); //chaque echantillons = 3*2 
  if (size_file == 0 || (size_file % bytesPerSample) != 0){ //(size % bytesPerSample) != 0 pour verifier que l'on a bien x, y et z dans chaque echantillons
    MessageBox(_T("Fichier invalide"));
    file.Close();
    return;
  }

  //s'assurer que que l'on n'ai pas des echantillons > MAX_ECH 
  int n = (short)(size_file / bytesPerSample);
  if (n > MAX_ECH){
    MessageBox(_T("Trop d'échantillons dans le fichier"));
    file.Close();
    return;
  }

  init_ech_a(); //Remise à zero

  //S'assurer de la bonne lecture des échantillons sinon close
  for (int i = 0; i < n; ++i){
    short ax, ay, az;
    if (file.Read(&ax, sizeof(short)) != sizeof(short) || file.Read(&ay, sizeof(short)) != sizeof(short) || file.Read(&az, sizeof(short)) != sizeof(short)){
      MessageBox(_T("Fichier invalide"));
      file.Close();
      return;
    }
    ech_a[0][i] = ax;
    ech_a[1][i] = ay;
    ech_a[2][i] = az;
  }

  file.Close(); //Fermer le fichier apres avoir fini

  nb_ech = n;
  m_acq  = false;     // on n’est pas en mode acquisition
  KillTimer(1);       

  Invalidate();  
}

void CChildView::OnUpdateSave(CCmdUI *pCmdUI)
{
  // TODO : ajoutez ici le code du gestionnaire d'interface utilisateur de mise à jour des commandes

  pCmdUI->Enable(nb_ech > 0);
}

void CChildView::OnDraw()
{
  // TODO : ajoutez ici le code de votre gestionnaire de commande
  if (!m_pSensor){
    MessageBox(_T("Pas de capteur disponible"));
    return;
  }
  CDrawView* p_DrawWnd = new CDrawView(m_pSensor);
  if(!p_DrawWnd->ShowFull(this)){
    MessageBox(_T("Impossible de creer la fenetre"));
    delete p_DrawWnd;
  }
}

void CChildView::OnConnect(int nErrorCode)
{
  // TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

  if (nErrorCode == 0){
    connected = true; //pas d'erreur donc connexion ok
    m_acq = true;

    linger l = { 1, 0 };
    SetSockOpt(SO_LINGER, (const char*)&l, sizeof(l));

    init_ech_a();
    Invalidate();
  }else{
    MessageBox(_T("Connexion refusée"), _T("Erreur"), MB_OK | MB_ICONHAND);
    CAsyncSocket::Close();
    connected = false;
    m_acq = false;
  }

  __super::OnConnect(nErrorCode);
}

void CChildView::OnReceive(int nErrorCode)
{
  // TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

  if (nErrorCode == 0){
     char buffer[6]; //buffer pour recevoir les 6 octets

     int n = CAsyncSocket::Receive(buffer, sizeof(buffer));
     if (n == sizeof(buffer) && m_acq){
       //on stocke dans ax, ay et az
       short ax = *((short*)(buffer + 0));
       short ay = *((short*)(buffer + 2));
       short az = *((short*)(buffer + 4));

       //on le rempli dans notre tableau  d'echantillon
       if (nb_ech < MAX_ECH){
         ech_a[0][nb_ech] = ax;
         ech_a[1][nb_ech] = ay;
         ech_a[2][nb_ech] = az;
         nb_ech++;

         Invalidate();   
       }
     }
  }

  __super::OnReceive(nErrorCode);
}

void CChildView::OnClose(int nErrorCode)
{
  // TODO : ajoutez ici votre code spécialisé et/ou l'appel de la classe de base

  CAsyncSocket::Close();

  connected = false;
  m_acq = false;

  Invalidate();

  __super::OnClose(nErrorCode);
}
