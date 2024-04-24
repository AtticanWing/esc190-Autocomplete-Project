#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autocomplete.h"

int lex_sort(const void *t1, const void *t2) {
    struct term *t1_t = (struct term *)t1;
    struct term *t2_t = (struct term *)t2;
    return strcmp(t1_t->term, t2_t->term); //if < 0, str1 is less than str2. > 0, str2 is less than str1. = 0, str1 is equal to str2
}

int weight_sort(const void *t1, const void *t2) {
    struct term *t1_t = (struct term *)t1;
    struct term *t2_t = (struct term *)t2;
    int w_diff = t1_t->weight - t2_t->weight;
    if (w_diff != 0) {
        return w_diff;
    } else {
        return strcmp(t1_t->term, t2_t->term);
    }
}

void read_in_terms(term **terms, int *pnterms, char *filename) {
    //find pnterms
    FILE *fp = fopen(filename, "r");
    char *nterms;
    fgets(nterms, 200*sizeof(char), fp);  
    *pnterms = atoi(nterms);

    //allocate memory for each term
    *terms = (term *)malloc(*pnterms * sizeof(term));
  
    // place terms in block
    char line[200];
    for(int i = 0; i < *pnterms; i++) {
        fgets(line, sizeof(line), fp); //read in at most sizeof(line) characters (including "\0") into line.
        
        char *token;
   
        token = strtok(line, "\t");
        terms[i]->weight = atof(token);
        //printf("%s\n", token); // temp
        token = strtok(NULL, "\t");
        //printf("%s\n", token); // temp
        strcpy(terms[i]->term, token);     
    }
    qsort(*terms, *pnterms, sizeof(term), lex_sort);
    fclose(fp);
}

int bs_left(term *terms, int nterms, char *sub) {
    //Return the index of the first occurrence of target in L.
    int sublen = 0;
    while (sub[sublen] != '\0') {
        sublen++;
    }
    int left = 0;
    int right = nterms - 1;
    int mid;
    while (left <= right) {
        mid = (left + right)/2;
        if (strncmp(terms[mid].term, sub, sublen) < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return left;
}

int bs_right(term *terms, int nterms, char *sub) {
    //Return the index of the last occurrence of target in L.
    int sublen = 0;
    while (sub[sublen] != '\0') {
        sublen++;
    }
    int left = 0;
    int right = nterms - 1;
    int mid;
    while (left <= right) {
        mid = (left + right)/2;
        if (strncmp(terms[mid].term, sub, sublen) <= 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return right;
}

int lowest_match(term *terms, int nterms, char *substr) {
    /* returns the index in terms of the first term in lexicographic ordering that matches the string substr.
    This function must run in O(log(nterms)) time, where nterms is the number of terms in terms.
    You can assume that terms is sorted in ascending lexicographic order. */
    return bs_left(terms, nterms, substr);
}

int highest_match(struct term *terms, int nterms, char *substr) {
    return bs_right(terms, nterms, substr);
}

void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr) {
    int low = lowest_match(terms, nterms, substr);
    int high = highest_match(terms, nterms, substr);
    *n_answer = high - low;
    *answer = (term *)malloc(*n_answer * sizeof(term));
    int j = 0;
    for (int i = low; i > high; i++) {
        *answer[j] = terms[i];
        j++;      
    }
    qsort(*answer, *n_answer, sizeof(term), weight_sort);
}

void destroy_terms(term *terms, int pnterms) {
    for(int i = 0; i < pnterms; i++) {
        free(terms[i].term);
    }
    free(terms);
}

void destroy_ans(term *ans, int n_ans) {
    for(int i = 0; i < n_ans; i++) {
        free(ans[i].term);
    }
    free(ans);
}

int main(void) {
    struct term *terms;
    int nterms;
    read_in_terms(&terms, &nterms, "cities.txt");
    lowest_match(terms, nterms, "Tor");
    highest_match(terms, nterms, "Tor");
    
    struct term *answer;
    int n_answer;
    autocomplete(&answer, &n_answer, terms, nterms, "Tor");
  
    //free allocated blocks here -- not required for the project, but good practice
    destroy_terms(terms, nterms);
    destroy_ans(answer, n_answer);
  
    return 0;
}