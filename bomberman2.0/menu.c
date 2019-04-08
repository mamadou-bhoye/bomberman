#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "menu.h"
#define  MAX 9
#define BUFFSIZE 1024


extern int num_niveaux;

extern menu_t *MENU_PRINCIPALE,*MENU_COURANT;

void alerte(char *mot);
void purge();
menu_t* createMenu(const char* text);
void addMenuAction(menu_t* m, const char* text, void(*f)());
void addSubMenu(menu_t* m, menu_t* sm);
void libere(menu_t *m);
void deleteMenu(menu_t* m);
void print_menu(menu_t* m);
void launchMenu(menu_t* m);
void retour();
void launcher();







void alerte(char *mot)
{
  char buffer[BUFFSIZE] = {'\0'};
  int n  = sprintf(buffer,"\n\t::::::::: %s :::::::::\n",mot);
  write(1,buffer,n);
}

void purge()
{
    int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF);
}


menu_t* createMenu(const char* text)
{
  int i;
  menu_t* p = NULL;
  if( (p = malloc(sizeof(menu_t)) ) == NULL) {alerte(" Erreur d'allocation ");}
  p -> nomMenu = malloc(strlen(text)+1);
  p -> nomMenu = strcpy(p -> nomMenu, text);
  p -> parent = NULL;
  p -> cpsntes = malloc(MAX * sizeof(item_t));
  for ( i= 0; i < MAX; i++){ p -> cpsntes[i] . TYPE = VIDE;}
  p -> cptItem = 0;
  return p;
}

void addMenuAction(menu_t* m, const char* text, void(*f)())
{
  if ( (m-> cptItem) < MAX -1)
  {
    m -> cpsntes[m-> cptItem] . TYPE = ACT;
    m -> cpsntes[m-> cptItem] . contenu . Action  = (action_t *)malloc(sizeof(action_t));
    m -> cpsntes[m-> cptItem] . contenu . Action -> nomAc = malloc(strlen(text)+1);
    strcpy(m -> cpsntes[m-> cptItem] . contenu . Action -> nomAc,text);
    m -> cpsntes[m-> cptItem] . contenu . Action -> fonction = f;
    m -> cptItem ++;
  }
}

void addSubMenu(menu_t* m, menu_t* sm)
{
  if ( (m -> cptItem) < MAX -1 )
  {
    m -> cpsntes [m -> cptItem] . TYPE = MENU;
    m -> cpsntes [m -> cptItem] . contenu . ssMenu = malloc(sizeof(struct menu_s*));
    m -> cpsntes[m -> cptItem] . contenu . ssMenu = sm;
    sm -> parent = m;
    m -> cptItem ++;
    return;
  }
  alerte("Nombre max de SOUS MENU atteint!");
}

void libere(menu_t *m)
{
  int i= 0;
  while(i < (m -> cptItem)){
    if( m -> cpsntes[i] . TYPE == MENU )
    {
      libere( m -> cpsntes[i] . contenu. ssMenu);
      free (m -> cpsntes[i] . contenu . ssMenu -> nomMenu);
      free (m -> cpsntes[i] . contenu. ssMenu);
    }else if (m -> cpsntes[i] . TYPE == ACT)
    {
      free (m -> cpsntes[i] . contenu . Action -> nomAc);
      free(m -> cpsntes[m-> cptItem] . contenu . Action);
    }
    i++;
  }
}

void deleteMenu(menu_t* m)
{
  if(m != NULL && m -> parent == NULL){libere(m);}
  else printf(" SOUS MENU !!!!!!!!!\n" );
}
void print_menu(menu_t* m)
{
  int i, nread,rd;
  char buf[BUFFSIZE];
  char buf1[1];
  int b=3;
  char * nom=malloc(strlen(m->nomMenu));
  strcpy(nom,m->nomMenu);
  nread =sprintf(buf, "\x1b[%d;%dH %s",b,32,nom);
  write(1,buf,nread);
  for ( i = 0; i < m -> cptItem;)
  {
    b++;
      if ( m -> cpsntes[i] . TYPE == ACT)
      {
        char *nomAtion=malloc(strlen(m -> cpsntes[i] . contenu . Action -> nomAc));
        strcpy(nomAtion,m -> cpsntes[i] . contenu . Action -> nomAc);
        rd =sprintf(buf, "\x1b[%d;%dH %d %s",b,32,i+1,nomAtion);
        write(1,buf,rd);
        i++;
      }else if (m -> cpsntes[i] . TYPE == MENU)
      {
        char *sousmenu=malloc(strlen( m -> cpsntes[i] . contenu . ssMenu-> nomMenu));
        strcpy(sousmenu, m -> cpsntes[i] . contenu . ssMenu-> nomMenu);
        rd =sprintf(buf, "\x1b[%d;%dH %d %s",b,32,i+1,sousmenu);
        write(1,buf,rd);
        i++;
      }
  }
 write(1,buf1,1);
}

void launchMenu(menu_t* m)
{
  system("clear");
  print_menu(m);
  uint saisie = 10;
  while((saisie > (m -> cptItem)) || saisie <1)
  {
     write(1,"\n\t\t\t\t-> Entrez votre choix :",28);
     if (scanf("%u", &saisie) == 0)purge();
  }
  saisie -=1;
   if (m -> cpsntes[saisie] . TYPE == ACT)
   {
     MENU_COURANT = m;
     m -> cpsntes[saisie] . contenu. Action -> fonction();
     return;
   }else if( m -> cpsntes[saisie] . TYPE == MENU)
   {
     MENU_COURANT = m -> cpsntes[saisie] . contenu. ssMenu -> parent;
     launchMenu (m -> cpsntes[saisie] . contenu. ssMenu);
   }
}

void retour()
{
  if (MENU_COURANT -> parent != NULL) launchMenu(MENU_COURANT -> parent);
  else {
    alerte("Retour non autorisé!!\n");
    launchMenu(MENU_COURANT);
  }
}


void jouer_a_deux(){}
void quitter(){}
void niv_facile(){num_niveaux = 1;}
void niv_moyen(){num_niveaux = 2;}
void niv_difficile(){num_niveaux = 3;}


void quit()
{
  alerte("MERCI À BIENTÔT!!!!");
  deleteMenu(MENU_PRINCIPALE);
}


void launcher()
{
  menu_t* parent = createMenu("  ::: BOMBER MAN ::: \n ");
  menu_t* s_menu = createMenu("CHARGER UN NIVEAU");
  addMenuAction(parent,"COMMENCER LE JEU",niv_facile);

  addMenuAction(parent,"JOUER A DEUX",quit);
  addSubMenu(parent, s_menu);
  addMenuAction(parent,"QUITTER",quit);

  addMenuAction(s_menu,"NIVEAU FACILE",niv_facile);
  addMenuAction(s_menu,"NIVEAU MOYEN",niv_moyen);
  addMenuAction(s_menu,"NIVEAU DIFFICILE",niv_difficile);
   addMenuAction(s_menu,"RETOUR",retour);

  MENU_PRINCIPALE= parent;
  MENU_COURANT = parent;

  while(!num_niveaux) launchMenu(parent);

}