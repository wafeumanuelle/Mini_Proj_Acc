#ifndef QACCVIEWSENSORSERVEUR_H
#define QACCVIEWSENSORSERVEUR_H

#include <QtGui/QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QHostAddress>
#include "ui_qaccviewsensorserveur.h"
#include "AccSensor.h"

#define SERV_PORT   4040
#define MAX_CLIENTS 50

class QAccViewSensorServeur : public QMainWindow
{
    Q_OBJECT

    QTcpServer  serv;
    QTcpSocket* client[MAX_CLIENTS];
    QTimer      tim;

    AccManager m_accManager;
    AccSensor* m_pSensor;

public:
    QAccViewSensorServeur(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~QAccViewSensorServeur();

    void Msg(const QString& s){
      ui.m_msg->addItem(s);
      ui.m_msg->scrollToBottom();
      ui.m_msg->repaint();
    }

public slots:
    void on_connexion(){
      size_t idx = 0;
      while (idx < MAX_CLIENTS && client[idx] != 0) idx++; //tant qu'un idx ne depasse pas max_client et que le client n'est pas libre on continue

      QTcpSocket* pcli = serv.nextPendingConnection();
      if (!pcli) return;

      if (idx < MAX_CLIENTS){
        Msg(QString("Connexion acceptée pour %1:%2").arg(pcli->peerAddress().toString()).arg(pcli->peerPort()));
        client[idx] = pcli;
        connect(pcli, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
      }else{
        Msg(QString("Connexion refusée pour %1:%2").arg(pcli->peerAddress().toString()).arg(pcli->peerPort()));
        pcli->close();
        pcli->deleteLater();
      }
    }

    void on_disconnected(){
      QTcpSocket* pcli = (QTcpSocket*)(sender());
      if (!pcli) return;

      for (size_t idx = 0; idx < MAX_CLIENTS; ++idx){
        if (client[idx] == pcli){
          Msg(QString("Fin de connexion pour %1:%2").arg(pcli->peerAddress().toString()).arg(pcli->peerPort()));
          client[idx] = 0;
          pcli->close();
          pcli->deleteLater();
          break;
        }
      }
    }

    void on_m_kill_all_released(){
      for (size_t idx = 0; idx < MAX_CLIENTS; ++idx){
        if (client[idx]){
            client[idx]->close();
            client[idx]->deleteLater();
            client[idx] = 0;
        }
      }
      Msg("Fin de la connexion pour tout le monde");
    }

    void send_one(){ //envoie periodique d'un echantillon
      if (!m_pSensor){
        Msg("Pas de capteur");
        return;
      }

      short axes[3];  
      if (!m_pSensor->ReadAcc(axes)){
        Msg("Échec de la lecture de l'accéléromètre");
        return;
      }

      // buffer pour stocker ax, ay et az
      char buffer[6];
      ::memcpy(buffer + 0, &axes[0], sizeof(short));  // ax
      ::memcpy(buffer + 2, &axes[1], sizeof(short));  // ay
      ::memcpy(buffer + 4, &axes[2], sizeof(short));  // az

      // On envoie à tous les clients actifs
      int nb_client = 0;
      for (size_t idx = 0; idx < MAX_CLIENTS; ++idx){
        if (client[idx])
        {
          ++nb_client;
          int n = client[idx]->write((char*)axes, sizeof(axes));
          if (n != sizeof(buffer))
          {
            Msg(QString("erreur d’envoi vers un client").arg(n));
          }else{
            //Msg(QString("Envoie : %1, %2, %3").arg(axes[0]).arg(axes[1]).arg(axes[2]));
          }
        }
      }

      if (nb_client > 0){
        Msg(QString("Envoi ax=%1 ay=%2 az=%3 (%4 client(s))").arg((int)axes[0]).arg((int)axes[1]).arg((int)axes[2]).arg(nb_client));
      }
  }

  void on_m_send_released(){

    //Msg("on_m_send_released() appelé"); //test

    tim.setInterval(200); 

    if(!tim.isActive()){
      tim.start();
      ui.m_send->setText("Stop send");
      send_one();
    }else{
      tim.stop();
      ui.m_send->setText("Send");
    }
	}

private:
    Ui::QAccViewSensorServeurClass ui;
};

#endif // QACCVIEWSENSORSERVEUR_H
