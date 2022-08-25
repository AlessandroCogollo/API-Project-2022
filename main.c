#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// ------------- GLOBAL VARIABLES ----------------

// TODO: Change cstr and make it local!
int k;
bool winner_flag = false;
struct constraint* cstr = NULL;

bool counter(const char wordRef[], const char wordP[], int pos) {
    int n = 0, c = 0, d = 0;
    for (int i = k - 1; i >= 0; i--) {
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
    } else {
        return false;
    }
}

// -------------- BST constraints ----------------

struct constraint {
    char symbol;
    bool belongs;
    int *is_present;
    int *not_present;
    int min_number;
    int exact_number;
    bool incr_flag;
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

struct constraint *newConstraint(char symbol, int b) {
    struct constraint* temp = (struct constraint*)malloc(sizeof(struct constraint));
    temp->symbol = symbol;
    temp->belongs = b;
    if (b == true) {
        temp->is_present = (int *) malloc(sizeof(int) * k);
        temp->not_present = (int *) malloc(sizeof(int) * k);
        // initialize array with -1
        for (int i = 0; i < k; i++) {
            (temp->is_present)[i] = -1;
            (temp->not_present)[i] = -1;
        }
    } else {
        temp->is_present = NULL;
        temp->not_present = NULL;
    }
    temp->min_number = 0;
    temp->exact_number = -1;
    temp->incr_flag = false;
    temp->left = temp->right = NULL;
    return temp;
}

struct constraint* insertConstraint(struct constraint* node, char symbol, bool b, int ip, int np) {
    if (node == NULL)
        return newConstraint(symbol, b);
    if (symbol < node->symbol)
        node->left = insertConstraint(node->left, symbol, b, ip, np);
    else if (symbol > node->symbol)
        node->right = insertConstraint(node->right, symbol, b, ip, np);
    return node;
}

bool checkIncr(char * refWord, const char * newWord,  char symbol, struct constraint * node) {
    int counterWord = 0, counterPresent = 0;
    for (int i = 0; i < strlen(refWord); i++) {
        if (refWord[i] == symbol && newWord[i] == refWord[i]) {
            counterWord++;
        }
        if (node->is_present[i] >= 0) {
            counterPresent++;
        }
    }
    //counterPresent++;
    if (counterWord == counterPresent) {
        return true;
    }
    return false;
}

void setConstraint(char * refWord, char * newWord, char symbol, bool belong, int ip, int np) {
    struct constraint *constraintNode = isPresent(cstr, symbol);
    if (cstr == NULL) {
        cstr = insertConstraint(cstr, symbol, belong, ip, np);
    } else {
        if (constraintNode == NULL) {
            // if symbol doesn't exist
            insertConstraint(cstr, symbol, belong, ip, np);
        }
    }
    constraintNode = isPresent(cstr, symbol);
    bool incr_min_number_flag = true;
    if (constraintNode->belongs) {
        int i = 0;
        if (ip >= 0) {
            if (np < 0) {
                while (constraintNode->is_present[i] >= 0) {
                    if (constraintNode->is_present[i] == ip) {
                        incr_min_number_flag = false;
                    }
                    i++;
                }
                // TODO: check this, constraintNode incrementation
                if (incr_min_number_flag == true) {
                    constraintNode->is_present[i] = ip;
                    if (constraintNode->exact_number < 0) {
                        i = 0;
                        while (constraintNode->is_present[i] >= 0) {
                            i++;
                        }
                        constraintNode->min_number = i;
                    }
                }
            }
        } else {
            if (np < 0) {
                constraintNode->exact_number = constraintNode->min_number;
            } else {
                while (constraintNode->not_present[i] >= 0) {
                    if (constraintNode->not_present[i] == np) {
                        incr_min_number_flag = false;
                    }
                    i++;
                }
                if (incr_min_number_flag) {
                    constraintNode->not_present[i] = np;
                    if (constraintNode->exact_number < 0 && checkIncr(refWord, newWord, symbol, constraintNode)) {
                        constraintNode->min_number++;
                    }
                }
            }
        }
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

void printFiltered(struct node* root) {
    if (root != NULL) {
        printFiltered(root->left);
        if (root->compatible) {
            printf("%s\n", root->word);
        }
        printFiltered(root->right);
    }
}

bool isPresentCompatible(struct constraint* root, char symbol) {
    struct constraint * tmp_node = isPresent(root, symbol);
    if (tmp_node != NULL && tmp_node->belongs == true) {
        return true;
    } else {
        return false;
    }
}

int printCompWord(struct node* root, bool print) {
    if (root == NULL)
        return 0;
    else {
        if (root->compatible) {
            return 1 + printCompWord(root->left, print) + printCompWord(root->right, print);
        } else {
            return printCompWord(root->left, print) + printCompWord(root->right, print);
        }
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

void setAllComp(struct node* root) {
    if (root != NULL) {
        setAllComp(root->left);
        root->compatible = true;
        setAllComp(root->right);
    }
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

void setFlagFalse(struct constraint* root) {
    if (root != NULL) {
        setFlagFalse(root->left);
        root->incr_flag = false;
        setFlagFalse(root->right);
    }
}

int charCounter(char word[], char letter) {
    int temp_count = 0;
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] == letter) {
            temp_count++;
        }
    }
    return temp_count;
}

void checkComp(struct node *node, struct constraint * constraintNode) {
    if (constraintNode != NULL) {
        checkComp(node, constraintNode->left);
        if (constraintNode->belongs) {
            // if symbol is part of word
            int i = 0;
            while (constraintNode->is_present[i] >= 0) {
                if (node->word[constraintNode->is_present[i]] != constraintNode->symbol) {
                    node->compatible = false;
                }
                i++;
            }
            int j = 0;
            while (constraintNode->not_present[j] >= 0) {
                if (node->word[constraintNode->not_present[j]] == constraintNode->symbol) {
                    node->compatible = false;
                }
                j++;
            }
            int temp_count = charCounter(node->word, constraintNode->symbol);
            if (constraintNode->exact_number >= 0) {
                if (temp_count != constraintNode->exact_number) {
                    node->compatible = false;
                }
            } else {
                if (temp_count < constraintNode->min_number) {
                    node->compatible = false;
                }
            }
        } else {
            // if symbol doesn't belong to the word
            if (strchr(node->word, constraintNode->symbol) != NULL) {
                node->compatible = false;
            }
        }
        checkComp(node, constraintNode->right);
    }
}

bool banWord(struct node* root, struct constraint* constraintNode) {
    if (root != NULL) {
        banWord(root->left, constraintNode);
        checkComp(root, constraintNode);
        banWord(root->right, constraintNode);
    }
    return true;
}

// -----------------------------------------------

bool compare(char refWord[], char newWord[]) {
    char *wordRes = (char*) malloc(sizeof(char)* strlen(refWord));
    int i = k-1;
    while (i >= 0) {
        if (refWord[i] == newWord[i]) {
            wordRes[i] = '+';
            setConstraint(refWord, newWord, newWord[i], true, i, -1);
        } else {
            if (!strchr(refWord, newWord[i]) || counter(refWord, newWord, i)) {
                wordRes[i] = '/';
                if (isPresentCompatible(cstr, newWord[i])) {
                    setConstraint(refWord, newWord, newWord[i], true, -1, -1);
                } else {
                    setConstraint(refWord, newWord, newWord[i], false, i, i);
                }
            } else {
                wordRes[i] = '|';
                setConstraint(refWord, newWord, newWord[i], true, -1, i);
            }
        }
        i--;
    }
    if (!strchr(wordRes, '/') && !strchr(wordRes, '|')) {
        return false;
    } else {
        printf("%s\n", wordRes);
        return true;
    }
}

// #pragma clang diagnostic push
// #pragma ide diagnostic ignored "EndlessLoop"

int main() {
    int n, word_count, compWordCount, scanf_return;
    char cmd_new_game[] =       "+nuova_partita",
         cmd_print_filtered[] = "+stampa_filtrate",
         cmd_insert_begin[] =   "+inserisci_inizio",
         cmd_insert_end[] =     "+inserisci_fine";
    char *new_word, *ref_word;

    struct node* root = NULL;

    // read word length
    // TODO: change scanf with a more performing input function
    scanf_return = scanf("%d", &k);
    fflush_unlocked(stdin);

    // read admissible words
    do {
        new_word = (char *) malloc(sizeof(char) * k);
        scanf_return = scanf("%s", new_word);
        fflush_unlocked(stdin);
        // acquireWord(new_word);
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
        word_count = 0;
        bool filtered_flag = false;
        bool print_flag = false;
        winner_flag = false;

        setAllComp(root);

        // read reference word
        ref_word = (char *) malloc(sizeof(char) * k);
        scanf_return = scanf("%s", ref_word);
        fflush_unlocked(stdin);
        //acquireWord(ref_word);

        // read number n of words
        scanf_return = scanf("%d", &n);
        fflush_unlocked(stdin);

        // read words sequence
        do {
            setFlagFalse(cstr);
            new_word = (char *) malloc(sizeof(char) * k);
            scanf_return = scanf("%s", new_word);
            fflush_unlocked(stdin);
            // acquireWord(new_word);
            if (strcmp(new_word, cmd_print_filtered) == 0) {
                printFiltered(root);
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
                    print_flag = compare(ref_word, new_word);
                    banWord(root, cstr);
                    if (print_flag) {
                        compWordCount = printCompWord(root, false);
                        printf("%d\n", compWordCount);
                    } else {
                        printf("ok\n");
                        winner_flag = true;
                    }
                    word_count++;
                } else {
                    printf("not_exists\n");
                }
            }

        } while (word_count < n && !winner_flag);

        if (winner_flag == false) {
            printf("ko\n");
        }

        do {
            new_word = (char *) malloc(sizeof(char) * k);
            scanf_return = scanf("%s", new_word);
            fflush_unlocked(stdin);
            // acquireWord(new_word);
            if (strcmp(new_word, cmd_new_game) != 0) {
                if (strcmp(new_word, cmd_insert_begin) == 0 && !filtered_flag) {
                    filtered_flag = true;
                } else {
                    if (strcmp(new_word, cmd_insert_end) == 0) {
                        filtered_flag = false;
                    } else {
                        insert(root, new_word);
                    }
                }
            }
        } while (strcmp(new_word, cmd_new_game) != 0);

        // TODO: deallocate memory
        scanf_return = scanf_return + 1;
        cstr = NULL;
        ref_word = NULL;
    } while (true);
}