# Explication detaillee du CRUD dans le module Production

Ce document explique comment le CRUD (Create, Read, Update, Delete) est implemente dans le module `production` de PressIQ.

Il couvre:
- les points d'entree UI (boutons, slots Qt)
- les classes/fichiers impliques
- les requetes SQL executees
- la logique metier autour du CRUD (simulation, rendement, qualite, rapports)
- les limites actuelles et pistes d'amelioration

---

## 1. Vue d'ensemble de l'architecture CRUD

Le CRUD est reparti sur deux couches principales:

1. Couche UI + orchestration metier
- Fichier principal: `production.cpp`
- Classe: `Production` (QMainWindow)
- Role:
  - Connecter les boutons aux slots
  - Lire/ecrire les champs UI
  - Lancer les requetes SQL directes pour beaucoup d'actions
  - Synchroniser les onglets (Planification, Lancement, Suivi, Rendement, Qualite, Rapports, Statistiques)

2. Couche modele (acces donnees utilitaire)
- Fichier: `productionmodel.cpp`
- Classe: `ProductionModel`
- Role:
  - Inserer une production (`addProduction`)
  - Modifier/supprimer via fonctions modele
  - Fournir modeles Qt (`QSqlQueryModel`, `QSqlTableModel`)
  - Recherche

Connexion base de donnees:
- `connection.cpp` expose `Connection::getInstance().createconnect()`
- Le nom de connexion SQL utilise dans le module est `production_conn`

---

## 2. CREATE (Creation)

### 2.1 Point d'entree UI
- Bouton: `btnPlanifier`
- Slot: `Production::onPlanifierClicked()`

### 2.2 Flux detaille
1. Lecture des champs Planification:
- quantite olives (`spinOlivesKg`)
- type pressage (`comboPressType`)
- notes (`txtNotesPlanification`)

2. Initialisation de l'objet metier:
- `ProductionModel prod;`
- date forcee a `QDate::currentDate()`
- statut initial: `Planifie`
- duree estimee calculee (~ `kg / 5`)
- huile/rendement/temps initialises a 0

3. Insertion SQL via modele:
- `prod.addProduction()`
- Requete INSERT dans `PRODUCTION` avec colonnes essentielles
- Recuperation de l'ID genere via sequence Oracle `SEQ_PRODUCTION.CURRVAL`

4. Post-traitement UI:
- Notification succes/erreur
- Mise a jour des labels de statut
- Recharge de l'historique + liste planifiee

### 2.3 Details importants
- Le modele nettoie `TYPEPRESSAGE` pour compatibilite Oracle/ODBC (suppression de caracteres non ASCII dans la logique actuelle du modele).
- Certains FK (client/user/machine/stock) sont forces a 0 dans le flux actuel de creation.

---

## 3. READ (Lecture)

La lecture est tres presente et alimente presque tous les onglets.

### 3.1 Historique general
- Methode: `Production::loadProductionHistory()`
- Requete: SELECT des colonnes cle (`IDPRODUCTION`, date, olives, huile, rendement, statut)
- Affichage: `tableProductionHistory`

### 3.2 Liste des productions planifiees
- Methode: `loadPlannedProductions()`
- Utilisee dans Lancement (combo de selection)
- Filtre principal sur statut planifie

### 3.3 Liste des productions terminees (Rendement)
- Methode: `loadTerminatedProductions()`
- Alimente `comboTerminatedProductions`
- Sert a precharger olives/huile/statut de la production cible

### 3.4 Liste des productions evaluables qualite
- Methode: `loadQualityEvaluableProductions()`
- Alimente `m_comboQualityProduction`
- Filtre sur plusieurs variantes de statuts termines/qualite validee

### 3.5 Lecture pour calculs et IA qualite
- Methode: `onQualityProductionSelected(int index)`
- Lit qualite existante, conformite, remarques, rendement, acidite
- Puis lance `autoEvaluateQuality(false, false)` pour une preview IA immediate

### 3.6 Lecture pour rapports
- Methode: `updateReportPreview()`
- Requetes agreges (COUNT, SUM, AVG) selon filtres de periode/qualite
- Alimente `textReport`, `lblProductionSummary`, `lblReportDate`

### 3.7 Lecture pour statistiques live
- Methode: `refreshStatistics()`
- KPIs globaux + series chart + pie chart
- Recharge periodique via `m_statsTimer`

---

## 4. UPDATE (Mise a jour)

Le module effectue plusieurs types de mises a jour:
- update de workflow (statut production)
- update de donnees metier (huile, rendement, temps)
- update qualite/IA
- update edition manuelle

### 4.1 Workflow de production (Lancement/Simulation)

#### a) Demarrer
- Slot: `onStartClicked()`
- Actions:
  - Verifie la selection dans la combo planifiee
  - Charge quantite/duree
  - Passe le statut a `En cours`
  - Demarre timer simulation (`m_simTimer`)

#### b) Pause/Arret
- Slot: `onStopClicked()`
- Actions:
  - Stop timer
  - Statut mis a `Arrete`
  - Sauvegarde `TEMPSECOULE`

#### c) Tick simulation
- Slot: `onSimTick()`
- Actions:
  - Calcule progression/phase/huile/rendement simules
  - Met a jour UI (Lancement + Suivi)
  - Toutes les 10 ticks: UPDATE en base
  - A 100%: finalise statut `Termine`

#### d) Finaliser manuellement
- Slot: `onFinishProductionClicked()`
- Actions:
  - UPDATE final (statut termine, huile, rendement, temps)
  - Reinitialise UI simulation

### 4.2 Rendement
- Slot: `onCalculateYieldClicked()`
- Calcul: `rendement = huile / olives * 100`
- Update SQL des champs `RENDEMENT` + `HUILEPRODUITEL`
- Synchronisation historique/listes/rapports/stats

### 4.3 Qualite et IA

#### a) Preview IA sans persistance
- Methode: `autoEvaluateQuality(false, false)`
- Evalue score/risque/recommandations et affiche dans le resume

#### b) Validation qualite avec persistance
- Slot: `onValidateQualityClicked()`
- Appel: `autoEvaluateQuality(true, true)`
- Update SQL principal:
  - `QUALITE`
  - `CONFORMENORMES`
  - `REMARQUESQUALITE`
  - `STATUT='Qualite validee'`
  - `ACIDITE`, `SCOREQUALITEIA`, `CONFIANCEIA` (si dispo)
- Avec fallback SQL simplifie si certaines colonnes n'existent pas

### 4.4 Modification manuelle d'une production
- Slot: `onModifyClicked()`
- Ouvre `ProductionEditDialog`
- Recharge ligne complete depuis DB
- Recalcule rendement a la sauvegarde
- Execute UPDATE multi-champs

---

## 5. DELETE (Suppression)

### 5.1 Point d'entree UI
- Bouton: `btnDeleteProduction`
- Slot: `onDeleteClicked()`

### 5.2 Flux
1. Verifie qu'une ligne historique est selectionnee
2. Recupere l'ID
3. Demande confirmation via `QMessageBox::question`
4. Appelle `ProductionModel::deleteProduction(id)`
5. Recharge l'historique

### 5.3 Niveau modele
- `ProductionModel::deleteProduction(int idProduction)`
- Requete: `DELETE FROM PRODUCTION WHERE IDPRODUCTION = :idProduction`
- Gestion de retour:
  - `numRowsAffected()`
  - analyse d'erreurs de contrainte/fk

---

## 6. Mapping rapide boutons -> slots CRUD

- `btnPlanifier` -> `onPlanifierClicked` (CREATE)
- `btnUpdateMonitoring` -> `onRefreshHistoryClicked` (READ refresh)
- `btnRefreshPlanned` -> `onRefreshPlannedClicked` (READ)
- `btnRefreshTerminated` -> `onRefreshTerminatedClicked` (READ)
- `btnCalculateYield` -> `onCalculateYieldClicked` (UPDATE)
- `btnValidateQuality` -> `onValidateQualityClicked` (UPDATE)
- `btnStart` -> `onStartClicked` (UPDATE statut)
- `btnStop` -> `onStopClicked` (UPDATE statut)
- `btnFinishProduction` -> `onFinishProductionClicked` (UPDATE final)
- `btnModifyProduction` -> `onModifyClicked` (UPDATE)
- `btnDeleteProduction` -> `onDeleteClicked` (DELETE)

---

## 7. Points forts de l'implementation actuelle

- Couverture fonctionnelle complete du cycle production
- Bon niveau de feedback utilisateur (notifications, labels, badges)
- Synchronisation multi-onglets apres operations CRUD
- Presence d'un module qualite IA avec score/confiance/risque
- Rapports et PDF avec filtres

---

## 8. Limites techniques actuelles

1. Requetes SQL dispersees dans `production.cpp`
- La logique CRUD est partiellement dans le modele et partiellement dans la fenetre
- Difficulte de maintenance/test

2. Statuts non totalement uniformes
- Variantes (`Termine`, `Terminee`, `Terminé`, etc.) gerees a plusieurs endroits
- Risque d'oublis dans un nouveau filtre

3. Transactions insuffisantes
- Certaines sequences critiques devraient etre atomiques (start/stop/finish/quality validation)

4. Gestion d'erreurs SQL heterogene
- Fallback utile sur qualite, mais pas generalise

5. Couplage UI/metier fort
- Beaucoup de code metier dans les slots UI

---

## 9. Recommandations pour fiabiliser le CRUD

1. Introduire un `ProductionService`
- Deplacer SQL + regles metier hors UI

2. Unifier les statuts
- Constantes/enum centralises
- Fonctions utilitaires de mapping

3. Encapsuler les operations critiques dans transactions
- Debut/commit/rollback

4. Centraliser validation d'entrees
- Fonctions reutilisables pour planifier, rendement, qualite

5. Ajouter tests fonctionnels minimaux
- CREATE reussi
- UPDATE statut (start -> stop -> finish)
- DELETE avec et sans contraintes

---

## 10. Resume ultra-court

- CREATE: `onPlanifierClicked` -> `ProductionModel::addProduction`
- READ: listes + historique + rapports + stats via `SELECT`
- UPDATE: workflow simulation, rendement, qualite IA, edition popup
- DELETE: `onDeleteClicked` -> `ProductionModel::deleteProduction`

Le CRUD est complet et operationnel, mais gagnerait en robustesse avec une meilleure separation des couches et des transactions systematiques.
