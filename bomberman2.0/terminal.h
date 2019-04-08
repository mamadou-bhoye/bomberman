
struct termios sauvegarde;

struct deroulement {
    char * nomDuMod;
    char * lvl;
};

typedef struct deroulement * deroulement_t;

struct powerups{
    union{
        char dessin;
    }element;
    enum{
        Bombe,Vitesse,Porte_bomb,vide
    }type; 
};

typedef struct powerups * powerups_t;

struct maps{
    char mur ;
    powerups_t power; 
};
typedef struct maps * maps_t;

struct niveaux{
    int hauteur;
    int largeur;
    maps_t * maps;
};

typedef struct niveaux * niveaux_t;

struct personnage{
    char * dessin;
    int vie;
    int Bombe;  
    int pos_x;
    int pos_y;  
};

struct bombe{
    char * dessin;
    int pos_x;
    int pos_y;
    
};
typedef struct  bombe bombe_t;
typedef struct personnage personnage_t;

struct game{
    personnage_t p1;
    personnage_t p2;
};
typedef struct game game_t;



int initialisation (int fd);
void restauration (int fd);
void  print_personage(personnage_t p ,int val);
void  print_bombe(bombe_t b);
int affiche_maps(niveaux_t t);




