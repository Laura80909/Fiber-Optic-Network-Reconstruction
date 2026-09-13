#include "Reseau.h"
#include "chaine.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr,
            "Usage : %s <fichier.cha> <methode>\n"
            "  methode = 1 : liste chainee\n"
            "            2 : table de hachage\n"
            "            3 : arbre\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }

    char *nomFichier = argv[1];
    int methode = atoi(argv[2]);

    if (methode < 1 || methode > 3) {
        fprintf(stderr, "Erreur : la methode doit etre 1, 2 ou 3.\n");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(nomFichier, "r");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }

    Chaines *C = lectureChaines(f);
    fclose(f);

    if (C == NULL) {
        fprintf(stderr, "Erreur lors de la lecture du fichier .cha\n");
        return EXIT_FAILURE;
    }

    printf("Fichier %s charge. Gamma = %d\n", nomFichier, C->gamma);

    Reseau *R = NULL;

    switch (methode) {
        case 1:
            printf("Methode choisie : Liste chainee\n");
            R = reconstitueReseauListe(C);
            break;
        case 2:
            printf("Methode choisie : Table de hachage\n");
            R = reconstitueReseauHachage(C);
            break;
        case 3:
            printf("Methode choisie : Arbre\n");
            R = reconstitueReseauArbre(C);
            break;
    }

    if (R == NULL) {
        fprintf(stderr, "Erreur lors de la reconstitution du reseau.\n");
        libererChaines(C);
        return EXIT_FAILURE;
    }

    printf("Reseau reconstitue avec succes !\n");
    printf("Nombre de noeuds : %d\n", R->nbNoeuds);
    printf("Nombre de liaisons : %d\n", nbLiaisons(R));
    printf("Nombre de commodites : %d\n", nbCommodites(R));

    FILE *fout = fopen("00014_burma.res", "w");
    if (fout != NULL) {
        ecrireReseau(R, fout);
        fclose(fout);
        printf("Fichier '00014_burma.res' genere avec succes!\n");
    }

    afficheReseauSVG(R, "test_reseau");
    printf("Fichier SVG 'test_reseau.html' genere avec succes!\n");

    libererReseau(R);
    libererChaines(C);

    return EXIT_SUCCESS;
}
