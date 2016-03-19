#ifndef GRAPH_H
#define GRAPH_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* ouvre une fenêtre de taille larg*haut */
void initgraph(int larg, int haut);

/* ferme la fenêtre */
void closegraph(void);

/* force l'affichage */
void flushgraph(void);

/* change la couleur du tracé: R,V,B \in [0.0, 1.0] */
unsigned long newcolor(float rouge, float vert, float bleu);
void setcolor(unsigned long);

/* efface la fenêtre */
void cleargraph(void);

/* trace un point */
void putpixel(int x, int y);

/* trace un segment */
void line(int x1, int y1, int x2, int y2);

/* trace un rectangle */
void rectangle(int x1, int y1, int x2, int y2);

/* rafraichit */
void refresh(void);

/* bloque l'exécution jusqu'à ce que l'utilisateur
   appuie sur une touche */
/* voir /usr/include/X11/keysymdef.h pour les valeurs de retour */
KeySym waitgraph(void);

/* idem non bloquant */
Bool *check_keys(void);


#endif /* ! GRAPH_H */


