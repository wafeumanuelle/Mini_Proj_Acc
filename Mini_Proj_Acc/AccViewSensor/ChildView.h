
// ChildView.h : interface de la classe CChildView
//

#include "AccSensor.h"
#include "DrawView.h"

#pragma once

#define MAX_ECH 2000

// fenêtre CChildView

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributs
public:

  int m_zoom;

private:
  AccManager m_accManager;   // Gère (acces et surveillance) les capteurs
  AccSensor* m_pSensor;      // Pointeur vers le capteur 

  short ech_a[3][MAX_ECH];
  int nb_ech; //nb d'echantillons deja stocké
  bool m_acq; //pour savoir si on lit deja les données (acquisition en cours)

  void init_ech_a(); //fonction d'initialisation de ech_a

// Opérations
public:

// Substitutions
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implémentation
public:
	virtual ~CChildView();

	// Fonctions générées de la table des messages
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  afx_msg void OnStart();
  afx_msg void OnUpdateStart(CCmdUI *pCmdUI);
  afx_msg void OnUpdateZoomIn(CCmdUI *pCmdUI);
  afx_msg void OnUpdateZoomOut(CCmdUI *pCmdUI);
  afx_msg void OnSave();
  afx_msg void OnLire();
  afx_msg void OnUpdateSave(CCmdUI *pCmdUI);
  afx_msg void OnDraw();
};

