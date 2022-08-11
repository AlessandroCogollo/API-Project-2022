#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// ------------- GLOBAL VARIABLES ----------------

int k;
bool winnerflag = false;

// --------------- BST structure -----------------

struct node {
    char *word;
    struct node *left, *right;
};

struct node* newNode(char scannedWord[]) {
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
    if (node == NULL)
        return newNode(word);

    if (strcmp(word, node->word) < 0)
        node->left = insert(node->left, word);
    else if (strcmp(word, node->word) > 0)
        node->right = insert(node->right, word);

    return node;
}

struct node* search(struct node* root, char* word) {
    if (root != NULL) {
        if (strcmp(root->word, word) == 0){
            return root;
        }
        if (strcmp(word, root->word) < 0) {
            return search(root->left, word);
        } else {
            return search(root->right, word);
        }
    } else {
        return NULL;
    }
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
        winnerflag = true;
    } else {
        printf("%s", wordRes);
    }
}

void acquireWord(char * pointerToWord) {
    int letter_count = 0;
    char tmp_letter;
    do {
        tmp_letter = (char) getchar_unlocked();
        if ((int) tmp_letter != 10) {
            pointerToWord[letter_count] = tmp_letter;
            letter_count++;
        }
    } while ((int) tmp_letter != 10);

}

int main() {
    int n, word_count = 0;
    char cmd_new_game[] =       "+nuova_partita",
         cmd_print_filtered[] = "+stampa_filtrate",
         cmd_insert_begin[] =   "+inserisci_inizio",
         cmd_insert_end[] =     "+inserisci_fine";
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
    bool filtered_flag = false;
    winnerflag = false;
    do {
        new_word = (char *) malloc(sizeof(char) * k);
        acquireWord(new_word);
        if (strcmp(new_word, cmd_print_filtered) == 0) {
            inorder(root);
        } else if (strcmp(new_word, cmd_insert_begin) == 0 && !filtered_flag) {
            filtered_flag = true;
        } else if (filtered_flag == true) {
            if (strcmp(new_word, cmd_insert_end) == 0) {
                filtered_flag = false;
            } else {
                insert(root, new_word);
            }
        } else {
            if (search(root, new_word) != NULL) {
                compare(ref_word, new_word);
                word_count++;
            } else {
                printf("not_exists");
            }
        }
    } while (word_count < n);

    if (winnerflag == false) {
        printf("ko");
    }

    do {
        acquireWord(new_word);
    } while (strcmp(new_word, cmd_new_game) != 0);
}