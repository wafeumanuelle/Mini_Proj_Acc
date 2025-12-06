#include "AccSensor.h"

#pragma once

#define MAX_POINTS 2000

// CDrawView

class CDrawView : public CWnd
{
	DECLARE_DYNAMIC(CDrawView)

public:
  CDrawView(AccSensor* Sensor);

  virtual ~CDrawView();

  BOOL ShowFull(CWnd* m_pWnd = 0); //cree et affiche la fenetre plein ecran

protected:
  virtual void PostNcDestroy();

private:
  AccSensor* m_pSensor; // Pointeur vers le capteur 
  bool clean; //pour effacer les traces de la fenetre 
  bool trace_stylo; //Pour savoir si on doit activer ou desactiver la trace du stylo 
  COLORREF Colors; //Couleur du stylo

  CPoint pre; // position précédente
  bool trace_pre; //si on a une trace precedente

protected:
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnPaint();
};


