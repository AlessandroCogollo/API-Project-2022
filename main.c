#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// --------------- BST structure -----------------

// --------------- Compare / Func ----------------

bool counter(char wordRef[], char wordP[], int pos) {
    int n = 0, c = 0, d = 0;
    for (int i = 0; i < (int) strlen(wordRef); i++) {
        printf("%d: %c, %c\n", i, wordRef[i], wordP[i]);
        if ((char) wordRef[i] == (char) wordP[pos]) {
            n++;
            if (i < pos && (char) wordP[i] == (char) wordRef[i]) {
                d++;
            }
        }
        if (strcmp(&wordRef[i], &wordP[i]) == 0) {
            c++;
        }
    }
    if (d >= (n-c)) {
        return true;
    }
    return false;
}

// ------------- Compare Function ----------------

void compare() {
    char wordA[] = "abcabcabcabcabc", wordB[] = "bbaabccbccbcabc", wordRes[15] = "";
    for (int i = 0; i < 15; i++) {
        if (wordA[i] == wordB[i]) {
            wordRes[i] = '+';
        } else {
            if (!strchr(&wordB[i], wordA[i]) || counter(wordA, wordB, i)) {
                wordRes[i] = '/';
            } else {
                wordRes[i] = '|';
            }
        }
    }
    printf("%s", wordRes);
}

int main() {
    int k, n, count;
    char cmd_new_game[] = "+nuova_partita",
         cmd_print_filtered[] = "+stampa_filtrate",
         cmd_insert_begin[] = "+inserisci_inizio",
         cmd_insert_end[] = "+inserisci_fine";
    char *new_word, *ref_word, tmp_letter;

    compare();

    // read word length
    k = (int) getchar_unlocked() - 48;

    // read admissible words
    do {
        count = 0;
        new_word = (char *) malloc(sizeof(char)*k);
        do {
            tmp_letter = (char) getchar_unlocked();
            if (tmp_letter != 10) {
                new_word[count] = tmp_letter;
                count++;
            }
        } while (tmp_letter != 10);
        if (strcmp(new_word, cmd_new_game) != 0 && strlen(new_word) == k) {
            // insert word in a "dictionary like" structure
        }
    } while (strcmp(new_word, cmd_new_game) != 0);

    // read reference word
    count = 0;
    ref_word = (char *) malloc(sizeof(char)*k);
    do {
        tmp_letter = (char) getchar_unlocked();
        if (tmp_letter != 10) {
            ref_word[count] = tmp_letter;
            count++;
        }
    } while (tmp_letter != 10);

    // read number n of words
    n = (int) getchar_unlocked() - 48;

    // read words sequence
    for (int i = 0; i < n; i++) {
        count = 0;
        new_word = (char *) malloc(sizeof(char)*k);
        do {
            tmp_letter = (char) getchar_unlocked();
            if (tmp_letter != 10) {
                new_word[count] = tmp_letter;
                count++;
            }
        } while (tmp_letter != 10);
        // TODO: Compare
    }
}