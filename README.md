# mod1

**mod1** est un projet de simulation graphique en **3D** représentant une surface sur laquelle circule de l’eau.  
Il permet de visualiser des phénomènes tels que des vagues, tsunamis ou pluies, à l’aide de calculs physiques et d’un rendu OpenGL.

---

## 🧩 Table des matières

- [Aperçu](#aperçu)
- [Fonctionnalités](#fonctionnalités)
- [Structure du projet](#structure-du-projet)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Contribuer](#contribuer)
- [Licence](#licence)

---

## 🌊 Aperçu

Le projet **mod1** simule le comportement de l’eau sur un terrain 3D, en prenant en compte la topographie et la physique des fluides de manière simplifiée.  
Il s’agit d’un projet éducatif et technique, combinant **modélisation physique**, **rendu 3D temps réel**, et **programmation bas niveau** en C/C++.

---

## ⚙️ Fonctionnalités

- Simulation d’une **surface 3D** dynamique.  
- Représentation visuelle de **l’écoulement de l’eau**.  
- Gestion de plusieurs phénomènes :
  - 🌧️ pluie
  - 🌊 vagues
  - 🌪️ tsunamis
- Rendu temps réel via **OpenGL**.
- Gestion du **maillage** et du **relief** du terrain.
- Possibilité de charger ou modifier des cartes de hauteur.

---

## 📁 Structure du projet
```
├── 2d_sim/ # Simulation 2D ou calculs physiques de base
├── data/ # Données (textures, cartes, etc.)
├── flood/ # Gestion de l’inondation et propagation d’eau
├── map/ # Cartes topographiques, maillages
├── srcs/ # Code source principal (C/C++, OpenGL)
├── subprojects/ # Dépendances externes éventuelles
├── vsupp/ # Fichiers utilitaires et de support
├── Makefile # Script de compilation alternatif
├── meson.build # Configuration du build Meson
└── README.md # Ce fichier
```

---

## 🧰 Installation

### Prérequis

Avant de compiler, assure-toi d’avoir installé :

- Un compilateur C++ compatible (C++17 minimum)
- [Meson](https://mesonbuild.com/) et [Ninja](https://ninja-build.org/)
- OpenGL et les bibliothèques associées (GLFW, GLEW, etc.)
- (Optionnel) Python 3 pour certains utilitaires

### Compilation avec Makefile et Meson

```bash
# Cloner le dépôt
git clone https://github.com/agtdbx/mod1.git
cd mod1

# Compiler le projet
make

# Aller au binaire
cd release
```
--

## 🚀 Utilisation

Après compilation, l’exécutable principal (souvent nommé mod1) se trouve dans le dossier builddir ou bin.

Exemple d’exécution :
```
./mod1 <path/to/map.mod1>
```
--

## 👨‍💻 Auteur

Développé par agtdbx et LekaUila
