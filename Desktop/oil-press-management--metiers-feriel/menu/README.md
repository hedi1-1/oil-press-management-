# Smart Oil Press Management - Module Selection Menu

## Description
Interface de sélection de modules professionnelle pour l'application "Smart Oil Press Management".

## Caractéristiques

### Interface
- **Titre principal** : "Smart Oil Press Management" (grande police, gras, centré)
- **Sous-titre** : "Sélection du module" (police moyenne, centré)
- **5 boutons de modules** organisés dans un GroupBox "Modules disponibles"
- **Pied de page** : "Projet universitaire – Qt & C++"

### Modules disponibles
1. **Gestion de la production** → Ouvre l'interface smartoil
2. **Gestion des machines** → À venir
3. **Maintenance & Alertes** → À venir
4. **Recherche & Analyse** → À venir
5. **Gestion financière** → À venir

### Style
- Design moderne et professionnel
- Palette de couleurs bleue (#3498db)
- Boutons réactifs avec effets hover et pressed
- Layouts responsifs (QVBoxLayout + QGridLayout)
- Compatible Qt Designer

## Intégration avec smartoil

Le bouton "Gestion de la production" ouvre l'interface smartoil (MainWindow).
- Lorsque cliqué, le menu se cache et l'interface de production s'affiche
- Les autres boutons affichent un message indiquant que le module sera disponible prochainement

## Structure des fichiers

```
menu/
├── menu.h           - En-tête de la classe menu
├── menu.cpp         - Implémentation avec connexions et intégration
├── menu.ui          - Interface Qt Designer
├── menu.pro         - Fichier projet Qt (inclut smartoil)
└── main.cpp         - Point d'entrée de l'application
```

## Compilation

1. Ouvrir `menu.pro` dans Qt Creator
2. Configurer le projet avec Qt 6.7.3
3. Compiler et exécuter

## Technologies
- Qt 6.7.3
- Qt Widgets
- C++17
- Qt Designer (.ui)

## Notes techniques
- Utilise uniquement des layouts (pas de positionnement absolu)
- Toutes les polices et tailles sont définies dans le .ui
- Styles CSS intégrés dans le fichier .ui
- Forward declaration pour éviter les dépendances circulaires
