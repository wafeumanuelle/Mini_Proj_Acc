#include "qaccviewsensorserveur.h"

QAccViewSensorServeur::QAccViewSensorServeur(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags), m_pSensor(NULL)
{
  ui.setupUi(this);

  // initilisation du tableau de clients
  for (size_t idx = 0; idx < MAX_CLIENTS; ++idx) client[idx] = 0;

  connect(&serv, SIGNAL(newConnection()), this, SLOT(on_connexion()));

  serv.listen(QHostAddress::Any, SERV_PORT); //on ecoute sur le port
  connect(&tim, SIGNAL(timeout()), this, SLOT(send_one()));    

  // On initialise l’accéléromètre
  int nb_capteur = m_accManager.GetNbSensors();

  if (nb_capteur > 0){
    m_pSensor = m_accManager.GetSensor(0); // On lui donne le 1er capteur
    Msg(QString("Capteur détecté").arg(nb_capteur));
  }else{
    m_pSensor = NULL;
    Msg("Aucun capteur détecté !");
  }

}

QAccViewSensorServeur::~QAccViewSensorServeur()
{
  serv.close();
	for(size_t idx = 0; idx < MAX_CLIENTS; idx++){ 
		if(client[idx]){ //si le pointeur n'est pas nulle, on close
			client[idx]->close(); 
			client[idx]->deleteLater();
		}
	}
}
