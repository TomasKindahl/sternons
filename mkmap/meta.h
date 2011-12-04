#ifndef _META_H
#define _META_H

/* Arithmetic */
#define BETW(LB,X,UB) (((LB)<(X))&&((X)<(UB)))

/* Allocation */
#define ALLOC(TYPE) (TYPE *)malloc(sizeof(TYPE))
#define ALLOCN(TYPE,NUM) (TYPE *)malloc(sizeof(TYPE)*(NUM))

/* Database sorted view of an item list */
#define _VIEW(ITEM) struct _ ## ITEM ## _view_S
#define VIEW(ITEM) ITEM ## _view
#define DEFVIEW(ITEM)        \
    typedef _VIEW(ITEM) {    \
        int size;            \
        int next;            \
        ITEM **S;            \
    } VIEW(ITEM)

#endif /* _META_H */
