#include "samples/prototypes.h"

#include <string.h>

//a binary insert function that places key in an array whose first member is pointed to by base
//if key is present we just return a ptr to the existing member
//if key isn't present we insert it appropriately and return a ptr to it's new home <3
void *binsert(const void *key, void *base, size_t *p_nelem, size_t width,
              int (*compar)(const void *, const void *)) {
    void *end = (char *)base + (width * *p_nelem);
    //loop over the array in some fashion...
    for (size_t nremain = *p_nelem; nremain != 0; nremain >>= 1) {
        //p is a pointer to the middle of the (relevant portion of the) array
        void *p = (char *)base + (nremain >> 1) * width;
        //neg if key < p, pos if key > p, 0 if key == p
        int sign = compar(key, p);
        if (sign == 0) {
            return p;    //if key is present we can just return a pointer to the exisitng index
        }
        if (sign > 0) { /* key > p: move right */
            base = (char *)p + width;
            nremain--;
        }   /* else move left */
    }

    //increment the number of elements bc we are inserting a new element
    *p_nelem = *p_nelem + 1;
    void *dest = (char *)base + width;   //this is the location to which we will be moving base
    memmove(dest, base, (char *)end - (char *)base);
    //i know i'm close but the last arg doesn't quite do it, what do i need????
    return memcpy(base, key, width);
}
