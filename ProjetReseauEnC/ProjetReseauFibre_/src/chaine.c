#include "chaine.h"
#include "SVGwriter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

Chaines *lectureChaines(FILE *f){
	if(f==NULL)return NULL;
	Chaines *C=malloc(sizeof(Chaines));
	int gamma=0;
	int nbchaines=0;
	int pts=0;
	if(fscanf(f,"NbChain: %d\nGamma: %d",&nbchaines,&gamma)==2){
		C->gamma=gamma;
		C->nbChaines=nbchaines;
		C->chaines=NULL;
	}else{
		free(C);
		return NULL;
	}
	CellChaine **dernier=&(C->chaines);
	for(int i=0;i<nbchaines;i++){
		CellChaine *cellch=malloc(sizeof(CellChaine));
		int num=0;
		if(fscanf(f,"%d %d",&num, &pts)==2){
			cellch->numero=num;
			cellch->points=NULL;
			cellch->suiv=NULL;
		}else{
			free(cellch);
			return NULL;
		}
		CellPoint **dernierPoint = &(cellch->points);
		for(int j=0;j<pts;j++){
			CellPoint *Cellp=malloc(sizeof(CellPoint));
			double x=0.0;
			double y=0.0;
			if(fscanf(f,"%lf %lf",&x, &y)==2){
				Cellp->x=x;
				Cellp->y=y;
				Cellp->suiv=NULL;

				*dernierPoint = Cellp;
				dernierPoint = &(Cellp->suiv);
			}else{
				free(Cellp);
				return NULL;
			}
		}
		*dernier=cellch;
		dernier =&(cellch->suiv);

	}
	return C;
}

void affichechaine(Chaines *Ch){
    if(Ch == NULL) return;

    printf("Gamma: %d, NbChaines: %d\n", Ch->gamma, Ch->nbChaines);

    CellChaine *current = Ch->chaines;

    while(current != NULL){
        printf("Chaine numero: %d\n", current->numero);

        CellPoint *pointActuel = current->points;
        while(pointActuel != NULL){
            printf("  x: %lf, y: %lf\n", pointActuel->x, pointActuel->y);
            pointActuel = pointActuel->suiv;
        }

        current = current->suiv;
    }
}

void ecrireChaines(Chaines *C, FILE *f){
	if(C==NULL || f==NULL) return;

	fprintf(f, "NbChain: %d\nGamma: %d\n", C->nbChaines, C->gamma);

	for(CellChaine *Ch=C->chaines; Ch!=NULL; Ch=Ch->suiv){
		int nbpts=0;
		for(CellPoint *p=Ch->points; p!=NULL; p=p->suiv) nbpts++;

		fprintf(f,"%d %d\n",Ch->numero,nbpts);

		for(CellPoint *p=Ch->points; p!=NULL; p=p->suiv){
			fprintf(f, "%f %f ", p->x, p->y);
		}
		fprintf(f, "\n");
	}
}

double longueurChaine(CellChaine *c){
	double s=0.0;
	for(CellPoint *p=c->points; p!=NULL && p->suiv!=NULL; p=p->suiv){
		double dx=p->suiv->x - p->x;
		double dy=p->suiv->y - p->y;
		s += sqrt(dx*dx + dy*dy);
	}
	return s;
}

double longueurTotale(Chaines *C){
	double somme=0.0;
	for(CellChaine *ch=C->chaines; ch!=NULL; ch=ch->suiv){
		somme += longueurChaine(ch);
	}
	return somme;
}

int comptePointsTotal(Chaines *C){
	int nbpts=0;
	for(CellChaine *ch=C->chaines; ch!=NULL; ch=ch->suiv){
		for(CellPoint *p=ch->points; p!=NULL; p=p->suiv){
			nbpts++;
		}
	}
	return nbpts;
}

void afficheChainesSVG(Chaines *C, char* nomInstance){
    double maxx=0,maxy=0,minx=1e6,miny=1e6;
    CellChaine *ccour;
    CellPoint *pcour;
    double precx,precy;
    SVGwriter svg;
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        while (pcour!=NULL){
            if (maxx<pcour->x) maxx=pcour->x;
            if (maxy<pcour->y) maxy=pcour->y;
            if (minx>pcour->x) minx=pcour->x;
            if (miny>pcour->y) miny=pcour->y;
            pcour=pcour->suiv;
        }
    ccour=ccour->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        SVGlineRandColor(&svg);
        SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
        precx=pcour->x;
        precy=pcour->y;
        pcour=pcour->suiv;
        while (pcour!=NULL){
            SVGline(&svg,500*(precx-minx)/(maxx-minx),500*(precy-miny)/(maxy-miny),500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            precx=pcour->x;
            precy=pcour->y;
            pcour=pcour->suiv;
        }
        ccour=ccour->suiv;
    }
    SVGfinalize(&svg);
}

void libererChaines(Chaines *C){
	if(C==NULL) return;
	CellChaine *ch=C->chaines;
	while(ch!=NULL){
		CellPoint *p=ch->points;
		while(p!=NULL){
			CellPoint *tmpP=p->suiv;
			free(p);
			p=tmpP;
		}
		CellChaine *tmpC=ch->suiv;
		free(ch);
		ch=tmpC;
	}
	free(C);
}
