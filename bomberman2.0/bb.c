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
#include <time.h>
#include"menu.h"

int num_niveaux = 1;
menu_t *MENU_PRINCIPALE,*MENU_COURANT;

#define buffsize 100
#define MAX_ARG 200
#define SIZE 1024
int field_height = 18;
int field_length = 70;
int decalage_x =5;
int decalage_y =15;
int cont=2;
int cont2=2;
#define ESC 0x1B
#define DISTANCE 2
#define BUFSIZE 1024


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

int charge_deroulement(char *file);
int fichier_niveaux(char * file);
int remplirs_tructureNiveaux(char * file);

niveaux_t t;
personnage_t p;
personnage_t p2;
bombe_t b1;
bombe_t b2;
time_t temp;
void print_cadre(){
    int i, check = 1;
    for (i = 1; i <= field_length + DISTANCE && check ; i++)
    {   
        check = (fprintf(stdout, "%c[%i;%iH#", ESC, 1, i) > 0);
        check = check && (fprintf(stdout, "%c[%i;%iH#", ESC, field_height + DISTANCE, i) > 0);
    }
    for (i = DISTANCE; i < field_height + DISTANCE && check; i++)
    {
        check = (fprintf(stdout, "%c[%i;%iH#", ESC, i, 1) > 0);
        check = check && (fprintf(stdout, "%c[%i;%iH#", ESC, i, field_length + DISTANCE) > 0);
    }
    fflush(stdout);
    assert(check);
}
int affiche(char ** tab ){
    int i=0;
    while(tab[i]!=NULL){
        printf("%s\n",tab[i]);
        i++;
    }
    return 0;
}
void clean () { 
    int i = 0 ; 
    for ( i = 0 ; i < 50 ; i++) { 
        write( 1 , "\n" , 2) ;
    }
}

char ** parse_line(char ** argv,char *s,int * j){
    char * pointeur;
    int i=*j;
    pointeur=strtok(s,"\n");
    while(pointeur!=NULL){
        argv[i] = malloc(sizeof(char)*(1+strlen(pointeur)));
        strcpy(argv[i], pointeur);
        i++;
        pointeur=strtok(NULL,"\n");
    }
    argv[i]=NULL;
    *j=i;
    return argv;
}

int fichier_deroulement(char * file){
    DIR *fd;
    char * nomfichie;
    struct dirent *dir;
    fd=opendir(file);
    if(fd==NULL) {
        fprintf(stderr,"Impossible de ouvrir le fichier %s\n",file);
        exit(0);
    }else{
        chdir(file);
        while((dir=readdir(fd))!=NULL){
            if(strcmp("deroulement",dir->d_name)==0){
                nomfichie=dir->d_name;
                charge_deroulement(nomfichie);
            }
        }
    }
    return 0;
}


int charge_deroulement(char *file){
    char **argv=NULL;
    int fd; 
    int i=1;
    fd=open(file,O_RDWR);
    if(fd==-1){
        fprintf(stderr,"Impossible de ouvrir le fichier %s\n",file);
        return EXIT_FAILURE;
    }else{
        if((argv = malloc(sizeof(char *) * MAX_ARG))==NULL){
            perror("malloc dans charge_deroulement");
            exit(1);
        }
        char buff[256];
        int  j=0;
        while((read(fd,buff,sizeof(buff)))>0){
            argv=parse_line(argv,buff,&j);
        }
        deroulement_t d = malloc (sizeof(struct deroulement));
        d->nomDuMod= malloc(sizeof(char)*(1+strlen(argv[0])));
        strcpy(d->nomDuMod, argv[0]);
      if (num_niveaux>0 && num_niveaux<MAX_ARG && argv[num_niveaux-1] != NULL) {
            d-> lvl=(argv[num_niveaux]);
            fichier_niveaux(d->lvl);
        }
    }
    close(fd);
    free(argv);
    return 0;
    
}

int fichier_niveaux(char * file){
    DIR *fd;
    char * nomfichie;
    struct dirent *dir;
    fd=opendir("niveaux");
    if(fd==NULL) {
        perror("opendir niveaux");
        exit(0);
    }else{
        chdir("niveaux");
        while((dir=readdir(fd))!=NULL){
            if(strcmp(file,dir->d_name)==0){
                nomfichie=dir->d_name;
                remplirs_tructureNiveaux(nomfichie);
            }
        }
    }
    return 0;
}


void  print_name(){
    int size,nread,rd;
    char buf[BUFSIZE];
    char * name=malloc(strlen("BOMBERMAN 2.0"));
    char * vie=malloc(strlen("vie:"));
     char * bombe_restante=malloc(strlen("bombe:"));
    strcpy(name,"BOMBERMAN 2.0");
    strcpy(vie,"vie:");
    strcpy(bombe_restante,"bombe:");
    size =sprintf(buf, "\x1b[%d;%dH %s",3,32,name);
     write(1,buf,size );
    nread =sprintf(buf, "\x1b[%d;%dH %s %d",5,2,vie,p.vie);
    write(1,buf,nread );
    rd =sprintf(buf, "\x1b[%d;%dH %s %d",6,2,bombe_restante,p.Bombe);
    write(1,buf,rd );
    fflush (stdout);
    assert (size ); 
     assert (nread ); 
}
void  print_text(char * chaine,int x){
    int size;
    char buf[BUFSIZE];
    size =sprintf(buf, "\x1b[%d;%dH%s",x+decalage_x,0+decalage_y,chaine);
    write(1,buf,size );
    fflush (stdout);
    assert (size ); 
}

void  print_personage(personnage_t p ,int val){
    int size;
    int i=0;
    char buf[BUFSIZE];
    size =sprintf (buf, "\x1b[%d;%dH\x1b[%dm%s\x1b[?25l\x1b[0m",p.pos_x+decalage_x, p.pos_y+decalage_y,val,p.dessin);
    write(1,buf,size );
    i++;
    fflush (stdout);
    assert (size );
}

void  print_bombe(bombe_t b){
    int size;
    int i=0;
    char buf[BUFSIZE];
    size =sprintf (buf, "\x1b[%d;%dH\x1b[31m%s\x1b[0m",b.pos_x+decalage_x, b.pos_y+decalage_y,b.dessin);
    write(1,buf,size );
    i++;
    fflush (stdout);
    assert (size );
}
int affiche_maps(niveaux_t t){
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

int remplirs_tructureNiveaux(char * file){
    char **tab=NULL;
    int fd; 
    int nread;
    int h=0;
    int l=0;
    fd=open(file,O_RDWR);
    t=malloc(sizeof(niveaux_t));
    if(t==NULL){
        perror("malloc remplirs_tructureNiveaux");
        exit(0);
    }
    if(fd==-1){
        fprintf(stderr,"Impossible de ouvrir le fichier %s\n",file);
        exit(0);
    }else{
        if((tab = malloc(sizeof(char *) * MAX_ARG))==NULL){
            perror("malloc");
            exit(1);
        }
        char buff1[5];
        nread=read(fd,buff1,sizeof(buff1));
        char * pointeur;
        pointeur=strtok(buff1," ");
        if(pointeur!=NULL){
            h=atoi(pointeur);
            pointeur=strtok(NULL," ");
            if(pointeur!=NULL){
                l=atoi(pointeur);
            }
        }
        int j=0;
        int taille;
        if((h!=0)&&(l!=0)){
            t->hauteur=h;
            t->largeur=l;
            taille=(h*l)+h;
        }else{
            perror("Impossible de recuper la hauteur et largeur");
            exit(2);
        }
        char buff[taille];
        while((nread=read(fd,buff,sizeof(buff)))>0){
            buff[nread+1]='\0';
            tab=parse_line(tab,buff,&j);
        }
        t->maps=malloc((t->hauteur)*(sizeof(maps_t)));
        int x;
        int y;
        for(x = 0; x<t->hauteur; x++){
            t->maps[x]=malloc((t->largeur)*sizeof(struct maps));
            for (y = 0; y<t->largeur; y++){
                t->maps[x][y].mur=tab[x][y]; 
            }
        }
        for(x = 0; x<t->hauteur-1; x++){
            for (y = 0; y<t->largeur; y++){
                t->maps[x][y].power=malloc(sizeof(struct powerups));
                if(tab[x+t->hauteur][y]=='@'){
                    t->maps[x][y].power->element.dessin='@';
                    t->maps[x][y].power->type=Bombe;
                }else if(tab[x+t->hauteur][y]=='+'){
                    t->maps[x][y].power->element.dessin='+';
                    t->maps[x][y].power->type=Vitesse;
                }else if(tab[x+t->hauteur][y]=='*'){
                    t->maps[x][y].power->element.dessin='*';
                    t->maps[x][y].power->type=Porte_bomb;  
                }else{
                    t->maps[x][y].power->element.dessin=' ';
                    t->maps[x][y].power->type=vide;
                }
            }
        }        
    }
    return 0; 
}

int initialisation (int fd){ 
    struct termios configuration;
    if (tcgetattr (fd, & configuration) != 0)
        return (-1);
    memcpy (& sauvegarde, & configuration, sizeof (struct termios));
    cfmakeraw (& configuration);
    configuration . c_cc [VMIN] = 0;
    configuration . c_cc [VTIME] = 0;
    if (tcsetattr (fd, TCSANOW, & configuration) != 0)
        return (-1);
    return (0);
}   

int restauration (int fd){
    tcsetattr (fd, TCSANOW, & sauvegarde);
    return (0);
}

void init_personage(){
    p.dessin="S";
    p.vie=2;
    p.Bombe=1;
    p.pos_x=1;
    p.pos_y=2;
    p2.dessin="P";
    p2.vie=2;
    p2.Bombe=1;
    p2.pos_x=1;
    p2.pos_y=42;
}
int ramasser(personnage_t p)
{
	if(t->maps[p.pos_x][p.pos_y].power->type==Bombe)
		{
			 t->maps[p.pos_x][p.pos_y].power->type=vide;
			 return 1;
		}
		return 0;
}
int diminue_vie(bombe_t b,personnage_t p)
{
	if(t->maps[b.pos_x][b.pos_y].mur==t->maps[p.pos_x][p.pos_y].mur)
	{
		return 1;
	}
	return 0;
}


int main(int argc, char * argv[]){
    int cpt=0;
    int size =4;
    //launcher();
    int test,test2; 
    char  buff [size];  
    buff[3]=0;
    initialisation (STDIN_FILENO);
    struct pollfd fds[2];
    int timeout=900;
    fds[0].fd =STDIN_FILENO; 
    fds[0].events = POLLIN;
    int ret;
    time_t t2,t3;
    init_personage();
    fichier_deroulement("modNormal");
    bool verif=false;
    bool verif2=false;
    bool explotion=false;
    bool explosion=false;
    char a,b,c,d,e;
    while (1){
        temp= time(NULL);   
        ret=poll(fds,2,timeout);
        if(ret>0){
            for(int i=0; i<1; i++){
                if(fds[i].revents & POLLIN){
                    read (STDIN_FILENO,buff,size);
                    if (buff[0] == '0'){
                        restauration (STDIN_FILENO);
                        exit(0);
                    }
                    if(strcmp(buff,"\x1b[C")==0){
                        if(t->maps[p.pos_x][p.pos_y+1].mur==' ')
                        	{if(p.pos_x!=p2.pos_x || p.pos_y+1!=p2.pos_y) 
                        		p.pos_y=p.pos_y+1;}
                    }else if(strcmp(buff,"\x1b[D")==0){
                        if(t->maps[p.pos_x][p.pos_y-1].mur==' ')
                        	{if(p.pos_x!=p2.pos_x || p.pos_y-1!=p2.pos_y)
                        		p.pos_y=p.pos_y-1;}
                    }else if(strcmp(buff,"\x1b[B")==0){
                        if(t->maps[p.pos_x+1][p.pos_y].mur==' ')
                        	{if(p.pos_x+1!=p2.pos_x || p.pos_y!=p2.pos_y)
                        		p.pos_x=p.pos_x+1;}
                    }else if(strcmp(buff,"\x1b[A")==0){
                        if(t->maps[p.pos_x-1][p.pos_y].mur==' ')
                        	{if(p.pos_x-1!=p2.pos_x || p.pos_y!=p2.pos_y)
                        		p.pos_x=p.pos_x-1;}
                    }else{
                        p.pos_x=p.pos_x;
                        p.pos_y=p.pos_y;
                    }
                    if(buff[0]=='\x20'){
                        t2= time(NULL);
                        verif=true;
                        b1.dessin="@";
                        b1.pos_x=p.pos_x;
                        b1.pos_y=p.pos_y;
                        
                    }
                    if(buff[0]=='d'){
                        if(t->maps[p2.pos_x][p2.pos_y+1].mur==' ')
                        	{if(p2.pos_x!=p.pos_x || p2.pos_y+1!=p.pos_y) 
                        		p2.pos_y=p2.pos_y+1;}
                    }else if(buff[0]=='q'){
                        if(t->maps[p2.pos_x][p2.pos_y-1].mur==' ')
                        	{if(p2.pos_x!=p.pos_x || p2.pos_y-1!=p.pos_y) 
                        		p2.pos_y=p2.pos_y-1;}
                    }else if(buff[0]=='s'){
                        if(t->maps[p2.pos_x+1][p2.pos_y].mur==' ')
                        	{if(p2.pos_x+1!=p.pos_x || p2.pos_y!=p.pos_y) 
                        		p2.pos_x=p2.pos_x+1;}
                    }else if(buff[0]=='z'){
                        if(t->maps[p2.pos_x-1][p2.pos_y].mur==' ')
                        	{if(p2.pos_x-1!=p.pos_x || p2.pos_y!=p.pos_y) 
                        		p2.pos_x=p2.pos_x-1;}
                    }else{
                        p2.pos_x=p2.pos_x;
                        p2.pos_y=p2.pos_y;
                    }
                    if (buff[0]=='b'){
                       t3= time(NULL);
                        verif2=true;
                        b2.dessin="°";
                        b2.pos_x=p2.pos_x;
                        b2.pos_y=p2.pos_y; 
                    }
                    clean();
                    test=ramasser(p);
                    if(test==1)p.Bombe++;
                    print_name();
                    if(((temp-t2)==cont)&&(verif==true)){
                        if(t->maps[b1.pos_x][b1.pos_y].mur!='0'){
                            a=t->maps[b1.pos_x][b1.pos_y].mur;
                            t->maps[b1.pos_x][b1.pos_y].mur='^';
                        }else{
                            a='0';
                        }
                        if(t->maps[b1.pos_x][b1.pos_y+1].mur!='0'){
                            b=t->maps[b1.pos_x][b1.pos_y+1].mur;
                            t->maps[b1.pos_x][b1.pos_y+1].mur='^';
                        }else{
                            b='0';
                        }
                        if(t->maps[b1.pos_x][b1.pos_y-1].mur!='0'){
                            e=t->maps[b1.pos_x][b1.pos_y-1].mur;
                            t->maps[b1.pos_x][b1.pos_y-1].mur='^';
                        }else{
                            e='0';
                        }
                        if(t->maps[b1.pos_x-1][b1.pos_y].mur!='0'){
                            c=t->maps[b1.pos_x-1][b1.pos_y].mur;
                            t->maps[b1.pos_x-1][b1.pos_y].mur='^';
                        }else{
                            c='0';
                        }
                        if(t->maps[b1.pos_x+1][b1.pos_y].mur!='0'){
                            d=t->maps[b1.pos_x+1][b1.pos_y].mur;
                            t->maps[b1.pos_x+1][b1.pos_y].mur='^';
                        }else{
                            d='0';
                        }
                        explotion=true;
                       
                    }
                    affiche_maps(t);
                    print_personage(p,36);
                    print_personage(p2,32);
                    if(((temp-t2)<cont)&&(verif==true)){print_bombe(b1);}
                    print_cadre();
                    if(explotion==true){
                        if(a!='0'){
                            
                            if(((a-1)!='0')&&(a!=' ')){
                                t->maps[b1.pos_x][b1.pos_y].mur=(a-1);
                            }else if (a==' ')
                            {
                            	test2=diminue_vie(b1,p);
                            	if(test2==1)p.vie--;
                            }{
                                t->maps[b1.pos_x][b1.pos_y].mur=' ';
                            }
                        }
                        if(b!='0'){
                            if(((b-1)!='0')&&(b!=' ')){
                                t->maps[b1.pos_x][b1.pos_y+1].mur=(b-1);
                            }else{
                                t->maps[b1.pos_x][b1.pos_y+1].mur=' ';
                            }
                        }
                        cpt++;
                        if(e!='0'){
                            if(((e-1)!='0')&&(e!=' ')){
                                t->maps[b1.pos_x][b1.pos_y-1].mur=(e-1);
                            }
                            else {
                                t->maps[b1.pos_x][b1.pos_y-1].mur=' ';
                            }
                        }
                        
                        if(c!='0'){
                            if(((c-1)!='0')&&(c!=' ')){
                                t->maps[b1.pos_x-1][b1.pos_y].mur=(c-1);
                            }else{
                                t->maps[b1.pos_x-1][b1.pos_y].mur=' ';
                            }
                            
                        }
                        if(d!='0'){
                            if(((d-1)!='0')&&(d!=' ')){
                                t->maps[b1.pos_x+1][b1.pos_y].mur=(d-1);
                            }else{
                                t->maps[b1.pos_x+1][b1.pos_y].mur=' ';
                            }
                        }
                        explotion=false;
                    }  
                
                } 
            }
        }else{
            clean();
            print_name();
            if(((temp-t3)==cont2)&&(verif2==true)){
                if(t->maps[b2.pos_x][b2.pos_y].mur!='0'){
                    a=t->maps[b2.pos_x][b2.pos_y].mur;
                    t->maps[b2.pos_x][b2.pos_y].mur='^';
                }else{
                    a='0';
                }
                if(t->maps[b2.pos_x][b2.pos_y+1].mur!='0'){
                    b=t->maps[b2.pos_x][b2.pos_y+1].mur;
                    t->maps[b2.pos_x][b2.pos_y+1].mur='^';
                }else{
                    b='0';
                }
                if(t->maps[b2.pos_x][b2.pos_y-1].mur!='0'){
                    e=t->maps[b2.pos_x][b2.pos_y-1].mur;
                    
                    t->maps[b2.pos_x][b2.pos_y-1].mur='^';
                }else{
                    e='0';
                }
                if(t->maps[b2.pos_x-1][b2.pos_y].mur!='0'){
                    c=t->maps[b2.pos_x-1][b2.pos_y].mur;
                    t->maps[b2.pos_x-1][b2.pos_y].mur='^';
                }else{
                    c='0';
                }
                if(t->maps[b2.pos_x+1][b2.pos_y].mur!='0'){
                    d=t->maps[b2.pos_x+1][b2.pos_y].mur;
                    t->maps[b2.pos_x+1][b2.pos_y].mur='^';
                }else{
                    d='0';
                }
                explosion=true;
            }
            affiche_maps(t);
            print_personage(p,36);
            print_personage(p2,32);
            if(((temp-t3)< cont2)&&(verif2==true)){print_bombe(b2);}
            print_cadre();
            if(explosion==true){
                if(a!='0'){
                    
                    if(((a-1)!='0')&&(a!=' ')){
                        t->maps[b2.pos_x][b2.pos_y].mur=(a-1);
                    }else{
                        t->maps[b2.pos_x][b2.pos_y].mur=' ';
                    }
                }
                if(b!='0'){
                    if(((b-1)!='0')&&(b!=' ')){
                        t->maps[b2.pos_x][b2.pos_y+1].mur=(b-1);
                    }else{
                        t->maps[b2.pos_x][b2.pos_y+1].mur=' ';
                    }
                }
                if(e!='0'){
                    if(((e-1)!='0')&&(e!=' ')){
                        t->maps[b2.pos_x][b2.pos_y-1].mur=(e-1);
                    }else{
                        t->maps[b2.pos_x][b2.pos_y-1].mur=' ';
                    }
                }
                        cpt++;
                if(c!='0'){
                    if(((c-1)!='0')&&(c!=' ')){
                        t->maps[b2.pos_x-1][b2.pos_y].mur=(c-1);
                    }else{
                        t->maps[b2.pos_x-1][b2.pos_y].mur=' ';
                    }
                    
                }
                if(d!='0'){
                    if(((d-1)!='0')&&(d!=' ')){
                        t->maps[b2.pos_x+1][b2.pos_y].mur=(d-1);
                    }else{
                        t->maps[b2.pos_x+1][b2.pos_y].mur=' ';
                    }
                }
                explosion=false;
            }  
           
            
        }
    }
    restauration (STDIN_FILENO);     
    return 0;
    
}
