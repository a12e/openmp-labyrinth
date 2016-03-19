#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* taille du labyrinthe : */
/* hauteur : */
#define N 400
/* largeur : */
#define M 600


/* à définir pour activer l'affichage */
#define AFFICHE

/* nombre de cases constructibles minimal */
#define CONSMIN 10
/* probabilité qu'une case du bord ne soit pas constructible */
#define PROBPASCONS 10

/* nombre d'ilots par defaut */
#define NBILOTS 20

#ifdef AFFICHE
#include "graph.h"

/* taille du mur (pixels) */
#define CARRE 2
/* espace entre les pixels */
#define INTER 0
/* nombre de couleurs */
#define NBCOL 120

#define REFRESH 20
#endif /* AFFICHE */

#ifdef AFFICHE
/* fonction qui affiche un carre de cote CARRE dans la case (i,j) */
void affichecarre(int i, int j)
{
	int k;
	for( k=0 ; k<CARRE ; ++k )
		line(j*(CARRE+INTER)+k,i*(CARRE+INTER),
		     j*(CARRE+INTER)+k,i*(CARRE+INTER)+CARRE-1);
}
#endif /* AFFICHE */

/* fonction estconstructible : renvoie vrai si la case (i,j) est constructible */
int estconstructible( int l[N][M], int i, int j)
{
	if( l[i][j]==0 )
		return( 0 );
	else if( (l[i-1][j]==0 && l[i][j+1] && l[i][j-1] && l[i+1][j-1] && l[i+1][j] && l[i+1][j+1] )
		|| (l[i+1][j]==0 && l[i][j+1] && l[i][j-1] && l[i-1][j-1] && l[i-1][j] && l[i-1][j+1] )
		|| (l[i][j-1]==0 && l[i+1][j] && l[i-1][j] && l[i-1][j+1] && l[i][j+1] && l[i+1][j+1] )
		|| (l[i][j+1]==0 && l[i+1][j] && l[i-1][j] && l[i-1][j-1] && l[i][j-1] && l[i+1][j-1] )
		)
		return( 1 );
	else
		return( 0 );

}


int main(int argc, char **argv)
{
	int i,j, nbilots, nbcons, r, ii, jj;
	int l[N][M];
#ifdef AFFICHE
	int ref=REFRESH;
#endif /* AFFICHE */

	if( argc==2 )
		nbilots = atoi(argv[1]);
	else
		nbilots = NBILOTS;

	srand( time(0) );

	/* initialise l : murs autour, vide a l'interieur */
	for( i=0 ; i<N ; i++ )
		for( j=0 ; j<M ; j++ )
			if( i==0 || i==N-1 ||j==0 || j==M-1 )
			{
				l[i][j] = 0; /* mur */
			}
			else
				l[i][j] = 1; /* vide */

	/* place <nbilots> ilots aleatoirement a l'interieur du laby */
	for( ; nbilots ; nbilots-- )
	{
		i = rand()%(N-4) + 2;
		j = rand()%(M-4) + 2;
		l[i][j] = 0;
	}

#ifdef AFFICHE
	initgraph(M*(CARRE+INTER), N*(CARRE+INTER));
	for( i=0 ; i<N ; i++ )
		for( j=0 ; j<M ; j++ )
			if( l[i][j]==0 )
				affichecarre(i,j);
	refresh();
#endif /* AFFICHE */



	/* initialise les cases constructibles */
	nbcons = 0;
	for( i=1 ; i<N-1 ; i++ )
		for( j=1 ; j<M-1 ; j++ )
			if( estconstructible( l, i, j ) )
			{
				l[i][j] = -1;
				nbcons++;
			}
	/* supprime quelques cases constructibles sur les bords */
	for( i=1 ; i<N-1 ; i++ )
	{
		if( l[i][1] == -1 && (rand()%PROBPASCONS) && nbcons>(CONSMIN*2) )
		{
			l[i][1] = 1;
			nbcons--;
		}
		if( l[i][M-2] == -1 && (rand()%PROBPASCONS) && nbcons>(CONSMIN*2) )
		{
			l[i][M-2] = 1;
			nbcons--;
		}
	}
  	for( j=1 ; j<M-1 ; j++ )
  	{
		if( l[1][j] == -1 && (rand()%PROBPASCONS) && nbcons>CONSMIN )
		{
			l[1][j] = 1;
			nbcons--;
		}
		if( l[N-2][j] == -1 && (rand()%PROBPASCONS) && nbcons>CONSMIN )
		{
			l[N-2][j] = 1;
			nbcons--;
		}
	}

	/* boucle principale de génération */
	while( nbcons )
	{
		r = 1 + rand() % nbcons;
		for( i=1 ; i<N-1 ; i++ )
		{
			for( j=1 ; j<M-1 ; j++ )
				if( l[i][j] == -1 )
					if( ! --r )
						break;
			if( ! r )
				break;
		}
		/* on construit en (i,j) */
		l[i][j] = 0;

#ifdef AFFICHE
		affichecarre(i,j);
		if( ! --ref )
		{
			refresh();
			ref = REFRESH;
		}
#endif /* AFFICHE */

		nbcons --;
		/* met a jour les 8 voisins */
		for( ii=i-1 ; ii<=i+1 ; ++ii )
			for( jj=j-1 ; jj<=j+1 ; ++jj )
				if( l[ii][jj]==1 && estconstructible(l, ii,jj) )
				{
					nbcons ++;
					l[ii][jj] = -1;
				}
				else if( l[ii][jj]==-1 && ! estconstructible(l, ii,jj) )
				{
					nbcons --;
					l[ii][jj] = 1;
				}
	}	/* fin while */


	/* ENREGISTRE UN FICHIER. Format : LARGEUR(int), HAUTEUR(int), tableau brut (N*M (int))*/
	int x, f;
	f = open( "laby.lab", O_WRONLY|O_CREAT, 0644 );
	x = N;
	write( f, &x, sizeof(int) );
	x = M;
	write( f, &x, sizeof(int) );
	write( f, l, N*M*sizeof(int) );
	close( f );

#ifdef AFFICHE
	refresh();
	waitgraph(); /* attend que l'utilisateur tape une touche */
	closegraph();
#endif /* AFFICHE */

	return( 0 );
}

