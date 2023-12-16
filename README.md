# *sudoSolv*

Un petit utilitaire en C++ pour Graph 90+E pour créer, éditer des grilles de Sudoku puis leur trouver une solution.

Cette première version ne contient que l'objet central, sans GUI et permet de valider les différentes méthodes de la classe.
En séquence :

* *load()* - Ouverture du fichier "\\fls0\\GRIDS\\1.txt". S'il n'existe pas le système partira avec une grille vide;
* *display()* - Affichage
* *edit()* - Edition de la grille. Un appui sur "EXE" sort du mode;
* *findObviousValues()* - Recherche des valeurs triviales (obvious values) : ie. les valeurs évidentes; Elles sont affichées en bleu
* *resolve()* - Recherche d'une solution

```c
#define GRID_FILE   u"\\\\fls0\\GRIDS\\1.txt"

dclear(C_WHITE);
	
sudoku myGame;

if (myGame.load((FONTCHARACTER)GRID_FILE)){
    myGame.display();
    
    myGame.edit();
    
    // Any obvious value(s) ?
    if (myGame.findObviousValues()){
        myGame.display();
        getkey();
    }
    
    // A solution ?
    if (myGame.resolve()){
        // yes !!!
        myGame.display();
        getkey();
    }
}
```

Le dossier `GRIDS` contient différentes grilles.
Une grille fait exactement 162 octets.