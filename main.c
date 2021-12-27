#include <time.h>
#include <stdio.h>
#include <unistd.h>

struct cache
{
    int id, size, cls;
};

void display_cache(struct cache cache){
    printf("Cache L\%d : \%d bits de cache, lignes de \%d\n", cache.id, cache.size, cache.cls);
}

//En Ko
#define MAX_TAILLE_DATA_KO 20000

int tab[(MAX_TAILLE_DATA_KO * 1024) / sizeof(int)]; // tableau des données accédées
int i,j, taille_data, nbdonnees;                                         // compteur pour repeter une expérience
                              // qui correspond à la taille totale des données accédées en K                                   // = (taille_data * 1024) / sizeof(int)

// variables utilisées pour mesurer le temps d’exécution
struct timespec t1, t2;

// En nanosecondes
float temps_acces_moyen;

float time_diff(struct timespec *start, struct timespec *end){
    return (end->tv_sec - start->tv_sec)*1e9 + (end->tv_nsec - start->tv_nsec);
}

void main()
{
    //On récupère les différentes tailles de cache depuis sysconf
    struct cache l1 = {1, sysconf(_SC_LEVEL1_DCACHE_SIZE), sysconf(_SC_LEVEL1_DCACHE_LINESIZE)};
    display_cache(l1);
    struct cache l2 = {2, sysconf(_SC_LEVEL2_CACHE_SIZE), sysconf(_SC_LEVEL2_CACHE_LINESIZE)};
    display_cache(l2);
    struct cache l3 = {3, sysconf(_SC_LEVEL3_CACHE_SIZE), sysconf(_SC_LEVEL3_CACHE_LINESIZE)};
    display_cache(l3);

    

    //Cache Line Size
    int cls = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    // nombre de données de type int dans une ligne cache
    int pas = cls / sizeof(int);
    int x = 0;

    sleep(4);

    for (taille_data = cls; taille_data <= MAX_TAILLE_DATA_KO * 1024; taille_data = taille_data + cls)
    {
        nbdonnees = (taille_data) / sizeof(int);
        for (i = 0; i < nbdonnees; i = i + pas)
        { // boucle simple qui pré-charge les données dans le cache L1
            x = x + tab[i];
        }                    // end for i
        clock_gettime(CLOCK_REALTIME, &t1);

        for (i = 0; i < nbdonnees; i = i + pas)
        { // boucle qui accède aux lignes cache, avec un pas d’accès
            x = x + tab[i];
        }                  // end for i
        clock_gettime(CLOCK_REALTIME, &t2);

        // convertir le temps écoulé en microsecondes
        temps_acces_moyen = time_diff(&t1, &t2) / (nbdonnees / pas);
        printf("\%d  : \%f\n", taille_data, temps_acces_moyen);
    }
}