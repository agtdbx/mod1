# 🌊 mod1 — Water Flow Simulator

<p align="center">
  <strong>Simulation graphique temps réel d'écoulements d'eau sur surface 3D</strong><br>
  Projet algorithmique de l'école 42 | C++ • OpenGL • GPU Computing
</p>

---

## 📖 Vue d'ensemble

**mod1** est un simulateur hydrodynamique en temps réel qui génère une surface 3D à partir de points de contrôle épars, puis simule différents comportements d'écoulement d'eau sur cette surface.

Le projet repose sur trois piliers techniques :

1. **Interpolation de surface** — Génération d'un maillage 3D lisse à partir de quelques points de contrôle
2. **Simulation physique** — Calcul temps réel de la physique de l'eau (montée progressive, vagues, pluie)
3. **Rendu GPU** — Visualisation OpenGL avec calculs sur carte graphique

---

## 🖼️ Screenshots

![Simulation de vague](readme_data/waves.gif)
*Vague déferlante en temps réel*

![Pluie sur cratère](readme_data/rain.gif)
*Simulation de pluie avec accumulation dans un cratère*

![Trou dans le terrain](readme_data/hole.gif)
*Simulation de l'ouverture puis la fermeture d'un trou dans le terrain*

---

## ✨ Fonctionnalités

### Partie obligatoire

- ✅ **Génération de surface** — Interpolation d'une topographie 3D à partir de fichiers `.mod1`
- ✅ **Montée d'eau progressive** — Simulation d'inondation uniforme recouvrant graduellement le terrain
- ✅ **Vague déferlante** — Vague arrivant latéralement et submergeant la surface
- ✅ **Pluie** — Simulation de précipitations avec accumulation progressive

### Bonus implémentés

- 🎨 **Interface 3D interactive** — Navigation caméra libre, contrôles intuitifs
- 🌊 **Modes supplémentaires** — Génération d'eau à un point précis, écoulement via un trou
- ⚡ **Calculs GPU** — Simulation physique entièrement calculée sur carte graphique (GLSL shaders)
- 🎥 **Contrôle de caméra avancé** — Rotation, zoom, panoramique temps réel
- 🎛️ **Paramètres ajustables** — Pause/Replay, avancement image par image, couleur de l'eau, précision du rendu

---

## 🎮 Utilisation

### Compilation

Prérequis : **C++17**, **OpenGL**, **GLFW**, **Meson**, **Ninja**

```bash
# Cloner le dépôt
git clone https://github.com/agtdbx/mod1.git
cd mod1

# Compiler avec Makefile (wrapper Meson)
make
```

### Lancement

```bash
# Depuis le répertoire release/
./mod1 path/to/map.mod1
```

### Contrôles

| Touche            | Action                                        |
|-------------------|-----------------------------------------------|
| **Souris**        | Rotation de la caméra                         |
| **Tab**           | Activer / désactiver le menu                  |
| **WASD**          | Mouvements basiques                           |
| **Espace/Lshift** | Mouvements sur l'axe Y                        |
| **Lctrl**         | Sprint                                        |
| **Flèches**       | Rotation de la caméra                         |
| **ESC**           | Quitter                                       |

---

## 📂 Structure du projet

```text
mod1/
├── 2d_sim/             # Version 2D de la simulation en python
├── data/               # Shaders et textures
├── flood/              # Algo de génération de
├── map/                # Fichiers .mod1 de test
├── srcs/               # Code source C++
├── subprojects/        # Dépendances (GLFW, GLM, etc.)
├── meson.build         # Configuration build Meson
├── Makefile            # Wrapper de build
├── README.md           # Ce fichier
└── vsupp               # Ficher de suppression valgrind
```

---

## 🧮 Aspects techniques

### Génération de surface

Le maillage 3D est généré par **interpolation polynomiale** à partir des points de contrôle fournis dans le fichier `.mod1`. L'algorithme garantit :

- Continuité de la surface (pas d'arêtes vives)
- Altitude nulle aux bords de la scène
- Résolution de grille adaptative selon la densité de points

### Simulation physique

L'eau est simulée en temps réel sur GPU via **compute shaders** OpenGL :

- Physique de l'eau basé sur des particules
- Interaction avec la topographie (accumulation dans les creux)

### Rendu graphique

Map :
- Mesh 3D rendu via OpenGL avec (vertex/fragment shader).

Eau :
- Mélange de raycasting 3D et raytracing pour détecter le terrain (fragment shader)
- Ray marching pour le rendu des particules d'eau (fragment shader)

Menu :
- Mesh 3D rendu via OpenGL avec (vertex/fragment shader).

---

## 🎯 Objectifs pédagogiques (42)

Ce projet de l'école 42 vise à maîtriser :

- ✅ Algorithmes d'interpolation et génération de surfaces
- ✅ Bases de la simulation physique (fluides, gravité)
- ✅ Programmation graphique GPU (OpenGL, shaders GLSL)
- ✅ Gestion de projet C++ structuré (build systems, dépendances)
- ✅ Optimisation temps réel (calculs parallèles, pipeline GPU)

---

## 📦 Dépendances

- **OpenGL 4.3+** (compute shaders)
- **GLFW** (gestion fenêtre et input)
- **GLM** (mathématiques 3D)
- **Meson + Ninja** (build system)
- **C++17** (compilateur compatible : GCC 9+, Clang 10+)

---

## 📜 License

Projet pédagogique école 42 — Usage éducatif uniquement.

---

## 👤 Auteur

**Auguste Deroubaix** (agtdbx) 🔗 [GitHub](https://github.com/agtdbx) • 🎓 Étudiant 42</br>
**Liam Flandrink** (LekaUila) 🔗 [GitHub](https://github.com/LekaUila) • 🎓 Étudiant 42

---

<p align="center">
  <i>Quand l'algorithmique rencontre la physique des fluides — en temps réel sur GPU</i>
</p>
