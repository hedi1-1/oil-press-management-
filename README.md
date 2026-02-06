# Application de Gestion des Clients - Olive Livery

## 📋 Présentation
Application Qt C++ pour gérer les clients d'une entreprise de livraison d'olives avec système de fidélité automatique.

## 🎨 Design
- **Couleur principale**: #1A3C2F (Vert foncé)
- **Interface**: 4 onglets avec widgets Qt
- **Données**: En mémoire (liste de clients)

## 📁 Structure du Projet
```
gestion_clients/
├── gestion_clients.pro      # Configuration Qt
├── mainwindow.h             # En-tête principal
├── mainwindow.cpp           # Implémentation principal
├── mainwindow.ui            # Interface (Qt Designer)
├── main.cpp                 # Point d'entrée
└── build/                   # Fichiers compilés
```

## 🔧 Compilation
```bash
qmake gestion_clients.pro
make
./gestion_clients  # ou gestion_clients.exe sur Windows
```

## 🎯 Fonctionnalités Principales

### 📌 Onglet 1: Gestion Client (CRUD)
- **Ajouter**: Créer un nouveau client
- **Modifier**: Mettre à jour les informations d'un client
- **Supprimer**: Supprimer un client
- **Consulter**: Afficher les informations d'un client sélectionné
- **Vider**: Réinitialiser le formulaire
- **Tableau**: Liste complète de tous les clients

**Attributs Client**:
- ID Client (auto-généré)
- Nom
- Prénom
- Téléphone
- Adresse
- Email
- Type Client (Particulier / Société)
- Total Olives Livrées (kg)
- Date Création
- Statut (auto-calculé)

### 🔍 Onglet 2: Recherche & Tri
**Recherche**:
- Critères: nom, prénom, téléphone, email, type_client
- Résultats en temps réel

**Tri**:
- Par nom, prénom, type_client
- Par total_olives_livrees (ordre décroissant)
- Par date_creation (plus récent d'abord)

**Exports**:
- PDF (fonctionnalité avancée)
- Excel (fonctionnalité avancée)
- Word (fonctionnalité avancée)
- TXT (implémenté)

### 📊 Onglet 3: Statistiques
- Nombre de clients par type
- Total olives par type de client
- Clients par date de création
- Distribution statut fidélité
- Classification importance

### ⭐ Onglet 4: Fidélité & Rapport
**Classification Automatique**:
```
Score Fidélité = total_olives_livrees / 100

Statut Fidélité:
├── Bronze: ≤ 2000 kg
├── Silver: 2000-5000 kg
├── Gold: 5000-8000 kg
└── Premium: > 8000 kg

Importance:
├── Standard: ≤ 1500 kg
├── Moyen: 1500-3000 kg
└── Important: > 3000 kg
```

**Rapport Client**:
- Informations personnelles
- Quantité totale livrée
- Date d'inscription
- Statut client
- Export PDF du rapport

## 📊 Données de Test
L'application démarre avec 3 clients de test:
1. Dupont Jean - Particulier - 2500 kg - Silver
2. Durand Marie - Société - 5500 kg - Gold
3. Martin Pierre - Particulier - 8500 kg - Premium

## 🔌 Connexions Signaux/Slots
Tous les boutons et tableaux sont connectés automatiquement via le système Qt:
- Nommage standard: `on_<objectName>_<signal>`
- Ex: `on_pushButton_ajouter_clicked()`

## 💾 Persistence des Données
Actuellement: **En mémoire seulement**
- Les données sont perdues à la fermeture de l'application
- Prêt pour une intégration avec une base de données (SQLite, MySQL, etc.)

## 📦 Dépendances Qt
- Core (listes, dates, strings)
- Gui (interfaces)
- Widgets (tableaux, formulaires)

## 🚀 Améliorations Futures
- [ ] Intégration base de données SQLite
- [ ] Export PDF avec Qt PDF
- [ ] Export Excel avec Qt5Xlsx
- [ ] Graphiques statistiques (QtCharts)
- [ ] Authentification utilisateur
- [ ] Sauvegarde/Restauration de données
- [ ] Email notifications pour fidélité
- [ ] API REST

## 👨‍💼 Auteur
Créé avec Qt 6.x

## 📝 Notes Importantes
1. Les **couleurs** utilisent la palette #1A3C2F (vert foncé) comme couleur primaire
2. Le **style** est appliqué via stylesheets Qt CSS
3. Les **interactions** respectent les conventions UX (confirmation avant suppression, etc.)
4. Le **code** est structuré pour faciliter l'extension future

---
**Status**: ✅ Interface complète et fonctionnalités CRUD opérationnelles
