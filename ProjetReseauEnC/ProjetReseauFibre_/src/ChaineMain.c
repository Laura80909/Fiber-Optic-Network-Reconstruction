#include <stdio.h>
#include <stdlib.h>
#include "chaine.h"

int main(int argc, char *argv[]){

    char *nomFichier = (argc >= 2) ? argv[1] : "00014_burma.cha";

    FILE *f = fopen(nomFichier, "r");
    if(f == NULL) {
        printf("Erreur d'ouverture du fichier %s\n", nomFichier);
        return 1;
    }

    Chaines *C = lectureChaines(f);
    fclose(f);

    if(C == NULL){
        printf("Erreur de lecture du fichier %s\n", nomFichier);
        return 1;
    }

    affichechaine(C);

    FILE *fi = fopen("00014_burma_modifie.cha", "w");
    if(fi != NULL){
        ecrireChaines(C, fi);
        fclose(fi);
        printf("Fichier '00014_burma_modifie.cha' regenere avec succes!\n");
    }

    printf("LONGUEUR TOTALE: %lf\n", longueurTotale(C));
    printf("NOMBRE DE POINTS TOTAL: %d\n", comptePointsTotal(C));

    afficheChainesSVG(C, "test_chaines");
    printf("Fichier SVG 'test_chaines.html' genere avec succes!\n");

    libererChaines(C);

    return 0;
}
