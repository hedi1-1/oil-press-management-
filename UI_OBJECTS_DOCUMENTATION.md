# PressIQ - UI Objects Documentation

**Version:** 2.0  
**Last Updated:** February 8, 2026  
**System:** Smart Oil Press Management System

---

## Table of Contents
1. [Menu Interface (menu.ui)](#menu-interface-menuui)
2. [Production Interface (production.ui)](#production-interface-productionui)
3. [Machine/Alerts Interface (machine.ui)](#machinealerts-interface-machineui)
4. [User & Staff Interface (userstaff.ui)](#user--staff-interface-userstaffui)
5. [Client Management Interface (gestionclients.ui)](#client-management-interface-gestionclientsui)
6. [Stock Management Interface (stock.ui)](#stock-management-interface-stockui)

---

## Menu Interface (menu.ui)

### Main Window
- **menu** (QMainWindow): Main application window (1440x616)

### Containers/Widgets
- **centralwidget** (QWidget): Central widget container
- **headerWidget** (QWidget): Header section container
- **contentWidget** (QWidget): Main content area container
- **welcomeWidget** (QWidget): Welcome message container
- **statsWidget** (QWidget): Statistics dashboard container
- **statCard1** (QWidget): First statistics card
- **statCard2** (QWidget): Second statistics card
- **statCard3** (QWidget): Third statistics card
- **statCard4** (QWidget): Fourth statistics card
- **footerWidget** (QWidget): Footer section container

### Buttons
- **btnClientManagement** (QPushButton): Opens client management module
- **btnUserStaffManagement** (QPushButton): Opens user and staff management module
- **btnStockManagement** (QPushButton): Opens stock management module
- **btnProductionManagement** (QPushButton): Opens production management module (active)
- **btnFinanceManagement** (QPushButton): Opens financial management module
- **btnMachineAlertManagement** (QPushButton): Opens machine and alert management module (active)

### Labels

#### Header Labels
- **lblLogo** (QLabel): Displays PressIQ logo image (70x70px)
- **lblAppTitle** (QLabel): Displays application title "PressIQ"
- **lblAppSubtitle** (QLabel): Displays subtitle "SYSTÈME INTELLIGENT DE GESTION D'HUILERIE"
- **lblDateTime** (QLabel): Displays current date and time with calendar emoji
- **lblOnlineStatus** (QLabel): Displays system online status "● SYSTÈME EN LIGNE"

#### Welcome Section Labels
- **lblWelcome** (QLabel): Welcome message "👋 Bienvenue dans PressIQ"
- **lblWelcomeDesc** (QLabel): Description of module selection instructions

#### Statistics Labels
- **lblStatIcon1** (QLabel): Icon for production statistics (🏭)
- **lblStatValue1** (QLabel): Value display for production count
- **lblStatLabel1** (QLabel): Label "PRODUCTIONS"
- **lblStatIcon2** (QLabel): Icon for oil production statistics (🫒)
- **lblStatValue2** (QLabel): Value display for oil produced in liters
- **lblStatLabel2** (QLabel): Label "HUILE PRODUITE"
- **lblStatIcon3** (QLabel): Icon for yield statistics (📊)
- **lblStatValue3** (QLabel): Value display for yield percentage
- **lblStatLabel3** (QLabel): Label "RENDEMENT"
- **lblStatIcon4** (QLabel): Icon for machine statistics (⚙️)
- **lblStatValue4** (QLabel): Value display for machine count
- **lblStatLabel4** (QLabel): Label "MACHINES"

#### Other Labels
- **lblModulesTitle** (QLabel): Section title "📱 Modules du système"
- **lblFooter** (QLabel): Footer text with copyright information

---

## Production Interface (production.ui)

### Main Window
- **Production** (QMainWindow): Production management window (1440x616)

### Container Widgets
- **centralwidget** (QWidget): Central widget
- **headerWidget** (QWidget): Header section
- **dashboardWidget** (QWidget): Dashboard statistics section
- **dashCard1** (QWidget): Production status card
- **dashCard2** (QWidget): Olives quantity card
- **dashCard3** (QWidget): Expected yield card
- **dashCard4** (QWidget): Alerts card
- **tabContainerWidget** (QWidget): Tab container

### Tab Widget
- **tabWidgetProduction** (QTabWidget): Main production tabs container

#### Tab 1: Planification
- **tabPlanification** (QWidget): Production planning tab

##### Group Boxes
- **groupProductionParams** (QGroupBox): Production parameters section
- **groupNotesPlanification** (QGroupBox): Planning notes section
- **groupPlanStatus** (QGroupBox): Planning status section
- **groupQuickStats** (QGroupBox): Quick statistics section

##### Input Fields
- **dateEditProduction** (QDateEdit): Production date picker with calendar popup
- **spinOlivesKg** (QSpinBox): Olives quantity input (kg), range 0-100000
- **comboPressType** (QComboBox): Press type selection (Cold/Hot)
- **comboOliveVariety** (QComboBox): Olive variety selection (Chemlali, Chétoui, etc.)
- **txtNotesPlanification** (QTextEdit): Planning notes text area

##### Labels
- **lblDateProduction** (QLabel): "📅 Date de production :"
- **lblOlivesQty** (QLabel): "⚖ Quantité d'olives :"
- **lblPressType** (QLabel): "🔧 Type de pressage :"
- **lblOliveVariety** (QLabel): "🫒 Variété d'olive :"
- **lblStatusPlanification** (QLabel): Planning status display
- **lblEstimatedYield** (QLabel): Estimated yield display
- **lblDuration** (QLabel): Estimated duration display
- **lblTodayProductions** (QLabel): "Productions aujourd'hui"
- **lblTodayProductionsValue** (QLabel): Today's production count value
- **lblWeekProductions** (QLabel): "Cette semaine"
- **lblWeekProductionsValue** (QLabel): Week's production count value
- **lblAvgYield** (QLabel): "Rendement moyen"
- **lblAvgYieldValue** (QLabel): Average yield percentage value

##### Buttons
- **btnPlanifier** (QPushButton): "✓ Planifier la production" - Confirms production planning

#### Tab 2: Lancement
- **tabLancement** (QWidget): Production launch tab

##### Group Boxes
- **groupLancementStatus** (QGroupBox): Launch status section
- **groupProgress** (QGroupBox): Production progress section
- **groupControls** (QGroupBox): Production controls section

##### Labels
- **lblCurrentStatusTitle** (QLabel): "STATUT ACTUEL"
- **lblCurrentStatus** (QLabel): Current production status display
- **lblStartTimeTitle** (QLabel): "HEURE DE DÉMARRAGE"
- **lblStartTime** (QLabel): Start time display
- **lblElapsedTimeTitle** (QLabel): "TEMPS ÉCOULÉ"
- **lblElapsedTime** (QLabel): Elapsed time display
- **lblPhase** (QLabel): Current production phase
- **lblProgressEnd** (QLabel): Progress completion display (kg treated)

##### Progress
- **progressProduction** (QProgressBar): Production progress percentage (0-100%)

##### Buttons
- **btnStart** (QPushButton): "▶ Démarrer" - Starts production
- **btnStop** (QPushButton): "⏹ Arrêter" - Stops production

#### Tab 3: Suivi
- **tabSuivi** (QWidget): Production monitoring tab

##### Group Boxes
- **groupRunningProduction** (QGroupBox): Running production section
- **groupHistory** (QGroupBox): Production history section

##### Labels
- **lblRunningProduction** (QLabel): Running production information display

##### Table
- **tableProductionHistory** (QTableWidget): Production history table with columns:
  - Date
  - Olives (kg)
  - Huile (L)
  - Rendement
  - Statut

##### Buttons
- **btnUpdateMonitoring** (QPushButton): "🔄 Actualiser le suivi" - Refreshes monitoring

#### Tab 4: Rendement
- **tabRendement** (QWidget): Yield calculation tab

##### Group Boxes
- **groupRendementInput** (QGroupBox): Yield input data section
- **groupRendementResult** (QGroupBox): Yield result section
- **groupYieldMessage** (QGroupBox): Evaluation message section

##### Labels
- **lblOlivesUsedTitle** (QLabel): "🫒 Olives utilisées :"
- **lblOlivesUsed** (QLabel): Olives quantity display
- **lblOilProducedTitle** (QLabel): "🛢 Huile produite :"
- **lblYield** (QLabel): Yield percentage result display
- **lblYieldMessage** (QLabel): Yield evaluation message

##### Input Fields
- **spinOilProduced** (QDoubleSpinBox): Oil produced quantity input (L), range 0-99999.99

##### Progress
- **progressYield** (QProgressBar): Yield percentage progress bar

##### Buttons
- **btnCalculateYield** (QPushButton): "📊 Calculer le rendement" - Calculates yield

#### Tab 5: Qualité
- **tabQualite** (QWidget): Quality control tab

##### Group Boxes
- **groupQualityEval** (QGroupBox): Quality evaluation section
- **groupQualityNotes** (QGroupBox): Quality remarks section
- **groupQualitySummary** (QGroupBox): Quality summary section

##### Labels
- **lblOilQuality** (QLabel): "⭐ Qualité de l'huile :"
- **lblAcidity** (QLabel): "🧪 Acidité :"
- **lblQualitySummary** (QLabel): Quality summary display

##### Input Fields
- **comboOilQuality** (QComboBox): Oil quality selection (Excellente, Bonne, Moyenne, Faible)
- **comboAcidity** (QComboBox): Acidity level selection (<0.8%, 0.8%-2%, >2%)
- **txtQualityNotes** (QTextEdit): Quality remarks text area
- **checkConformity** (QCheckBox): "✓ Conforme aux normes de qualité" - Quality conformity checkbox

##### Buttons
- **btnValidateQuality** (QPushButton): "✓ Valider l'évaluation qualité" - Validates quality evaluation

#### Tab 6: Rapports
- **tabRapports** (QWidget): Reports generation tab

##### Group Boxes
- **groupReportSummary** (QGroupBox): Report summary section
- **groupReportContent** (QGroupBox): Report content section

##### Labels
- **lblProductionSummary** (QLabel): Production summary display
- **lblReportDate** (QLabel): Last report generation date

##### Text Display
- **textReport** (QTextBrowser): Report content HTML display

##### Buttons
- **btnGenerateReport** (QPushButton): "📊 Générer rapport" - Generates production report
- **btnExportPDF** (QPushButton): "📥 Exporter PDF" - Exports report to PDF

### Header Section
#### Labels
- **lblLogo** (QLabel): PressIQ logo (50x50px)
- **lblAppTitle** (QLabel): "Gestion de la Production"
- **lblAppSubtitle** (QLabel): "MODULE DE PRODUCTION INTELLIGENT"
- **lblDateTime** (QLabel): Date and time display
- **lblOnlineStatus** (QLabel): "● EN LIGNE"

#### Buttons
- **btnBackToMenu** (QPushButton): "← Retour au menu" - Returns to main menu

### Dashboard Statistics Labels
- **lblDashIcon1** (QLabel): Production status icon (▶)
- **lblDashValue1** (QLabel): Production status value
- **lblDashLabel1** (QLabel): "STATUT PRODUCTION"
- **lblDashIcon2** (QLabel): Olives icon (🫒)
- **lblDashValue2** (QLabel): Planned olives quantity
- **lblDashLabel2** (QLabel): "OLIVES PLANIFIÉES"
- **lblDashIcon3** (QLabel): Yield icon (📊)
- **lblDashValue3** (QLabel): Expected yield value
- **lblDashLabel3** (QLabel): "RENDEMENT ESTIMÉ"
- **lblDashIcon4** (QLabel): Alert icon (🔔)
- **lblDashValue4** (QLabel): Active alerts count
- **lblDashLabel4** (QLabel): "ALERTES ACTIVES"

---

## Machine/Alerts Interface (machine.ui)

### Main Window
- **machine** (QMainWindow): Machine and alert management window (1441x616)

### Container Widgets
- **centralwidget** (QWidget): Central widget
- **headerWidget** (QWidget): Header section
- **frameSummary** (QFrame): Summary statistics frame
- **zoneGraphique** (QWidget): Statistics chart display area

### Tab Widget
- **tabWidgetMachineManagement** (QTabWidget): Main machine management tabs

#### Tab 1: Parc Machines
- **tabParcMachines** (QWidget): Machine fleet tab

##### Group Boxes
- **groupRecherche** (QGroupBox): Search and filter section

##### Input Fields
- **recherche_nom_machine** (QLineEdit): Machine name search field
- **filtre_type_machine** (QComboBox): Machine type filter (Presse, Broyeur, Malaxeur, Chauffeur)
- **filtre_etat_marche** (QComboBox): Operating state filter (ON, OFF, VEILLE)
- **filtre_type_alerte** (QComboBox): Alert type filter (Aucune, Température, Surcharge, Panne)
- **tri_par** (QComboBox): Sort by field selector

##### Table
- **tableMachines** (QTableView): Machines list table

##### Toolbar
- **toolBarMachines** (QToolBar): Machine management toolbar

##### Summary Labels
- **lblMachinesNormales** (QLabel): "✓ Machines normales: 0" - Normal machines count
- **lblMachinesAlerte** (QLabel): "⚠ Machines en alerte: 0" - Alert machines count
- **lblMachinesPanne** (QLabel): "✗ Machines en panne: 0" - Broken machines count

##### Buttons
- **btnRechercher** (QPushButton): "🔍 Rechercher" - Executes search

#### Tab 2: Ajouter / Modifier
- **tabAjouterModifier** (QWidget): Add/modify machine tab

##### Group Box
- **groupFormMachine** (QGroupBox): Machine information form

##### Form Labels
- **lblNomMachine** (QLabel): "Nom de la machine:"
- **lblTypeMachine** (QLabel): "Type de machine:"
- **lblEtatMarche** (QLabel): "État marche:"
- **lblTemperature** (QLabel): "Température actuelle (°C):"
- **lblNiveauCharge** (QLabel): "Niveau de charge (%):"
- **lblEtatFonctionnement** (QLabel): "État fonctionnement:"
- **lblTypeAlerte** (QLabel): "Type d'alerte:"
- **lblNiveauCriticite** (QLabel): "Niveau de criticité:"
- **lblDateMaintenance** (QLabel): "Dernière maintenance:"

##### Input Fields
- **nom_machine** (QLineEdit): Machine name input
- **type_machine** (QComboBox): Machine type selection
- **etat_marche** (QComboBox): Operating state selection
- **temperature_actuelle** (QDoubleSpinBox): Current temperature input (0-200°C)
- **niveau_charge** (QSpinBox): Load level percentage (0-100%)
- **etat_fonctionnement** (QComboBox): Functioning state (Normal, Alerte, Panne)
- **type_alerte** (QComboBox): Alert type selection
- **niveau_criticite** (QComboBox): Criticality level (Faible, Moyen, Élevé, Critique)
- **date_derniere_maintenance** (QDateEdit): Last maintenance date picker

##### Buttons
- **btnEnregistrer** (QPushButton): "💾 Enregistrer" - Saves machine data
- **btnAnnuler** (QPushButton): "✗ Annuler" - Cancels operation

#### Tab 3: Détails Machine
- **tabDetails** (QWidget): Machine details tab

##### Group Boxes
- **groupInfosMachine** (QGroupBox): Machine information section
- **groupActions** (QGroupBox): Available actions section

##### Information Labels
- **lblInfoNom** (QLabel): "Nom:"
- **lblValeurNom** (QLabel): Machine name value display
- **lblInfoType** (QLabel): "Type:"
- **lblValeurType** (QLabel): Machine type value display
- **lblInfoEtat** (QLabel): "État marche:"
- **lblValeurEtat** (QLabel): Operating state value display
- **lblInfoTemp** (QLabel): "Température:"
- **lblValeurTemp** (QLabel): Temperature value display
- **lblInfoScore** (QLabel): "Score santé:"

##### Progress
- **progressScoreSante** (QProgressBar): Machine health score progress bar

##### Action Buttons
- **btnReinitialiser** (QPushButton): "🔄 Réinitialiser machine" - Resets machine
- **btnMiseVeille** (QPushButton): "💤 Mise en veille" - Puts machine in standby
- **btnArretForce** (QPushButton): "⛔ Arrêt forcé" - Forces machine stop

#### Tab 4: Alertes
- **tabAlertes** (QWidget): Alerts management tab

##### Group Box
- **groupFiltresAlertes** (QGroupBox): Alert filters section

##### Input Fields
- **filtre_alerte_type** (QComboBox): Alert type filter
- **filtre_alerte_criticite** (QComboBox): Criticality filter
- **date_debut** (QDateEdit): Start date filter
- **date_fin** (QDateEdit): End date filter

##### Labels
- **lblDateDebut** (QLabel): "Du:"
- **lblDateFin** (QLabel): "Au:"

##### Table
- **tableAlertes** (QTableView): Alerts list table

##### Buttons
- **btnFiltrerAlertes** (QPushButton): "🔍 Filtrer" - Filters alerts

#### Tab 5: Statistiques
- **tabStatistiques** (QWidget): Statistics tab

##### Group Box
- **groupOptionsStats** (QGroupBox): Statistics options section

##### Input Fields
- **type_statistique** (QComboBox): Statistics type selector
- **type_graphique** (QComboBox): Chart type selector (BarChart, PieChart, LineChart)

##### Labels
- **lblTypeStats** (QLabel): "Type de statistique:"
- **lblTypeGraphique** (QLabel): "Type de graphique:"

##### Display Area
- **zoneGraphique** (QWidget): Chart display area (native, 300px min height)

##### Buttons
- **btnGenererStats** (QPushButton): "📈 Générer" - Generates statistics

#### Tab 6: Maintenance
- **tabMaintenance** (QWidget): Maintenance management tab

##### Group Box
- **groupMaintenancePredictive** (QGroupBox): Predictive maintenance section

##### Table
- **tableMaintenance** (QTableView): Machines to monitor table

##### Buttons
- **btnEnregistrerMaintenance** (QPushButton): "💾 Enregistrer maintenance" - Records maintenance
- **btnPlanifierMaintenance** (QPushButton): "📅 Planifier maintenance" - Plans maintenance

#### Tab 7: Historique
- **tabHistorique** (QWidget): History tab

##### Group Box
- **groupHistoriqueOnOff** (QGroupBox): ON/OFF history section

##### Table
- **tableHistorique** (QTableView): Machine history table

##### Frame
- **frameTempsTotal** (QFrame): Total time display frame

##### Labels
- **lblTempsTotalOn** (QLabel): "⏱️ Temps total ON: 0h 0m"
- **lblTempsTotalOff** (QLabel): "⏱️ Temps total OFF: 0h 0m"

#### Tab 8: Décision IA
- **tabDecisionIA** (QWidget): AI decision tab

##### Group Box
- **groupRecommandationIA** (QGroupBox): AI recommendation section

##### Labels
- **lblRecommandationIA** (QLabel): AI recommendation display
- **lblConfianceLabel** (QLabel): "Niveau de confiance:"

##### Progress
- **progressConfianceIA** (QProgressBar): AI confidence level progress bar

##### Buttons
- **btnAppliquerDecision** (QPushButton): "✓ Appliquer décision" - Applies AI decision
- **btnIgnorerDecision** (QPushButton): "✗ Ignorer" - Ignores AI decision

### Header Section
#### Labels
- **lblLogo** (QLabel): PressIQ logo (120x80px)
- **lblAppTitle** (QLabel): "GESTION DES MACHINES"
- **lblAppSubtitle** (QLabel): "SURVEILLANCE & ALERTES"
- **lblDateTime** (QLabel): Date and time display
- **lblOnlineStatus** (QLabel): "● EN LIGNE"

#### Buttons
- **btnBackToMenu** (QPushButton): "← Retour" - Returns to menu
- **btnGestionStock** (QPushButton): "📦 Gestion De Stock" - Opens stock management
- **btnGestionProduction** (QPushButton): "🏭 Gestion De Production" - Opens production management

---

## User & Staff Interface (userstaff.ui)

### Main Window
- **userstaff** (QMainWindow): User and staff management window (1440x616)

### Container Widgets
- **centralwidget** (QWidget): Central widget
- **headerFrame** (QFrame): Header section frame
- **sectionTitleFrame** (QFrame): Section title frame
- **contentFrame** (QFrame): Main content frame
- **brandingFrame** (QFrame): Branding information frame

### Statistics Cards
- **cardAdmin** (QFrame): Administrator statistics card
- **cardOperateur** (QFrame): Operator statistics card
- **cardManager** (QFrame): Manager statistics card

### Action Bar
- **actionBarFrame** (QFrame): Action bar container

### Header Section
#### Labels
- **labelLogoImage** (QLabel): PressIQ logo (40x40px)
- **appNameLabel** (QLabel): "PRESSIQ"
- **subtitleLabel** (QLabel): "SMART OIL SYSTEMS"
- **statusLabel** (QLabel): "● EN LIGNE"
- **versionBadge** (QLabel): "v1.0"
- **sectionTitleLabel** (QLabel): "User & Staff Management"

#### Button
- **btnBack** (QPushButton): "Retour" - Returns to main menu

### Statistics Cards Labels
#### Admin Card
- **cardAdminTitle** (QLabel): "Administrateurs"
- **cardAdminValue** (QLabel): Administrator count display

#### Operator Card
- **cardOperateurTitle** (QLabel): "Opérateurs"
- **cardOperateurValue** (QLabel): Operator count display

#### Manager Card
- **cardManagerTitle** (QLabel): "Managers"
- **cardManagerValue** (QLabel): Manager count display

### Action Bar
#### Input Fields
- **searchField** (QLineEdit): Search by name or email field
- **roleFilter** (QComboBox): Role filter (Tous les rôles, Admin, Opérateur, Manager)

#### Buttons
- **btnAjouter** (QPushButton): "+ Ajouter" - Adds new user
- **btnExporter** (QPushButton): "Exporter" - Exports user data

### Main Content
#### Table
- **userTable** (QTableWidget): Users table with columns:
  - Nom (Name)
  - Identifiant (ID)
  - Rôle (Role)
  - Statut (Status)
  - Logs
  - Actions

---

## Client Management Interface (gestionclients.ui)

### Main Window
- **GestionClients** (QMainWindow): Client management window (1441x616)

### Container Widgets
- **centralwidget** (QWidget): Central widget
- **headerWidget** (QWidget): Header section
- **dateTimeWidget** (QWidget): Date/time display widget
- **widget_actionButtons** (QWidget): Action buttons container

### Tab Widget
- **tabClient** (QTabWidget): Main client management tabs

#### Tab 1: Gestion Client
- **tab_gestionClient** (QWidget): Client management tab

##### Group Boxes
- **groupBox_formulaire** (QGroupBox): Client information form
- **groupBox_tableau** (QGroupBox): Clients list table section

##### Form Labels
- **label_id** (QLabel): "ID Client:"
- **label_nom** (QLabel): "Nom:"
- **label_prenom** (QLabel): "Prénom:"
- **label_tel** (QLabel): "Téléphone:"
- **label_adresse** (QLabel): "Adresse:"
- **label_email** (QLabel): "Email:"
- **label_type** (QLabel): "Type Client:"
- **label_total** (QLabel): "Total Olives (kg):"
- **label_date** (QLabel): "Date Création:"
- **label_statut_label** (QLabel): "Statut:"
- **label_statut** (QLabel): Status value display

##### Input Fields
- **lineEdit_id** (QLineEdit): Client ID display (read-only)
- **lineEdit_nom** (QLineEdit): Client last name input
- **lineEdit_prenom** (QLineEdit): Client first name input
- **lineEdit_tel** (QLineEdit): Phone number input
- **lineEdit_adresse** (QLineEdit): Address input
- **lineEdit_email** (QLineEdit): Email input
- **comboBox_type** (QComboBox): Client type selector (Particulier, Société)
- **lineEdit_total** (QLineEdit): Total olives quantity input
- **dateEdit_creation** (QDateEdit): Creation date picker

##### Buttons
- **btnAjouter** (QPushButton): "Ajouter" - Adds new client
- **btnModifier** (QPushButton): "Modifier" - Modifies client data
- **btnSupprimer** (QPushButton): "Supprimer" - Deletes client

##### Table
- **tableView_client** (QTableWidget): Clients list table

#### Tab 2: Recherche & Tri
- **tab_rechercheTri** (QWidget): Search and sort tab

##### Group Boxes
- **groupBox_recherche** (QGroupBox): Client search section
- **groupBox_tri** (QGroupBox): Sort options section
- **groupBox_export** (QGroupBox): Export options section
- **groupBox_resultats** (QGroupBox): Search results section

##### Search Section
###### Labels
- **label_critere** (QLabel): "Rechercher par:"
- **label_valeur** (QLabel): "Texte à rechercher:"

###### Input Fields
- **comboBox_critere** (QComboBox): Search criteria (Nom, Prénom, Téléphone, Email, Type de client)
- **lineEdit_recherche** (QLineEdit): Search text input

###### Button
- **pushButton_rechercher** (QPushButton): "Rechercher" - Executes search

##### Sort Section
###### Label
- **label_trier** (QLabel): "Trier par:"

###### Input Fields
- **comboBox_tri** (QComboBox): Sort field selector

###### Button
- **pushButton_trier** (QPushButton): "Trier" - Applies sorting

##### Export Section
###### Buttons
- **pushButton_pdf** (QPushButton): "Export PDF" - Exports to PDF
- **pushButton_excel** (QPushButton): "Export Excel" - Exports to Excel
- **pushButton_word** (QPushButton): "Export Word" - Exports to Word
- **pushButton_txt** (QPushButton): "Export TXT" - Exports to text file

##### Results Table
- **tableView_resultats** (QTableWidget): Search results table

#### Tab 3: Statistiques
- **tab_statistiques** (QWidget): Statistics tab

##### Group Boxes
- **groupBox_statistiques** (QGroupBox): Statistics generation section
- **groupBox_graph** (QGroupBox): Charts section

##### Labels
- **label_stat_type** (QLabel): "Type de Statistique:"
- **label_chart_placeholder** (QLabel): "Graphique sera affiché ici" - Chart placeholder

##### Input Fields
- **comboBox_stat** (QComboBox): Statistics type selector

##### Button
- **pushButton_stat** (QPushButton): "Générer" - Generates statistics

#### Tab 4: Fidélité & Rapport
- **tab_fideliteRapport** (QWidget): Loyalty and report tab

##### Group Boxes
- **groupBox_classification** (QGroupBox): Automatic classification section
- **groupBox_rapport** (QGroupBox): Client report section

##### Classification Labels
- **label_score_title** (QLabel): "Score Fidélité:"
- **label_score_value** (QLabel): Loyalty score value display
- **label_statut_titre** (QLabel): "Statut Fidélité:"
- **label_statut_value** (QLabel): Loyalty status value display
- **label_importance_titre** (QLabel): "Importance:"
- **label_importance_value** (QLabel): Importance value display

##### Report Section
- **textEdit_rapport** (QTextEdit): Report content display (read-only)

##### Buttons
- **pushButton_rapport** (QPushButton): "Générer Rapport" - Generates report
- **pushButton_exportRapport** (QPushButton): "Export PDF" - Exports report to PDF

### Header Section
#### Labels
- **labelLogoImage** (QLabel): PressIQ logo (45x45px)
- **label_titre_principal** (QLabel): "GESTION DES CLIENTS"
- **label_titre_sous** (QLabel): "ACCOMPAGNEMENT CLIENTS"
- **label_en_ligne** (QLabel): "● EN LIGNE"
- **calendarIconLabel** (QLabel): Calendar day number display
- **dateTimeLabel** (QLabel): Full date and time display

#### Button
- **pushButton_retour** (QPushButton): "← Retour Menu" - Returns to main menu

---

## Stock Management Interface (stock.ui)

### Main Window
- **Stock** (QMainWindow): Stock management window (1441x616)

### Container Widgets
- **centralwidget** (QWidget): Central widget
- **headerWidget** (QWidget): Header section
- **footerWidget** (QWidget): Footer section

### Tab Widget
- **tabWidgetStock** (QTabWidget): Main stock management tabs

#### Tab 1: Gestion Stock
- **tabGestionStock** (QWidget): Stock management tab

##### Group Boxes
- **groupInfoStock** (QGroupBox): Stock information section
- **groupTableStocks** (QGroupBox): Stock list table section

##### Form Labels
- **labelIdStock** (QLabel): "ID Stock :"
- **labelTypeHuile** (QLabel): "Type d'huile :"
- **labelQuantite** (QLabel): "Quantité actuelle (L) :"
- **labelSeuilAlerte** (QLabel): "Seuil d'alerte :"
- **labelDateMaj** (QLabel): "Date dernière mise à jour :"
- **labelEmplacement** (QLabel): "Emplacement de stockage :"
- **labelEtatStock** (QLabel): "État du stock :"

##### Input Fields
- **lineEditIdStock** (QLineEdit): Stock ID input
- **comboTypeHuile** (QComboBox): Oil type selector (Extra vierge, Vierge, Lampante)
- **spinQuantite** (QDoubleSpinBox): Current quantity input (L), range 0-100000
- **spinSeuilAlerte** (QSpinBox): Alert threshold input (L), range 0-10000
- **dateEditMaj** (QDateEdit): Last update date picker
- **lineEditEmplacement** (QLineEdit): Storage location input
- **comboEtatStock** (QComboBox): Stock state selector (En stock, Stock faible, Rupture)

##### Buttons
- **btnAjouter** (QPushButton): "➕ Ajouter" - Adds new stock entry
- **btnModifier** (QPushButton): "✏️ Modifier" - Modifies stock entry
- **btnSupprimer** (QPushButton): "🗑️ Supprimer" - Deletes stock entry
- **btnConsulter** (QPushButton): "👁️ Consulter" - Views stock details

##### Table
- **tableStocks** (QTableWidget): Stock list table with columns:
  - ID
  - Type d'huile
  - Quantité (L)
  - Seuil
  - Date MAJ
  - Emplacement
  - État

#### Tab 2: Tri
- **tabTri** (QWidget): Sort/filtering tab

##### Group Boxes
- **groupOptionsTri** (QGroupBox): Sort options section
- **groupTableTriee** (QGroupBox): Sorted stock table section

##### Labels
- **labelTrierSelon** (QLabel): "Trier selon :"

##### Input Fields
- **comboTriCritere** (QComboBox): Sort criteria selector

##### Button
- **btnAppliquerTri** (QPushButton): "✅ Appliquer le tri" - Applies sorting

##### Table
- **tableTriee** (QTableWidget): Sorted stock table

#### Tab 3: Recherche
- **tabRecherche** (QWidget): Search tab

##### Group Boxes
- **groupCriteres** (QGroupBox): Search criteria section
- **groupResultats** (QGroupBox): Search results section

##### Labels
- **labelRechType** (QLabel): "Type d'huile :"
- **labelRechEtat** (QLabel): "État du stock :"
- **labelRechEmpl** (QLabel): "Emplacement :"

##### Input Fields
- **comboRechercheType** (QComboBox): Oil type filter
- **comboRechercheEtat** (QComboBox): Stock state filter
- **lineRechercheEmplacement** (QLineEdit): Location search input

##### Buttons
- **btnRechercher** (QPushButton): "🔍 Rechercher" - Executes search
- **btnReinitialiser** (QPushButton): "🔄 Réinitialiser" - Resets search

##### Table
- **tableResultats** (QTableWidget): Search results table

#### Tab 4: Export
- **tabExport** (QWidget): Export tab

##### Group Boxes
- **groupFormat** (QGroupBox): Format selection section
- **groupOptions** (QGroupBox): Export options section

##### Radio Buttons
- **radioPDF** (QRadioButton): "📄 PDF" - PDF format (default checked)
- **radioExcel** (QRadioButton): "📊 Excel" - Excel format
- **radioWord** (QRadioButton): "📝 Word" - Word format
- **radioTXT** (QRadioButton): "📃 TXT" - Text format

##### Checkboxes
- **checkStats** (QCheckBox): "📊 Inclure statistiques" - Include statistics
- **checkDateTime** (QCheckBox): "📅 Inclure date & heure" - Include date/time (checked)

##### Labels
- **labelNomFichier** (QLabel): "Nom du fichier :"

##### Input Fields
- **lineNomFichier** (QLineEdit): Filename input (default: "export_stock")

##### Button
- **btnExporter** (QPushButton): "💾 Exporter le stock" - Exports stock data

#### Tab 5: Statistiques
- **tabStatistiques** (QWidget): Statistics tab

##### Group Boxes
- **groupResume** (QGroupBox): Quick summary section
- **groupOptionsStats** (QGroupBox): Statistics calculation options section
- **groupStats** (QGroupBox): Statistical summary section

##### Labels
- **labelTotalArticles** (QLabel): "Total d'articles : 0"
- **labelValeurTotale** (QLabel): "Volume total : 0 L"
- **labelStatsPar** (QLabel): "Statistiques par :"

##### Input Fields
- **comboStatsPar** (QComboBox): Statistics criteria selector

##### Button
- **btnCalculer** (QPushButton): "📈 Calculer" - Calculates statistics

##### Text Display
- **textResumeStats** (QTextEdit): Statistics summary display (read-only)

#### Tab 6: Historique
- **tabHistorique** (QWidget): History tab

##### Group Boxes
- **groupFiltrePeriode** (QGroupBox): Period filter section
- **groupHistorique** (QGroupBox): Movement history section

##### Labels
- **labelDateDebut** (QLabel): "Date début :"
- **labelDateFin** (QLabel): "Date fin :"

##### Input Fields
- **dateDebut** (QDateEdit): Start date picker
- **dateFin** (QDateEdit): End date picker

##### Button
- **btnAfficherHistorique** (QPushButton): "📋 Afficher" - Displays history

##### Table
- **tableHistorique** (QTableWidget): Movement history table with columns:
  - Date & Heure
  - Type mouvement
  - Quantité (L)
  - Produit

#### Tab 7: État Stock
- **tabEtatStock** (QWidget): Stock state tab

##### Group Boxes
- **groupEtatGlobal** (QGroupBox): Global stock state section
- **groupProduitsCritiques** (QGroupBox): Critical products section

##### Labels
- **labelEtatGlobal** (QLabel): "État : ✅ Normal" - Global state display

##### Progress
- **progressStock** (QProgressBar): Global stock level progress bar (75%)

##### List
- **listProduitsCritiques** (QListWidget): Critical products list

#### Tab 8: Temps Réel
- **tabTempsReel** (QWidget): Real-time monitoring tab

##### Group Box
- **groupTempsReel** (QGroupBox): Real-time tracking section

##### Labels
- **labelQuantiteActuelle** (QLabel): "Quantité totale actuelle :"
- **labelUnite** (QLabel): "Litres"
- **labelDerniereMaj** (QLabel): "Dernière mise à jour : --"
- **labelInfoRefresh** (QLabel): "🔄 Mise à jour automatique toutes les 5 secondes"

##### LCD Display
- **lcdVolume** (QLCDNumber): Volume display (10 digits, flat style)

#### Tab 9: Alertes
- **tabAlertes** (QWidget): Alerts tab

##### Group Boxes
- **groupConfigAlertes** (QGroupBox): Alert configuration section
- **groupStocksSousSeuil** (QGroupBox): Stocks below threshold section

##### Labels
- **labelSeuilDefini** (QLabel): Alert trigger explanation text

##### Button
- **btnVerifierAlertes** (QPushButton): "🔍 Vérifier maintenant" - Checks alerts now

##### List
- **listStocksSousSeuil** (QListWidget): Stocks below alert threshold list

#### Tab 10: Tendances
- **tabTendances** (QWidget): Trends analysis tab

##### Group Boxes
- **groupOptionsTendances** (QGroupBox): Trends analysis section
- **groupGraphique** (QGroupBox): Trends chart section
- **groupAnalyseAuto** (QGroupBox): Automatic analysis section

##### Labels
- **labelPeriode** (QLabel): "Période :"
- **labelPlaceholderChart** (QLabel): Chart placeholder text

##### Input Fields
- **comboPeriode** (QComboBox): Period selector (7 days, 30 days, 3 months, This year)

##### Button
- **btnAnalyserTendances** (QPushButton): "📊 Analyser" - Analyzes trends

##### Text Display
- **textAnalyseTendances** (QTextEdit): Trends analysis display (read-only)

#### Tab 11: Rapports
- **tabRapports** (QWidget): Reports tab

##### Group Boxes
- **groupSelectionRapport** (QGroupBox): Stock selection section
- **groupApercuRapport** (QGroupBox): Report preview section

##### Buttons
- **btnGenererRapport** (QPushButton): "📄 Générer rapport" - Generates report
- **btnExportRapportPDF** (QPushButton): "📥 Export PDF" - Exports report to PDF

##### Tables
- **tableRapportSelection** (QTableWidget): Stock selection table
- **textApercuRapport** (QTextEdit): Report preview display (read-only)

#### Tab 12: Arduino
- **tabArduino** (QWidget): Arduino integration tab

##### Group Boxes
- **groupConnexion** (QGroupBox): Arduino connection section
- **groupCapteur** (QGroupBox): Sensor data section
- **groupLogs** (QGroupBox): Logs and anomalies section

##### Labels
- **labelEtatConnexion** (QLabel): "🔴 Non connecté" - Connection status
- **labelValeurCapteur** (QLabel): "Valeur capteur : -- L" - Sensor value display

##### Button
- **btnSynchroniser** (QPushButton): "🔄 Synchroniser" - Synchronizes with Arduino

##### Text Display
- **textLogsArduino** (QTextEdit): Arduino logs display (read-only, monospace font)

### Header Section
#### Labels
- **labelLogoImage** (QLabel): PressIQ logo (50x50px)
- **lblAppTitle** (QLabel): "Gestion Des Stock"
- **lblAppSubtitle** (QLabel): "SYSTÈME DE GESTION"
- **lblDateTime** (QLabel): Date and time display
- **labelStatus** (QLabel): "EN LIGNE"

#### Button
- **btnBackToMenu** (QPushButton): "← Retour Menu" - Returns to main menu

### Footer Section
#### Labels
- **labelStatut** (QLabel): "📊 Statut : Système opérationnel"
- **labelCopyright** (QLabel): "© 2026 PRESSIQ - Tous droits réservés"

---

## Widget Types Summary

### Common Widget Classes Used Across All Interfaces

#### Input Widgets
- **QLineEdit**: Text input fields for names, search, IDs, etc.
- **QComboBox**: Dropdown selection lists for types, categories, filters
- **QSpinBox**: Integer number input with increment/decrement buttons
- **QDoubleSpinBox**: Decimal number input with precision control
- **QDateEdit**: Date picker with calendar popup
- **QTextEdit**: Multi-line text editor for notes and remarks
- **QCheckBox**: Boolean toggle for options
- **QRadioButton**: Mutually exclusive selection options

#### Display Widgets
- **QLabel**: Static or dynamic text/image display
- **QProgressBar**: Progress indication with percentage
- **QTableWidget / QTableView**: Data grid/table display
- **QTextBrowser**: Read-only HTML-formatted text display
- **QLCDNumber**: Digital numeric display (LCD style)
- **QListWidget**: Scrollable item list

#### Action Widgets
- **QPushButton**: Clickable action buttons
- **QToolBar**: Toolbar with action buttons

#### Container Widgets
- **QWidget**: Generic container widget
- **QFrame**: Styled container with borders
- **QGroupBox**: Labeled container box with border
- **QTabWidget**: Tabbed interface container
- **QMainWindow**: Top-level application window

---

## Notes

1. **Naming Conventions:**
   - French labels and UI text reflect the application's target language
   - Object names use camelCase (e.g., btnBackToMenu, lblAppTitle)
   - Prefixes indicate widget type: btn (button), lbl (label), combo (combobox), spin (spinbox)

2. **Size Constraints:**
   - Most windows are 1440x616 pixels (minimum 1200x550)
   - Logo sizes vary: 40x40, 45x45, 50x50, 70x70, 120x80 pixels
   - Many widgets have min/max size constraints for responsive design

3. **Styling:**
   - Extensive custom CSS/QSS styling throughout all interfaces
   - Consistent color scheme: Primary green (#1B4332), Gold accent (#C9A227)
   - Modern card-based design with rounded corners (border-radius: 8-12px)

4. **Icons and Emojis:**
   - Extensive use of emoji icons throughout the interface (📊, 🏭, 🫒, etc.)
   - Provides visual cues without requiring external icon files

5. **Tab Structures:**
   - Production: 6 tabs (Planning, Launch, Monitoring, Yield, Quality, Reports)
   - Machine: 8 tabs (Fleet, Add/Modify, Details, Alerts, Statistics, Maintenance, History, AI)
   - Stock: 12 tabs (Management, Sort, Search, Export, Statistics, History, State, Real-time, Alerts, Trends, Reports, Arduino)
   - Clients: 4 tabs (Management, Search/Sort, Statistics, Loyalty/Report)

---

**Document End**  
*Generated for PressIQ v2.0 - Smart Oil Press Management System*
