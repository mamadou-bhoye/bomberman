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

#include "menu.h"
#include "terminal.h"

#define buffsize 100
#define MAX_ARG 200
#define SIZE 1024
int field_height = 18;
int field_length = 70;
int cont=2;
int cont2=2;
int indice=0;
#define ESC 0x1B
#define DISTANCE 2
#define BUFSIZE 1024

int num_niveaux = 0;

menu_t *MENU_PRINCIPALE,*MENU_COURANT;

/*une fonction qui me permet de charger le niveau correspondant*/
int fichier_niveaux(char * file);
/*il prend en parametre un nom du fichier et charge le niveau correspondant*/

/*cette fonction me permet de remplir la structure niveaux*/
int remplirs_tructureNiveaux(char * file);
/*elle se sert essentiellement de parse_line qui me permet de mettre une ligne dans un tableau */

/*cette fonction me permet d'afficher un cadre à fin de faire une bonne apparence*/
void print_cadre();
/*:::::::::::::::::::::::::&&::::::::::::::::::*/
/*cette fontion elle permet de me decouper le char *s en parametre partot ou il trouvera de \n*/
char ** parse_line(char ** argv,char *s,int * j);
/*::::::::::::::::::::::::::::::::&::::::::::::::::::*/
/*cette fonction permet d'ouvrir le dossier mod chercher le fichier deroulement a fin de le donner a charge_deroulement*/
int fichier_deroulement(char * file);
/*::::::::::::::::&&::::::::::::::::::*/
/*cette fonction ouvre le fichier deroulement lit le fichier le decoupe et charge  le niveau correspondand à num_niveaux*/
int charge_deroulement(char *file);
/*::::::::::::::::::::&&::::::::::::::::*/
/*cette fonction me permet d'afficher le nom du jeu de mettre la couleur et les info du joueur*/
void  print_name(personnage_t a1,int x,int y,int z,int p,int c,int c2);
/*::::::::::::::&&::::::::::::::::::*/
/*elle me permet d'initier les joueurs*/
void init_personage();
/*:::::::::::&&::::::::::::::::::::*/
/*cette fonction me permet de ramasser les powerups */
int ramasser(personnage_t p);
/*::::::::::::::&&:::::::::::::::::::*/
/*cette fontion me permet de diminuer la vie du joueur*/
int diminue_vie(bombe_t b,personnage_t p);
/*::::::::::::::::&&:::::::::::::::::*/
/*cette fontion me permet de mettre la porte a deux et de  faire exploser*/
void explose2(time_t temp1,time_t temp2,bool v1,bool v2, bombe_t b,int x);
/*:::::::::::&&::::::::::::::::::::*/
/*cette fontion me permet de garder la porter à un et de faire exploser */
void explose(time_t temp1,time_t temp2,bool v1,bool v2, bombe_t b,int x);
/*::::::::::::&&:::::::::::::::::*/
/*cette fonction me permet de proposer au joueur de recommencer la partie une fois sa vie terminer */
bool recommencer();
/*::::::::::::::::::&&::::::::::::::::::::::*/

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

int fichier_niveaux(char * file)
{
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


void  print_name(personnage_t a1,int x,int y,int z,int p,int c,int c2){
    int size,nread,rd,j,i;
    char buf[BUFSIZE];
    char * name=malloc(strlen("BOMBERMAN BY ME!!!"));
    j=sprintf(buf,"\x1b[%d;%dH\x1b[%d;3m%s\x1b[0m",4,3,c,"joueur1");
    write(1,buf,j);
     j=0;
     j=sprintf(buf,"\x1b[%d;%dH\x1b[%d;3m%s\x1b[0m",4,61,c2,"joueur2");
    write(1,buf,j);
    char * vie=malloc(strlen("vie:"));
     char * bombe_restante=malloc(strlen("bombe:"));
    strcpy(name,"BOMBERMAN BY ME!!!");
    strcpy(vie,"vie:");
    strcpy(bombe_restante,"bombe:");
    size =sprintf(buf, "\x1b[%d;%dH\x1b[31;3m%s\x1b[0m",3,28,name);
     write(1,buf,size );
    nread =sprintf(buf, "\x1b[%d;%dH %s %d",x,y,vie,a1.vie);
    write(1,buf,nread );
    rd =sprintf(buf, "\x1b[%d;%dH %s %d",z,p,bombe_restante,a1.Bombe);
    write(1,buf,rd );
    i=sprintf(buf, "\x1b[%d;%dH\x1b[31;3m%s\x1b[0m",16,25,"APPUYER SUR 0 POUR QUITTER");
    write(1,buf,i);
    fflush (stdout);
    assert (size ); 
    assert (nread); 
    assert (rd);
    assert (j); 
    assert (i);  
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


void init_personage(){
    p.dessin="♠";
    p.vie=2;
    p.Bombe=1;
    p.pos_x=1;
    p.pos_y=2;
    p2.dessin="♥";
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
    if(t->maps[p.pos_x][p.pos_y].power->type==Porte_bomb)
        {
             t->maps[p.pos_x][p.pos_y].power->type=vide;
             return 2;
        }
    if(t->maps[p.pos_x][p.pos_y].power->type==Vitesse)
    {
        t->maps[p.pos_x][p.pos_y].power->type=vide;
        return 3;
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
void nettoyer(){
	char c;
	while((c=getchar())!= EOF && c!= '\n');
}
void explose2(time_t temp1,time_t temp2,bool v1,bool v2, bombe_t b,int x)
{
    char a,s,c,d,e,q,z,w,n;

     if(((temp1-temp2)==cont)&&(v1==true)){
                        if(t->maps[b.pos_x][b.pos_y].mur!='0'){
                            a=t->maps[b.pos_x][b.pos_y].mur;
                            t->maps[b.pos_x][b.pos_y].mur='^';
                        }else{
                            a='0';
                        }
                        if(t->maps[b.pos_x][b.pos_y+x].mur!='0'){
                            s=t->maps[b.pos_x][b.pos_y+x].mur;
                            if(t->maps[b.pos_x][b.pos_y+x+1].mur!='0')
                            {
                                 q=t->maps[b.pos_x][b.pos_y+x+1].mur;
                                 t->maps[b.pos_x][b.pos_y+x+1].mur='^';
                            }else{q='0';}
                            t->maps[b.pos_x][b.pos_y+x].mur='^';
                        }else{
                            s='0';
                        }
                        if(t->maps[b.pos_x][b.pos_y-x].mur!='0'){
                            e=t->maps[b.pos_x][b.pos_y-x].mur;
                            if(t->maps[b.pos_x][b.pos_y-x-1].mur!='0')
                            {
                                z=t->maps[b.pos_x][b.pos_y-x-1].mur;
                                t->maps[b.pos_x][b.pos_y-x-1].mur='^';
                            }else{z='0';}
                            t->maps[b.pos_x][b.pos_y-x].mur='^';
                        }else{
                            e='0';
                        }
                        if(t->maps[b.pos_x-x][b.pos_y].mur!='0'){
                            c=t->maps[b.pos_x-x][b.pos_y].mur;
                            if(t->maps[b.pos_x-x-1][b.pos_y].mur!='0')
                            {
                                w=t->maps[b.pos_x-x-1][b.pos_y].mur;
                                t->maps[b.pos_x-x-1][b.pos_y].mur='^';
                            }else{w='0';}
                            t->maps[b.pos_x-x][b.pos_y].mur='^';
                        }else{
                            c='0';
                        }
                        if(t->maps[b.pos_x+x][b.pos_y].mur!='0'){
                            d=t->maps[b.pos_x+x][b.pos_y].mur;
                            if(t->maps[b.pos_x+x+1][b.pos_y].mur!='0')
                            {
                                n=t->maps[b.pos_x+x+1][b.pos_y].mur;
                                t->maps[b.pos_x+x+1][b.pos_y].mur='^';
                            }else{n='0';}
                            t->maps[b.pos_x+x][b.pos_y].mur='^';
                        }else{
                            d='0';
                        }
                        v2=true;
                       
                    }
                    affiche_maps(t);
                    print_personage(p,36);
                    print_personage(p2,32);
                    if(((temp1-temp2)<cont)&&(v1==true)){print_bombe(b);}
                    print_cadre();
                    if(v2==true){
                        if(a!='0'){
                            
                            if(((a-1)!='0')&&(a!=' ')){
                                t->maps[b.pos_x][b.pos_y].mur=(a-1);
                            }else if (a==' ')
                            {
                               if(indice==1){
                                int a=diminue_vie(b1,p);
                                if(a==1)p.vie--;
                                }
                                if(indice==2){
                                int a=diminue_vie(b2,p2);
                                if(a==1)p2.vie--;}
                            }{
                                t->maps[b.pos_x][b.pos_y].mur=' ';
                            }
                        }
                        if(s!='0'){
                            if(((s-1)!='0')&&(s!=' ')){
                                t->maps[b.pos_x][b.pos_y+x].mur=(s-1);
                            }else{
                                t->maps[b.pos_x][b.pos_y+x].mur=' ';
                            }
                            if(q!='0'){
                             if(((q-1)!='0')&&(q!=' ')){
                                t->maps[b.pos_x][b.pos_y+x+1].mur=(q-1);
                            }else{
                                t->maps[b.pos_x][b.pos_y+x+1].mur=' ';
                            }
                          }
                        }
                        if(e!='0'){
                            if(((e-1)!='0')&&(e!=' ')){
                                t->maps[b.pos_x][b.pos_y-x].mur=(e-1);
                            }
                            else {
                                t->maps[b.pos_x][b.pos_y-x].mur=' ';
                            }
                            if(z!='0'){
                            if(((z-1)!='0')&&(z!=' ')){
                                t->maps[b.pos_x][b.pos_y-x-1].mur=(z-1);
                            }
                            else {
                                t->maps[b.pos_x][b.pos_y-x-1].mur=' ';
                                }
                            }
                        }
                        
                        if(c!='0'){
                            if(((c-1)!='0')&&(c!=' ')){
                                t->maps[b.pos_x-x][b.pos_y].mur=(c-1);
                            }else{
                                t->maps[b.pos_x-x][b.pos_y].mur=' ';
                            }
                            if(w!='0'){
                            if(((w-1)!='0')&&(w!=' ')){
                                t->maps[b.pos_x-x-1][b.pos_y].mur=(w-1);
                            }else{
                                t->maps[b.pos_x-x-1][b.pos_y].mur=' ';
                            }
                           } 
                        }
                        if(d!='0'){
                            if(((d-1)!='0')&&(d!=' ')){
                                t->maps[b.pos_x+x][b.pos_y].mur=(d-1);
                            }else{
                                t->maps[b.pos_x+x][b.pos_y].mur=' ';
                            }
                             if(n!='0'){
                            if(((n-1)!='0')&&(n!=' ')){
                                t->maps[b.pos_x+x+1][b.pos_y].mur=(n-1);
                            }else{
                                t->maps[b.pos_x+x+1][b.pos_y].mur=' ';
                            }
                        }
                    }
                        v2=false;
                 }  
                
}

void explose(time_t temp1,time_t temp2,bool v1,bool v2, bombe_t b,int x)
{
   char a,s,c,d,e;
   for(int i=0 ;i<x ;i++)
   {
     if(((temp1-temp2)==cont)&&(v1==true)){
                        if(t->maps[b.pos_x][b.pos_y].mur!='0'){
                            a=t->maps[b.pos_x][b.pos_y].mur;
                            t->maps[b.pos_x][b.pos_y].mur='^';
                        }else{
                            a='0';
                        }
                        if(t->maps[b.pos_x][b.pos_y+x].mur!='0'){
                            s=t->maps[b.pos_x][b.pos_y+x].mur;
                            t->maps[b.pos_x][b.pos_y+x].mur='^';
                        }else{
                            s='0';
                        }
                        if(t->maps[b.pos_x][b.pos_y-x].mur!='0'){
                            e=t->maps[b.pos_x][b.pos_y-x].mur;
                            t->maps[b.pos_x][b.pos_y-x].mur='^';
                        }else{
                            e='0';
                        }
                        if(t->maps[b.pos_x-x][b.pos_y].mur!='0'){
                            c=t->maps[b.pos_x-x][b.pos_y].mur;
                            t->maps[b.pos_x-x][b.pos_y].mur='^';
                        }else{
                            c='0';
                        }
                        if(t->maps[b.pos_x+x][b.pos_y].mur!='0'){
                            d=t->maps[b.pos_x+x][b.pos_y].mur;
                            t->maps[b.pos_x+x][b.pos_y].mur='^';
                        }else{
                            d='0';
                        }
                        v2=true;
                       
                    }
                    affiche_maps(t);
                    print_personage(p,36);
                    print_personage(p2,32);
                    if(((temp1-temp2)<cont)&&(v1==true)){print_bombe(b);}
                    print_cadre();
                    if(v2==true){
                        if(a!='0'){
                            
                            if(((a-1)!='0')&&(a!=' ')){
                                t->maps[b.pos_x][b.pos_y].mur=(a-1);
                            }else if (a==' ')
                            {
                                 if(indice==1){
                                int a=diminue_vie(b1,p);
                                if(a==1)p.vie--;
                                }
                                if(indice==2){
                                int a=diminue_vie(b2,p2);
                                if(a==1)p2.vie--;}
                            }
                            {
                                t->maps[b.pos_x][b.pos_y].mur=' ';
                            }
                        }
                        if(s!='0'){
                            if(((s-1)!='0')&&(s!=' ')){
                                t->maps[b.pos_x][b.pos_y+x].mur=(s-1);
                            }else{
                                t->maps[b.pos_x][b.pos_y+x].mur=' ';
                            }
                        }
                        if(e!='0'){
                            if(((e-1)!='0')&&(e!=' ')){
                                t->maps[b.pos_x][b.pos_y-x].mur=(e-1);
                            }
                            else {
                                t->maps[b.pos_x][b.pos_y-x].mur=' ';
                            }
                        }
                        
                        if(c!='0'){
                            if(((c-1)!='0')&&(c!=' ')){
                                t->maps[b.pos_x-x][b.pos_y].mur=(c-1);
                            }else{
                                t->maps[b.pos_x-x][b.pos_y].mur=' ';
                            }
                            
                        }
                        if(d!='0'){
                            if(((d-1)!='0')&&(d!=' ')){
                                t->maps[b.pos_x+x][b.pos_y].mur=(d-1);
                            }else{
                                t->maps[b.pos_x+x][b.pos_y].mur=' ';
                            }
                        }
                        v2=false;
                    } 
                } 
   }
                
bool recommencer()
{
    int size,nread,rd,s,a;
    char buf[BUFSIZE];
    clean();
    char* g=malloc(strlen("::::game over:::::"));
    strcpy(g,"::::GAME OVER::::::");
    a=sprintf(buf, "\x1b[%d;%dH %s",3,32,g);
    write(1,buf,a);
    sleep(2);
    clean();
    print_name(p,5,2,6,2,36,32);
    print_name(p2,5,60,6,60,36,32);
    restauration (STDIN_FILENO);
    char *r=malloc(strlen("recommencer"));
    char *p=malloc(strlen("taper 1 pour oui"));
    char *n=malloc(strlen("0 sinon"));
    strcpy(r,"RECOMMENCER?");
    strcpy(p,"taper 1 pour oui");
    strcpy(n,"0 sinon");
     size =sprintf(buf, "\x1b[%d;%dH %s",4,32,r);
     write(1,buf,size);
     nread =sprintf(buf, "\x1b[%d;%dH %s",5,32,p);
     write(1,buf,nread),
      rd =sprintf(buf, "\x1b[%d;%dH %s",6,32,n);
      write(1,buf,rd),
      write(1,"\n\t\t\t\t-> Entrez votre choix :",28);
      scanf("%d",&s);
      if(s==1)
      {
        initialisation (STDIN_FILENO);
        init_personage();
        return true;
      }
      exit(0);

}
int main(int argc, char * argv[]){
	launcher();
    bool porte,porte2;
    int size =4,test; 
    char  buff [size];  
    buff[3]=0;
    initialisation (STDIN_FILENO);
    struct pollfd fds[2];
    int timeout=500;
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
    do{
        while (p.vie>0 && p2.vie>0){
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
                            indice=1;
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
                            indice=2;
                            b2.dessin="@";
                            b2.pos_x=p2.pos_x;
                            b2.pos_y=p2.pos_y; 
                        }
                        clean();
                        test=ramasser(p);
                        print_name(p,5,2,6,2,36,32);
                        print_name(p2,5,60,6,60,36,32);
                        if(test==1)p.Bombe++;
                        if(test==2)porte=true;
                        if(porte)
                        {explose2(temp,t2,verif,explotion,b1,1);}
                         else{ explose(temp,t2,verif,explotion,b1,1);}
                    } 
                }
                
            }else{
                clean();
                test=ramasser(p2);
                print_name(p,5,2,6,2,36,32);
                print_name(p2,5,60,6,60,36,32);
                if(test==1)p2.Bombe++;
                if(test==2)porte2=true;
                if(porte2)
                {explose2(temp,t3,verif2,explosion,b2,1);}
                else{explose(temp,t3,verif2,explosion,b2,1);}
               
            }
    }
    }while(recommencer());
    restauration (STDIN_FILENO);     
    return 0;
    
}
