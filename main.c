#include <time.h>
#include <stdio.h>
#include <unistd.h>

//En Ko
#define MAX_TAILLE_DATA_KO 8192

struct cache
{
    int id, size, cls;
};

float time_diff(struct timespec *start, struct timespec *end){
    return (end->tv_sec - start->tv_sec)*1e9 + (end->tv_nsec - start->tv_nsec);
}

int tab[(MAX_TAILLE_DATA_KO * 1024) / sizeof(int)]; // tableau des données accédées
int i,j, taille_data, nbdonnees;
// i,j compteur pour repeter une expérience
// taille_data = qui correspond à la taille totale des données accédées en K
// nbdonnees = (taille_data * 1024) / sizeof(int)

// variables utilisées pour mesurer le temps d’exécution
struct timespec t1, t2;

// En nanosecondes
float temps_acces_moyen;
FILE *fp;


void main()
{
    fp = fopen("/tmp/output.txt", "w+");

    //On récupère les différentes tailles de cache depuis sysconf
    //Cache L1
    struct cache l1 = {1, sysconf(_SC_LEVEL1_DCACHE_SIZE), sysconf(_SC_LEVEL1_DCACHE_LINESIZE)};
    printf("Cache L\%d : \%d bits de cache, lignes de \%d\n", l1.id, l1.size, l1.cls);

    // L2
    struct cache l2 = {2, sysconf(_SC_LEVEL2_CACHE_SIZE), sysconf(_SC_LEVEL2_CACHE_LINESIZE)};
    printf("Cache L\%d : \%d bits de cache, lignes de \%d\n", l2.id, l2.size, l2.cls);

    //L3
    struct cache l3 = {3, sysconf(_SC_LEVEL3_CACHE_SIZE), sysconf(_SC_LEVEL3_CACHE_LINESIZE)};
    printf("Cache L\%d : \%d bits de cache, lignes de \%d\n", l3.id, l3.size, l3.cls);

    //Cache Line Size
    int cls = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    printf("Cache Line Size : \%d bits\n", cls);

    //Page Size
    int page_size = sysconf(_SC_PAGE_SIZE);
    printf("Memory Page Size : \%d bits\n", page_size);


    // nombre de données de type int dans une ligne cache
    int pas = cls / sizeof(int);

    
    int x = 0;

    sleep(4);

    for (taille_data = cls; taille_data <= MAX_TAILLE_DATA_KO * 1024; taille_data = taille_data + cls)
    {
        nbdonnees = (taille_data) / sizeof(int);
        // boucle simple qui pré-charge les données dans le cache L1
        for (i = 0; i < nbdonnees; i = i + pas)
        { 
            x = x + tab[i];
        }
        clock_gettime(CLOCK_REALTIME, &t1);

        // boucle qui accède aux lignes cache, avec un pas d’accès
        for (i = 0; i < nbdonnees; i = i + pas)
        { 
            x = x + tab[i];
        }
        clock_gettime(CLOCK_REALTIME, &t2);

        // convertir le temps écoulé en microsecondes
        temps_acces_moyen = time_diff(&t1, &t2) / (nbdonnees / pas);
        printf("\%f\r", taille_data / MAX_TAILLE_DATA_KO*1024);
        fprintf(fp, "\%d  : \%f\n", taille_data, temps_acces_moyen);
    }
}