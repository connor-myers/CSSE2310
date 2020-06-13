#ifndef PATH_H
#define PATH_H

typedef enum {
    MO_SITE = 0, // The Mo site
    V1_SITE = 1, // The V1 site
    V2_SITE = 2, // The v2 site
    DO_SITE = 3, // The Do site;
    RI_SITE = 4, // the Ri site;
    BAR_SITE = 5 // The barrier site, i.e. "::"
} SiteType;

typedef struct {
    SiteType type; // the type of site this site is
    int siteNumber; // the number site this is in the path
    int* playerIds; // playerIds in order of the players at this site
    int capacity; // the capacity of this site
    int numPlayers; // the number of players at this site
} Site;

typedef struct {
    int size; // the number of sites in this path
    Site* sites;
} Path;

typedef struct {
    int site; // the site the player is in
    int siteIndex; // where in the site the player is
} PlayerCoord;

#endif