# Gestion de l'Huilerie - Interface Machine

Ce fichier documente les composants de l'interface utilisateur (widgets) utilisés dans l'application de gestion des machines.

## Documentation de l'Interface

L'interface est organisée en plusieurs sections logiques pour faciliter la gestion et le suivi des machines de production d'huile.

### 1. En-tête (Header)
Cette zone fournit les informations contextuelles et la navigation de base.
- **`lblLogo_machine`** : Affiche le logo de l'entreprise/application.
- **`lblAppTitle_machine`** : Titre principal de la fenêtre ("GESTION DES MACHINES").
- **`lblDateTime_machine`** : Horloge temps réel affichant la date et l'heure actuelles.
- **`lblOnlineStatus_machine`** : Indicateur visuel de la connectivité avec le serveur central.
- **`btnBackToMenu_machine`** : Bouton de retour permettant de naviguer vers le menu principal de l'application.
- **`btnGestionStock_machine` / **`btnGestionProduction_machine`** : Raccourcis vers les autres modules de gestion.

### 2. Tableau de Bord & Statistiques
Une vue rapide de l'état global du parc machine.
- **`lblMachinesNormales_machine`** : Affiche le nombre total de machines opérationnelles (en vert).
- **`lblMachinesAlerte_machine`** : Affiche le nombre de machines nécessitant une vérification (en orange).
- **`lblMachinesPanne_machine`** : Affiche le nombre de machines actuellement à l'arrêt ou en panne (en rouge).
- **`btnSummaryActualiser_machine`** : Permet de rafraîchir instantanément les compteurs de statistiques.

### 3. Gestion du Parc Machines (Tableau Principal)
L'élément central pour la visualisation et l'interaction avec les données.
- **`tabWidgetMachineManagement`** : Organise l'interface en onglets (Parc Machines, Ajouter Machine, etc.).
- **`tableMachines`** : Vue tabulaire (`QTableView`) listant toutes les machines avec leurs attributs (Nom, Type, État, Criticité, etc.). Supporte le double-clic pour la modification.
- **`groupRecherche_machine`** : Contenu des filtres de recherche avancés.
  - `recherche_nom_machine` : Zone de texte pour rechercher une machine par son nom.
  - `filtre_type_machine` : Liste déroulante pour filtrer par catégorie de machine.
  - `filtre_etat_marche_machine` : Filtre selon l'état de fonctionnement.

### 4. Actions & Opérations
Outils pour manipuler les données des machines.
- **`tableMachines_machine`** : Vue tabulaire (`QTableView`) listant toutes les machines.
- **`btnActualiser_machine`** : Recharge la liste des machines depuis la base de données.
- **`btnExporter_machine`** : Génère un rapport d'exportation des données affichées.
- **`btnHistoriqueToggle_machine`** : Affiche ou masque le panneau latéral de l'historique.

### 5. Maintenance & Suivi (Todo List)
Zone dédiée à la gestion des interventions techniques.
- **`frameTodoList_machine`** : Conteneur de la liste des tâches de maintenance en attente.
- **`scrollTodoList_machine`** : Zone défilante contenant les widgets individuels de chaque tâche.
- **`btnAjouterTodo_machine`** : Ouvre un dialogue pour ajouter une nouvelle machine à la liste des réparations.
- **`scrollHistoriqueOnOff_machine`** : Panneau coulissant affichant les derniers événements enregistrés sur le parc.

### 6. Onglet "Actions" (Informations de la machine)
Cette section permet d'ajouter ou de modifier les détails d'une machine spécifique via un formulaire complet.
- **`lineEdit_nom_machine`** : Champ de saisie pour le nom ou l'identifiant de la machine (Obligatoire).
- **`comboBox_type_machine`** : Menu déroulant pour sélectionner le type (Presse, Broyeur, Malaxeur, Chauffeur).
- **`comboBox_etat_marche_machine`** : Définit l'état actuel de fonctionnement (ON, OFF, VEILLE).
- **`doubleSpinBox_temperature_machine`** : Contrôle de précision pour la température actuelle.
- **`doubleSpinBox_niveau_charge_machine`** : Indique le niveau de charge de la machine.
- **`comboBox_etat_fonctionnement_machine`** : État général de santé (Normal, Alerte, Panne).
- **`comboBox_type_alerte_machine`** : Catégorie d'alerte spécifique (Température, Maintenance, Sécurité).
- **`comboBox_niveau_criticite_machine`** : Niveau de priorité de l'état (Faible, Moyen, Élevé, Critique).
- **`dateEdit_derniere_maintenance_machine`** : Calendrier pour sélectionner la date de la dernière intervention technique.
- **`spinBox_score_sante_machine`** : Score numérique de 0 à 100 représentant la santé globale.
- **`pushButton_enregistrer_machine`** : Bouton pour enregistrer les données saisies en base de données.
- **`btnReinitialiser_machine`** : Remet à zéro tous les champs du formulaire.

### 7. Onglet "Statistiques"
Cette section fournit des outils d'analyse visuelle et de maintenance prédictive.
- **`type_statistique_machine`** : Sélection du type de données à analyser (État, Alertes, Température).
- **`type_graphique_machine`** : Choix du format de visualisation (BarChart, PieChart, LineChart).
- **`btnGenererStats_machine`** : Lance la génération du graphique dans la zone dédiée.
- **`zoneGraphique_machine`** : Conteneur dynamique où les graphiques sont rendus.
- **`tableMaintenance_machine`** : Tableau listant les machines nécessitant une attention.
- **`btnEnregistrerMaintenance_machine`** : Valide et enregistre une intervention effectuée.
- **`btnPlanifierMaintenance_machine`** : Ajoute une intervention au calendrier futur.

### 8. Onglet "Décision IA"
Interface dédiée aux recommandations générées par l'Intelligence Artificielle.
- **`lblRecommandationIA_machine`** : Zone de texte affichant l'analyse détaillée et les conseils de l'IA.
- **`progressConfianceIA_machine`** : Barre de progression indiquant le niveau de certitude de l'algorithme (0-100%).
- **`btnAppliquerDecision_machine`** : Permet d'exécuter automatiquement la recommandation suggérée par l'IA.
- **`btnIgnorerDecision_machine`** : Ferme ou ignore la suggestion actuelle.
