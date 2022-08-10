#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// ------------- GLOBAL VARIABLES ----------------

int k;

// --------------- BST structure -----------------

struct node {
    char *word;
    struct node *left, *right;
};

struct node* newNode(char * scannedWord) {
    struct node* temp = (struct node*)malloc(sizeof(struct node));
    temp->word = scannedWord;
    temp->left = temp->right = NULL;
    return temp;
}

void inorder(struct node* root)
{
    if (root != NULL) {
        inorder(root->left);
        printf("%s \n", root->word);
        inorder(root->right);
    }
}

struct node* insert(struct node* node, char word[])
{
    /* If the tree is empty, return a new node */
    if (node == NULL)
        return newNode(word);

    /* Otherwise, recur down the tree */
    if (word < node->word)
        node->left = insert(node->left, word);
    else if (word > node->word)
        node->right = insert(node->right, word);

    /* return the (unchanged) node pointer */
    return node;
}

struct node* search(struct node* root, char word[]) {
    if (root == NULL || root->word == word)
        return root;
    if (root->word < word)
        return search(root->right, word);
    return search(root->left, word);
}

// -----------------------------------------------

bool counter(char wordRef[], const char wordP[], int pos) {
    int n = 0, c = 0, d = 0;
    for (int i = 0; i < (int) strlen(wordRef); i++) {
        if (wordRef[i] == wordP[pos]) {
            n++;
            if (wordRef[i] == wordP[i]) {
                c++;
            }
        }
    }
    for (int j = 0; j < pos; j++) {
        if (wordP[j] == wordP[pos] && wordRef[j] != wordP[j]) {
            d++;
        }
    }
    // printf("n = %d, c = %d, d = %d\n", n, c, d);
    if (d >= (n-c)) {
        return true;
    }
    return false;
}

void compare(char refWord[], char newWord[]) {
    char *wordRes = (char*) malloc(sizeof(char)* strlen(refWord));
    for (int i = 0; i < (int) strlen(refWord); i++) {
        if (refWord[i] == newWord[i]) {
            wordRes[i] = '+';
        } else {
            if (!strpbrk(newWord, &refWord[i]) || counter(refWord, newWord, i)) {
                wordRes[i] = '/';
            } else {
                wordRes[i] = '|';
            }
        }
    }
    if (!strpbrk(wordRes, "/") && !strpbrk(wordRes, "|")) {
        printf("ok");
    } else {
        printf("%s", wordRes);
    }
}

void acquireWord(char * pointerToWord) {
    int letter_count = 0;
    char tmp_letter, *new_word;
    do {
        tmp_letter = (char) getchar_unlocked();
        if (tmp_letter != 10) {
            pointerToWord[letter_count] = tmp_letter;
            letter_count++;
        }
    } while (tmp_letter != 10);
}

int main() {
    int n, word_count = 0;
    char cmd_new_game[] = "+nuova_partita",
         cmd_print_filtered[] = "+stampa_filtrate",
         cmd_insert_begin[] = "+inserisci_inizio",
         cmd_insert_end[] = "+inserisci_fine";
    char *new_word, *ref_word;

    struct node* root = NULL;

    // read word length
    k = (int) getchar_unlocked() - 48;

    // read admissible words
    do {
        new_word = (char *) malloc(sizeof(char) * k);
        acquireWord(new_word);
        if (strcmp(new_word, cmd_new_game) != 0 && strlen(new_word) == k) {
            if (root == NULL) {
                root = insert(root, new_word);
            } else {
                insert(root, new_word);
            }
        }
    } while (strcmp(new_word, cmd_new_game) != 0);

    // read reference word
    ref_word = (char *) malloc(sizeof(char) * k);
    acquireWord(ref_word);

    // read number n of words
    n = (int) getchar_unlocked() - 48;
    getchar_unlocked();

    // read words sequence
    do {
        new_word = (char *) malloc(sizeof(char) * k);
        acquireWord(new_word);
        if (strcmp(new_word, cmd_print_filtered) != 0 && strcmp(new_word, cmd_insert_begin) != 0 && strcmp(new_word, cmd_insert_end) != 0) {
            compare(ref_word, new_word);
            word_count++;
        } else if (strcmp(new_word, cmd_insert_begin) == 0){
            do {
                new_word = (char *) malloc(sizeof(char) * k);
                acquireWord(new_word);
                insert(root, new_word);
            } while (strcmp(new_word, cmd_insert_end) == 0);
        } else {
            inorder(root);
        }
    } while (word_count < n);
}