#include "Reseau.h"
#include "SVGwriter.h"
#include <stdio.h>
#include <stdlib.h>

/* Fonction pour rechercher ou creer un noeud */
Noeud *rechercheCreeNoeudListe(Reseau *R, double x, double y) {
    if (R == NULL) return NULL;

    CellNoeud *cour = R->noeuds;
    while (cour != NULL) {
        if (cour->nd->x == x && cour->nd->y == y) {
            return cour->nd;
        }
        cour = cour->suiv;
    }

    Noeud *nouv = malloc(sizeof(Noeud));
    if (nouv == NULL) return NULL;

    nouv->num = R->nbNoeuds + 1;
    nouv->x = x;
    nouv->y = y;
    nouv->voisins = NULL;

    CellNoeud *nouvelleCellule = malloc(sizeof(CellNoeud));
    if (nouvelleCellule == NULL) {
        free(nouv);
        return NULL;
    }

    nouvelleCellule->nd = nouv;
    nouvelleCellule->suiv = R->noeuds;
    R->noeuds = nouvelleCellule;
    R->nbNoeuds++;

    return nouv;
}

/* Fonction pour ajouter un voisin a un noeud (relation symetrique) */
void ajouterVoisin(Noeud *n1, Noeud *n2) {
    if (n1 == NULL || n2 == NULL) return;

    CellNoeud *cour = n1->voisins;
    while (cour != NULL) {
        if (cour->nd == n2) return;
        cour = cour->suiv;
    }

    CellNoeud *nouveauVoisin = malloc(sizeof(CellNoeud));
    if (nouveauVoisin == NULL) return;

    nouveauVoisin->nd = n2;
    nouveauVoisin->suiv = n1->voisins;
    n1->voisins = nouveauVoisin;

    CellNoeud *nouveauVoisin2 = malloc(sizeof(CellNoeud));
    if (nouveauVoisin2 == NULL) return;

    nouveauVoisin2->nd = n1;
    nouveauVoisin2->suiv = n2->voisins;
    n2->voisins = nouveauVoisin2;
}

/* Fonction pour creer une commodite */
void ajouterCommodite(Reseau *R, Noeud *extrA, Noeud *extrB) {
    if (R == NULL || extrA == NULL || extrB == NULL) return;

    CellCommodite *nouvelleCommodite = malloc(sizeof(CellCommodite));
    if (nouvelleCommodite == NULL) return;

    nouvelleCommodite->extrA = extrA;
    nouvelleCommodite->extrB = extrB;
    nouvelleCommodite->suiv = R->commodites;
    R->commodites = nouvelleCommodite;
}

/* Methode 1 : reconstitution par liste chainee (Exercice 2) */
Reseau *reconstitueReseauListe(Chaines *C) {
    if (C == NULL) return NULL;

    Reseau *R = malloc(sizeof(Reseau));
    if (R == NULL) return NULL;

    R->noeuds = NULL;
    R->commodites = NULL;
    R->nbNoeuds = 0;
    R->gamma = C->gamma;

    CellChaine *chaineCourante = C->chaines;

    while (chaineCourante != NULL) {
        CellPoint *pointCourant = chaineCourante->points;
        Noeud *precedent = NULL;
        Noeud *premierNoeud = NULL;

        while (pointCourant != NULL) {
            Noeud *noeudCourant = rechercheCreeNoeudListe(R, pointCourant->x, pointCourant->y);

            if (noeudCourant == NULL) {
                free(R);
                return NULL;
            }

            if (premierNoeud == NULL) {
                premierNoeud = noeudCourant;
            }

            if (precedent != NULL) {
                ajouterVoisin(precedent, noeudCourant);
            }

            precedent = noeudCourant;
            pointCourant = pointCourant->suiv;
        }

        if (premierNoeud != NULL && precedent != NULL && premierNoeud != precedent) {
            ajouterCommodite(R, premierNoeud, precedent);
        }

        chaineCourante = chaineCourante->suiv;
    }

    return R;
}

/* Methode 2 : reconstitution par table de hachage (adressage ouvert) */
Reseau *reconstitueReseauHachage(Chaines *C) {
    if (C == NULL) return NULL;

    int taille = 4001;
    Noeud **tab = malloc(taille * sizeof(Noeud*));
    if (tab == NULL) return NULL;
    for (int i = 0; i < taille; i++) tab[i] = NULL;

    Reseau *R = malloc(sizeof(Reseau));
    if (R == NULL) { free(tab); return NULL; }

    R->noeuds = NULL;
    R->commodites = NULL;
    R->nbNoeuds = 0;
    R->gamma = C->gamma;

    CellChaine *chaines = C->chaines;

    while (chaines != NULL) {
        CellPoint *points = chaines->points;
        Noeud *precedent = NULL;
        Noeud *premierNoeud = NULL;

        while (points != NULL) {
            unsigned long h = (unsigned long)(points->x * 1000) * 73856093UL
                             ^ (unsigned long)(points->y * 1000) * 19349663UL;
            int pos = (int)(h % taille);
            int i = 0;
            Noeud *noeudCour = NULL;

            while (i < taille) {
                int p = (pos + i) % taille;
                if (tab[p] == NULL) break;
                if (tab[p]->x == points->x && tab[p]->y == points->y) {
                    noeudCour = tab[p];
                    break;
                }
                i++;
            }

            if (noeudCour == NULL) {
                noeudCour = rechercheCreeNoeudListe(R, points->x, points->y);
                if (noeudCour == NULL) { free(tab); free(R); return NULL; }

                int j = 0;
                while (j < taille) {
                    int p = (pos + j) % taille;
                    if (tab[p] == NULL) { tab[p] = noeudCour; break; }
                    j++;
                }
            }

            if (premierNoeud == NULL) premierNoeud = noeudCour;
            if (precedent != NULL) ajouterVoisin(precedent, noeudCour);
            precedent = noeudCour;
            points = points->suiv;
        }

        if (premierNoeud != NULL && precedent != NULL && premierNoeud != precedent) {
            ajouterCommodite(R, premierNoeud, precedent);
        }

        chaines = chaines->suiv;
    }

    free(tab);
    return R;
}

/* Methode 3 : reconstitution par arbre (arbre quaternaire) : specification
   fournie dans la suite du sujet, non encore disponible. */
Reseau *reconstitueReseauArbre(Chaines *C) {
    (void)C;
    fprintf(stderr, "reconstitueReseauArbre : methode pas encore specifiee dans le sujet.\n");
    return NULL;
}

int nbCommodites(Reseau *R) {
    int nb = 0;
    for (CellCommodite *cour = R->commodites; cour != NULL; cour = cour->suiv) nb++;
    return nb;
}

int nbLiaisons(Reseau *R) {
    int nb = 0;
    for (CellNoeud *cour = R->noeuds; cour != NULL; cour = cour->suiv) {
        for (CellNoeud *voisin = cour->nd->voisins; voisin != NULL; voisin = voisin->suiv) {
            nb++;
        }
    }
    return nb / 2;
}

void ecrireReseau(Reseau *R, FILE *f) {
    if (R == NULL || f == NULL) return;

    fprintf(f, "NbNoeuds: %d\nNbLiaisons: %d\nNbCommodites: %d\nGamma: %d\n\n",
            R->nbNoeuds, nbLiaisons(R), nbCommodites(R), R->gamma);

    for (CellNoeud *cour = R->noeuds; cour != NULL; cour = cour->suiv) {
        fprintf(f, "v %d %f %f\n", cour->nd->num, cour->nd->x, cour->nd->y);
    }
    fprintf(f, "\n");

    for (CellNoeud *cour = R->noeuds; cour != NULL; cour = cour->suiv) {
        for (CellNoeud *voisin = cour->nd->voisins; voisin != NULL; voisin = voisin->suiv) {
            if (voisin->nd->num < cour->nd->num) {
                fprintf(f, "l %d %d\n", voisin->nd->num, cour->nd->num);
            }
        }
    }
    fprintf(f, "\n");

    for (CellCommodite *cour = R->commodites; cour != NULL; cour = cour->suiv) {
        fprintf(f, "k %d %d\n", cour->extrA->num, cour->extrB->num);
    }
}

void afficheReseauSVG(Reseau *R, char* nomInstance){
    CellNoeud *courN,*courv;
    SVGwriter svg;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;

    courN=R->noeuds;
    while (courN!=NULL){
        if (maxx<courN->nd->x) maxx=courN->nd->x;
        if (maxy<courN->nd->y) maxy=courN->nd->y;
        if (minx>courN->nd->x) minx=courN->nd->x;
        if (miny>courN->nd->y) miny=courN->nd->y;
        courN=courN->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    courN=R->noeuds;
    while (courN!=NULL){
        SVGpoint(&svg,500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
        courv=courN->nd->voisins;
        while (courv!=NULL){
            if (courv->nd->num<courN->nd->num)
                SVGline(&svg,500*(courv->nd->x-minx)/(maxx-minx),500*(courv->nd->y-miny)/(maxy-miny),500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
            courv=courv->suiv;
        }
        courN=courN->suiv;
    }
    SVGfinalize(&svg);
}

void libererReseau(Reseau *R) {
    if (R == NULL) return;

    CellNoeud *cour = R->noeuds;
    while (cour != NULL) {
        CellNoeud *tmp = cour->suiv;
        CellNoeud *v = cour->nd->voisins;
        while (v != NULL) {
            CellNoeud *tmpV = v->suiv;
            free(v);
            v = tmpV;
        }
        free(cour->nd);
        free(cour);
        cour = tmp;
    }

    CellCommodite *k = R->commodites;
    while (k != NULL) {
        CellCommodite *tmp = k->suiv;
        free(k);
        k = tmp;
    }

    free(R);
}
