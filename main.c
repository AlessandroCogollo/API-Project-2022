#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// ------------- GLOBAL VARIABLES ----------------

int k, count = 0;
bool winner_flag = false;
struct constraint* cstr = NULL;

// -------------- BST constraints ----------------

struct constraint {
    char symbol;
    bool belongs;
    int *is_present;
    int *not_present;
    int min_number;
    int exact_number;
    struct constraint *left, *right;
};

struct constraint* isPresent(struct constraint * constraintNode, char symbol) {
    if (constraintNode != NULL) {
        if (constraintNode->symbol == symbol){
            return constraintNode;
        }
        if (symbol < constraintNode->symbol) {
            return isPresent(constraintNode->left, symbol);
        } else {
            return isPresent(constraintNode->right, symbol);
        }
    } else {
        return NULL;
    }
}

struct constraint* newConstraint(char symbol, int b, int ip, int np, int mn, int en) {
    struct constraint* temp = (struct constraint*)malloc(sizeof(struct constraint));
    temp->symbol = symbol;
    temp->belongs = b;
    if (b == true) {
        temp->is_present = (int *) malloc(sizeof(int) * k);
        temp->not_present = (int *) malloc(sizeof(int) * k);
    } else {
        temp->is_present = NULL;
        temp->not_present = NULL;
    }
    temp->min_number = mn;
    temp->exact_number = en;
    temp->left = temp->right = NULL;
    return temp;
}

struct constraint* insertConstraint(struct constraint* node, char symbol, bool b, int ip, int np, int mn, int en) {
    if (node == NULL)
        return newConstraint(symbol, b, ip, np, mn, en);
    if (symbol < node->symbol)
        node->left = insertConstraint(node->left, symbol, b, ip, np, mn, en);
    else if (symbol > node->symbol)
        node->right = insertConstraint(node->right, symbol, b, ip, np, mn, en);
    return node;
}

void setConstraint(char symbol, bool belong, int ip, int np, int mp, int en) {
    struct constraint * constraintNode = isPresent(cstr, symbol);
    if (constraintNode == NULL) {
        // if symbol doesn't exist
        if (cstr == NULL) {
            cstr = insertConstraint(cstr, symbol, belong, ip, np, mp, en);
        } else {
            insertConstraint(cstr, symbol, belong, ip, np, mp, en);
        }
    }
    constraintNode = isPresent(cstr, symbol);
    if (constraintNode->belongs != false) {
        int i = 0;
        // if ip is < 0, consider only np, and viceversa
        if (ip >= 0) {
            while (constraintNode->is_present[i] != 0) {
                i++;
            }
            constraintNode->is_present[i] = ip;
        } else {
            while (constraintNode->not_present[i] != 0) {
                i++;
            }
            constraintNode->not_present[i] = np;
        }
        constraintNode->min_number = mp;
        constraintNode->exact_number = en;
    }
}

void printConstraints(struct constraint* node) {
    if (node != NULL) {
        printConstraints(node->left);
        printf("Symbol: %c, \n", node->symbol);
        printConstraints(node->right);
    }
}

// --------------- BST structure -----------------

struct node {
    char *word;
    bool compatible;
    struct node *left, *right;
};

struct node* newNode(char scannedWord[]) {
    struct node* temp = (struct node*)malloc(sizeof(struct node));
    temp->word = scannedWord;
    temp->compatible = true;
    temp->left = temp->right = NULL;
    return temp;
}

void printCompWord(struct node* root, bool print) {
    if (root != NULL && root->compatible == true) {
        printCompWord(root->left, print);
        if (print == true) { printf("%s\n", root->word); }
        count++;
        printCompWord(root->right, print);
    }
}

struct node* insert(struct node* node, char word[]) {
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
    if (d >= (n-c)) {
        return true;
    }
    return false;
}

void compare(struct node* root, char refWord[], char newWord[]) {
    char *wordRes = (char*) malloc(sizeof(char)* strlen(refWord));
    for (int i = 0; i < (int) strlen(refWord); i++) {
        // TODO: change this below
        int tmp_mp = 8;
        if (refWord[i] == newWord[i]) {
            wordRes[i] = '+';
            setConstraint(newWord[i], true, i, i, tmp_mp, tmp_mp);
        } else {
            if (!strpbrk(newWord, &refWord[i]) || counter(refWord, newWord, i)) {
                wordRes[i] = '/';
                setConstraint(newWord[i], false, i, i, tmp_mp, tmp_mp);
            } else {
                wordRes[i] = '|';
                setConstraint(newWord[i], true, -1, i, tmp_mp, tmp_mp);
            }
        }
    }
    if (!strpbrk(wordRes, "/") && !strpbrk(wordRes, "|")) {
        printf("ok\n");
        winner_flag = true;
    } else {
        printf("%s\n", wordRes);
        printCompWord(root, false);
        printf("%d\n", count);
        count = 0;
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

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

    do {
        // initialize new game
        bool filtered_flag = false;
        winner_flag = false;

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
            if (strcmp(new_word, cmd_print_filtered) == 0) {
                printCompWord(root, true);
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
                    compare(root, ref_word, new_word);
                    word_count++;
                } else {
                    printf("not_exists\n");
                }
            }
        } while (word_count < n);

        if (winner_flag == false) {
            printf("ko\n");
        }

        do {
            acquireWord(new_word);
            if (strcmp(new_word, cmd_insert_begin) == 0 && !filtered_flag) {
                filtered_flag = true;
            } else {
                if (strcmp(new_word, cmd_insert_end) == 0) {
                    filtered_flag = false;
                } else {
                    insert(root, new_word);
                }
            }
        } while (strcmp(new_word, cmd_new_game) != 0);
    } while (true);
}