# Suguru (C) (DM3, Informatique, MP2I Carnot 21)
#### Tristan Gierczak--Galle, MP2I

*Ce document constitue le compte-rendu complet du deuxième exercice du Devoir Maison d'informatique à rendre le vendredi 22 mai, portant sur l'implémentation et l'élaboration d'un alogrithme de résolution du jeu logique Suguru.*
*L'explication de l'implémentation du jeu suit la trame proposée dans le devoir.*

### I] Implémentation du jeu
#### Corps central
Le jeu du Suguru se jouant sur une grille n*m, avec n et m des entiers, il est naturel de représenter la grille de jeu sous la forme d'une matrice d'entier. On définit alors le type `Grid`, de la manière suivante:
```c
struct Grid {
  int n;
  int m;
  Cell **cells;
  int *zone_sizes;
  int zones_n;
};
```
On explique le rôle et l'utilité de chacun des champs de `Grid`:

- `n`, `m`: les dimensions de la grille
- `cells`: la grille en elle-même, faites de cellules de type `Cell` (qu l'on explicite ensuite)
- `zones_sizes`: la taille de chaque zone (tableau indexé par des identifiants de zone)
- `zones_n`: nombre total de zones dans la grille instanciée

Le type `Cell`, qui construit entièrement la grille, est défini comme suit:

```c
struct Cell {
  int val;
  int zone_id;
};
```
qui stocke `val` la valeur et `zone_id` l'identifiant de zone (découpage dans la grille de Suguru) de la cellule.

#### Instanciation d'une grille de jeu

Ici, je suis allé un peu plus loin en proposant un *parseur de configurations*.

Dans un fichier dont le format exact est fourni ci-après, l'utilisateur peut utiliser l'exécutable pour créer une grille de jeu, dont les formes et valeurs contenues sont choisies.

Le fichier doit **impérativement** être formaté de la manière suivante (un exemple est fourni dans `config.txt`):

```
n m
a b ... 
.
.
.
x y ...
d e ...
.
.
.
t u ...
```
- La première ligne contient les dimensions `n` et `m` de la grille voulue séparées par un espace.

- Les `n` prochaines lignes sont les différentes lignes de valeurs à insérer dans la grille. Chacune de ces lignes contient alors `m` valeurs, encore une fois séparées par des espaces.

- Les `n` dernières lignes cartographient la grille selon les identifiants de zones, à partir de `0`.
Par exemple, pour une grille `3 * 3`, les `n` lignes suivantes:
```
0 0 1
0 1 1
2 2 2
```
donne le découpage suivant (sans représenter les valeurs):
```
+---+---+---+
|       |   |
+   +---+   +
|   |       |
+---+---+---+
|           |  
+---+---+---+
```