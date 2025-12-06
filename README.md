# Mini-projet Accéléromètre USB

Ce dépôt contient un mini-projet C++ de visualisation en temps réel d’un accéléromètre USB (interface HID) et sa déclinaison en architecture client–serveur.

Le projet est composé de **trois exécutables principaux** :

- **AccViewSensor** : application MFC locale qui lit directement le capteur.
- **QAccViewSensorServeur** : application Qt qui joue le rôle de serveur, lit le capteur et diffuse les données sur le réseau.
- **AccViewSensorClient** : application MFC cliente qui se connecte au serveur et affiche les mesures reçues.

---

## 1. Fonctionnalités

### AccViewSensor (MFC)

- Détection et ouverture du capteur d’accélération USB via l’interface HID.
- Acquisition en temps réel des trois composantes **X, Y, Z**.
- Affichage graphique sous forme de courbes dans une vue dédiée.
- Possibilité de :
  - **Sauvegarder** les signaux dans des fichiers binaires `*.ech`.
  - **Recharger** des fichiers `*.ech` pour rejouer une acquisition.

### QAccViewSensorServeur (Qt)

- Lecture des mesures du capteur (réel ou virtuel) en continu.
- Mise à disposition des données sur un **port TCP** (constante `SERV_PORT`).
- Gestion d’un tableau de clients (`MAX_CLIENTS`) et envoi périodique des mesures à chaque client connecté.

### AccViewSensorClient (MFC)

- Connexion au serveur via **adresse IP** (par défaut `127.0.0.1`) et `SERV_PORT`.
- Réception des mesures d’accélération et **affichage des courbes** comme dans AccViewSensor.
- Utilisation possible pour tester le serveur avec plusieurs clients (fonctionnel, mais à stabiliser).

---

## 2. Fichiers importants

- `AccSensor.h`  
  Interface d’accès au capteur d’accélération (ou capteur virtuel) via HID.

- `DrawView.*` (MFC)  
  Vue graphique pour l’affichage des signaux (courbes X/Y/Z, gestion du rafraîchissement).

- `QAccViewSensorServeur.*` (Qt)  
  Fenêtre principale du serveur, gestion des sockets, de la liste des clients et du timer d’envoi.

- `test1.ech`, `test2.ech`, `test3.ech`  
  Exemples de fichiers d’acquisition pour tester le chargement et l’affichage hors ligne.

> D’autres fichiers de projet (Doc/View, MainFrm, ressources…) sont ceux générés par les assistants MFC/Qt et assurent la structure des applications.

---

## 3. Format des fichiers `.ech`

Les fichiers `*.ech` contiennent les échantillons d’accélération enregistrés :

- Données binaires (non texte).
- Chaque échantillon stocke les valeurs des trois axes **X, Y, Z**.
- Utilisés par AccViewSensor pour :
  - rejouer une acquisition,
  - valider l’affichage sans capteur branché.

---

## 4. Compilation & dépendances

- **Plateforme** : Windows.
- **Langage** : C++.
- **Bibliothèques** :
  - MFC pour `AccViewSensor` et `AccViewSensorClient`.
  - Qt (Widgets, réseau) pour `QAccViewSensorServeur`.
  - API HID de Windows :
    - `hid.lib`
    - Headers `hidsdi.h`, `hidusage.h`.

Configuration générale :

1. Ajouter le chemin des headers HID (`hidsdi.h`, `hidusage.h`, etc.) dans les **include paths**.
2. Lier la bibliothèque `hid.lib` dans les options de l’éditeur de liens.
3. Pour le projet Qt, s’assurer que les modules **widgets** et **network** sont activés (`QT += widgets network` dans le `.pro`).

---

## 5. Utilisation

### Mode local – AccViewSensor

1. Brancher l’accéléromètre USB (ou lancer le capteur virtuel si prévu).
2. Lancer **AccViewSensor**.
3. Vérifier que le capteur est détecté.
4. Observer les courbes X/Y/Z en temps réel.
5. Optionnel :
   - Sauvegarder une acquisition au format `.ech`.
   - Recharger un fichier `.ech` pour rejouer les données.

### Mode client–serveur

1. Lancer **QAccViewSensorServeur**.
   - Le serveur écoute sur `SERV_PORT` (par défaut sur toutes les interfaces).
   - Le capteur doit être détecté pour que des données soient envoyées.
2. Lancer un ou plusieurs **AccViewSensorClient**.
   - Saisir l’adresse IP du serveur (par défaut `127.0.0.1` si tout est en local).
   - Se connecter et vérifier l’affichage des courbes.
3. Tester plusieurs connexions clients (fonctionnel mais nécessite encore des tests intensifs).

---

## 6. Limitations & améliorations possibles

- **Multi-clients** :  
  La gestion de plusieurs clients simultanés est en place mais reste à stabiliser (tests supplémentaires nécessaires).

- **Bouton “Déconnecter tout”** :  
  La fonctionnalité est implémentée de manière partielle et pourrait être améliorée (fermeture propre des sockets, remise à zéro de l’interface).

- **Interface utilisateur** :
  - Affichage plus clair du nombre d’échantillons en cours.
  - Messages d’erreur plus explicites lorsque le capteur n’est pas détecté (capteur réel ou virtuel).

- **Paramétrage réseau** :
  - Rendre le port et l’adresse IP configurables via l’interface.

---

