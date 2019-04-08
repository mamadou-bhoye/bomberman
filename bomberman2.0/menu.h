
typedef struct menu_s menu_t;
typedef struct item_s item_t;
typedef struct action_s action_t;
typedef unsigned int uint;
typedef enum {MenuP = 0, Autre } Fonctions;






struct menu_s
{
  struct menu_s* parent;
  char * nomMenu;
  item_t *cpsntes;
  uint cptItem;
};

struct item_s
{
  enum {VIDE=0, ACT,MENU} TYPE;
  union
  {
    struct menu_s* ssMenu;
    action_t* Action;
  }contenu;
};

struct action_s
{
  char* nomAc;
  void (*fonction) ();
};

void launcher();
//void deleteMenu(menu_t *);