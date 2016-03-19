#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "graph.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* taille du laby (N=hauteur, M=largeur) : */
static int N, M;
/* laby : */
static int **l;

/* taille du mur (pixels) */
//#define CARRE 2
static int CARRE=1;
/* espace entre les murs */
#define INTER 0
/* nombre de couleurs */
#define NBCOL 250

#define MIN(a,b) (((a)<(b))?(a):(b))

/* fonction qui affiche un carre de cote CARRE dans la case (i,j) */
void affichecarre(int i, int j)
{
	int k;
	for( k=0 ; k<CARRE ; ++k )
		line(j*(CARRE+INTER)+k,i*(CARRE+INTER),
		     j*(CARRE+INTER)+k,i*(CARRE+INTER)+CARRE-1);
}

void dessine()
{
	int i, j;
	for( i=0 ; i<N ; i++ )
		for( j=0 ; j<M ; j++ )
			if( ! l[i][j] )
				affichecarre( i, j );
	refresh();
}

void cheminlepluscourt( int di, int dj, int fi, int fj)
{
	int i,j,k,cont;
	newcolor(0,1,1);
	affichecarre( di, dj );refresh();
	newcolor(1,0,0);
	affichecarre( fi, fj );refresh();

	l[di][dj] = 2;
	cont = 1;
	for( k=3 ; cont ; k++ )
	{
		cont = 0;
		for( i=1 ; i<N-1 ; i++ )
	  		for( j=1 ; j<M-1 ; j++ )
				if( l[i][j]==1 &&
				(l[i][j-1]==k-1 || l[i][j+1]==k-1 || l[i-1][j]==k-1 || l[i+1][j]==k-1) )
				{
					cont = 1;
					/* affiche un carre de couleur sur le chemin de recherche */
					newcolor(0.5,(k%NBCOL)/(float)NBCOL, 0.5 );
/* un autre affichage */
/*					if( k%41>3 )
						newcolor((k%41)/40.,1,(k%41)/40.);
					else
						newcolor(1,0,0); */
					affichecarre( i, j );
					l[i][j] = k;
/* arrête l'inondation dès qu'on est arrivés à la fin
 * 					if( i==fi && j==fj )
					{
						cont = 0;
						break;
					}
 */
				}
		refresh();
	}
	i = fi;
	j = fj;
	k=l[i][j];
	if(k>2)
	{
		newcolor(1,.5,0);   /* affiche un carre orange sur le chemin le plus court */
		while ( i!=di || j!=dj )
		{
			affichecarre( i, j );refresh();
			if(l[i+1][j]==k-1 )	
				i++;
			else if(l[i-1][j]==k-1 )
				i--;
			else if(l[i][j-1]==k-1 )
				j--;
			else if(l[i][j+1]==k-1 )
				j++;
			else
				break;	/* bouh ! pas de chemin trouvé ! */
			k--;
		}
	}
}


/* fonction recursive qui cherche un chemin dans le labyrinthe */
/* Cette fonction met des 2 sur les cases parcourues */
/* renvoie 1 si le chemin a ete trouve, 0 sinon. */
/* paramètres : (i,j) case courante
                 (fi,fj) case cherchee */
int cheminR( int fi, int fj, int i, int j )
{
	int r, rr;	/* ces deux variables servent a choisir une direction de maniere
	               aleatoire s'il y en a plusieurs possibles */


	newcolor(0,1,0);   /* affiche un carre vert sur le chemin */
	affichecarre( i, j ); refresh();

	if( i==fi && j==fj )/* fin de la fonction : on a trouve la sortie */
		return( 1 );

	l[i][j]=2;    /* marque la case (i,j) */

	/* cherche un chemin a partir de (i,j) */
	while( (r=(l[i+1][j]==1)?1:0 + (l[i][j+1]==1)?1:0 + (l[i-1][j]==1)?1:0 + (l[i][j-1]==1)?1:0)!=0 )
	{
		rr = rand()%r;
		if( l[i+1][j]==1 )
		{	/* bas */
			if( rr-- == 0 )
			{
				if( cheminR( fi, fj, i+1, j ) )
					return( 1 );
				continue;
			}
		}
		if( l[i-1][j]==1 )
		{	/* haut */
			if( rr-- == 0 )
			{
				if( cheminR( fi, fj, i-1, j ) )
					return( 1 );
				continue;
			}
		}
		if( l[i][j+1]==1 )
		{	/* droite */
			if( rr-- == 0 )
			{
				if( cheminR( fi, fj, i, j+1 ) )
					return( 1 );
				continue;
			}
		}
		if( l[i][j-1]==1 )
		{	/* gauche */
			if( rr-- == 0 )
			{
				if( cheminR( fi, fj, i, j-1 ) )
					return( 1 );
				continue;
			}
		}
	}
	newcolor(.8,.8,.8);  /* retour d'une impasse : affiche un carre gris */
	affichecarre( i, j );

	return( 0 );
}

/* fonction qui appelle cheminR */
void chemin(  int di, int dj, int fi, int fj)
{
	int i,j;
	newcolor(0,1,1);
	affichecarre( di, dj );refresh();
	newcolor(1,0,0);
	affichecarre( fi, fj );refresh();
	cheminR( fi, fj, di, dj );

	/* remet des 1 dans les cases marquees */
	for( i=0 ; i<N ; i++ )
		for( j=0 ; j<M ; j++ )
			if( l[i][j] )
				l[i][j] = 1;
}

/*************************************************************/
int main(int argc, char **argv)
{
	int f, i, *m;

	/* charge le laby */
	if( (f = open( "laby.lab", O_RDONLY )) == -1 )
	{
		fprintf(stderr, "impossible d'ouvrir le fichier laby.lab\n" );
		exit( 1 );
	}
	read( f, &N, sizeof(int) );
	read( f, &M, sizeof(int) );

	l = malloc( N * sizeof(int *) );
	m = malloc( N*M*sizeof(int) );
	for( i=0 ; i<N ; i++ )
		l[i] = m + i*M;
	if( read( f, m, N*M*sizeof(int) ) != N*M*sizeof(int) )
	{
		close( f );
		fprintf(stderr, "erreur de lecture dans le fichier laby.lab\n" );
		exit( 1 );
	}
	close( f );

	/* on essaye d'ouvrir une fenêtre de 1600*1024 max */
	CARRE = MIN(1024./N, 1600./M);
	if( CARRE==0 )
		CARRE = 1;

	initgraph(M*(CARRE+INTER), N*(CARRE+INTER));

	dessine();

	/* cherche un chemin aléatoirement */
	if(argc==2)
	{
		srand( time(0) );
		chemin(1,1,N-2,M-2);
		waitgraph();
	}

	/* cherche le chemin le plus court */
	cheminlepluscourt(1,1,N-2,M-2);
	while( waitgraph() != XK_q )
		fprintf(stderr, "'q' pour quitter\n");	/* attend que l'utilisateur appuie 'q' */

	closegraph();

	return( 0 );
}

