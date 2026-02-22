# 🫒 Gestion des Clients - Olive Livery

Application Qt C++ pour la gestion des clients d'une entreprise de livraison d'olives.

## 📋 Description

Application de bureau moderne développée avec Qt Widgets permettant la gestion complète des clients avec système de fidélité basé sur les quantités d'olives livrées.

## 🚀 Fonctionnalités

- **Gestion des clients** : CRUD complet (Créer, Lire, Modifier, Supprimer)
- **Système de fidélité** : Classification automatique (Bronze, Silver, Gold, Premium)
- **Niveau d'importance** : Standard, Moyen, Important
- **Types de clients** : Particuliers et Sociétés
- **Interface moderne** : Design professionnel avec thème vert olive
- **Date/heure en temps réel** : Mise à jour automatique

## 🛠️ Technologies

- **Qt 6.7.3** - Framework d'interface graphique
- **C++17** - Langage de programmation
- **MinGW 64-bit** - Compilateur
- **qmake** - Système de build

## 📁 Structure du projet

```
gestion_clients/
├── main.cpp              # Point d'entrée de l'application
├── client.h              # Déclarations (struct Client + MainWindow)
├── client.cpp            # Implémentations
├── mainwindow.ui         # Interface utilisateur Qt Designer
├── gestion_clients.pro   # Fichier de configuration qmake
└── README.md             # Ce fichier
```

## 🏗️ Compilation

### Prérequis
- Qt 6.7.3 installé dans `C:\Qt\6.7.3\`
- MinGW 64-bit installé dans `C:\Qt\Tools\mingw1120_64\`

### Commandes de build

```powershell
# Se placer dans le répertoire du projet
cd "c:\Users\MSI\Documents\new design\gestion_clients 21111"

# Ajouter Qt au PATH
$env:Path += ";C:\Qt\6.7.3\mingw_64\bin;C:\Qt\Tools\mingw1120_64\bin"

# Nettoyer et créer le répertoire build
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path build

# Générer le Makefile et compiler
cd build
qmake ../gestion_clients.pro
mingw32-make

# Lancer l'application
.\release\gestion_clients.exe
```

## 💾 Structure de données

### Client
```cpp
struct Client {
    int id_client;                    // Identifiant unique
    QString nom;                      // Nom du client
    QString prenom;                   // Prénom
    QString telephone;                // Numéro de téléphone
    QString adresse;                  // Adresse postale
    QString email;                    // Email
    QString type_client;              // "Particulier" ou "Société"
    double total_olives_livrees;      // Quantité totale en kg
    QDate date_creation;              // Date d'inscription
    QString statut;                   // Niveau de fidélité
    QString importance;               // Importance du client
};
```

### Classification de fidélité
- **Bronze** : < 2000 kg
- **Silver** : 2000 - 5000 kg
- **Gold** : 5000 - 8000 kg
- **Premium** : > 8000 kg

### Niveau d'importance
- **Standard** : < 1500 kg
- **Moyen** : 1500 - 3000 kg
- **Important** : > 3000 kg

## 🎨 Interface

L'application propose une interface moderne avec :
- Header vert olive professionnel avec logo
- Affichage de la date et heure en temps réel
- Indicateur de statut "EN LIGNE"
- Système d'onglets pour l'organisation des fonctionnalités
- Design responsive avec taille minimum 1200x550

## 📝 Licence

Projet développé pour la gestion interne d'Olive Livery.

## 👨‍💻 Développement

**Version** : 3.0 - Version simplifiée  
**Date** : Février 2026  
**Framework** : Qt 6.7.3  
**Compilateur** : MinGW 11.2.0 64-bit

---

**Note** : Ce projet utilise une architecture simplifiée avec un seul fichier source principal (client.cpp) contenant toute la logique métier pour faciliter la maintenance.
