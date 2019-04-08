#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <poll.h>
#include "terminal.h"
#include<termios.h>
#include <unistd.h>
#include "menu.h"

int decalage_x =5;
int decalage_y =15;

#define BUFSIZE 1024
// extern MENU_PRINCIPALE;

int initialisation (int fd);
void restauration (int fd);
void  print_text(char * chaine,int x);
void  print_personage(personnage_t p ,int val);
void  print_bombe(bombe_t b);
int affiche_maps(niveaux_t t);






int initialisation (int fd)
{ 
    struct termios configuration;
    if (tcgetattr (fd, & configuration) != 0)
        return (-1);
    memcpy (& sauvegarde, & configuration, sizeof (struct termios));
    cfmakeraw (&configuration);
    configuration . c_cc [VMIN] = 0;
    configuration . c_cc [VTIME] = 0;
    if (tcsetattr (fd, TCSANOW, & configuration) != 0)
        return (-1);
    return (0);
}   




void restauration (int fd)
{
    tcsetattr (fd, TCSANOW, & sauvegarde);
   // deleteMenu(MENU_PRINCIPALE);
}


void  print_text(char * chaine,int x)
{
    int size;
    char buf[BUFSIZE];
    size =sprintf(buf, "\x1b[%d;%dH%s",x+decalage_x,0+decalage_y,chaine);
    write(1,buf,size );
    fflush (stdout);
    assert (size ); 
}

void  print_personage(personnage_t p ,int val)
{
    int size;
    int i=0;
    char buf[BUFSIZE];
    size =sprintf (buf, "\x1b[%d;%dH\x1b[%dm%s\x1b[?25l\x1b[0m",p.pos_x+decalage_x, p.pos_y+decalage_y,val,p.dessin);
    write(1,buf,size );
    i++;
    fflush (stdout);
    assert (size );
}

void  print_bombe(bombe_t b)
{
    int size;
    int i=0;
    char buf[BUFSIZE];
    size =sprintf (buf, "\x1b[%d;%dH\x1b[31m%s\x1b[0m",b.pos_x+decalage_x, b.pos_y+decalage_y,b.dessin);
    write(1,buf,size );
    i++;
    fflush (stdout);
    assert (size );
}

int affiche_maps(niveaux_t t)
{
    int x;
    int y;
    char *buf;
    for (x = 0; x<t->hauteur; x++){
        buf=malloc((t->largeur)*sizeof(char));
        if(buf==NULL){
            perror("buf");
            exit(0);
        }
        for (y = 0; y<t->largeur; y++){
            if((t->maps[x][y].mur==' ')&&(t->maps[x][y].power->type!=vide)&&(x<t->hauteur-1)){
                buf[y]=t->maps[x][y].power->element.dessin;
                
            }else{
                buf[y]=t->maps[x][y].mur;
            }
        }
        print_text(buf,x);
        write(1,"\n",sizeof(char));
        
    }
    
    return 0;
}

