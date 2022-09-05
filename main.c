#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// ------------- GLOBAL VARIABLES ----------------

// TODO: Change cstr and make it local!
int k, * visited;
char * result;
bool exac_flag = false;
bool * is_present;
bool * not_present;
bool winner_flag = false;
struct constraint* cstr;

// -------------- UTILS ----------------

int wordHandler(char *pointer) {
    char tmp_word[20], read_char;
    int i = 0;
    memset(tmp_word, '\0', 20);
    do {
        read_char = (char) getchar_unlocked();
        if (read_char != 10 && read_char != EOF) {
            tmp_word[i] = read_char;
        } else if (read_char == EOF) {
            // return end of game
            return 5;
        }
        i++;
    } while (read_char != 10);
    // tmp_word[i] = '\n';
    strcpy(pointer, tmp_word);
    if (i == k + 1) {
        return 0; // word is returned
    } else {
        // pointer = NULL;
        if (strcmp(tmp_word, "+inserisci_inizio") == 0) {
            return 1; // word is + inserisci_inizio
        } else if (strcmp(tmp_word, "+inserisci_fine") == 0) {
            return 2; // word is +inserisci_fine
        } else if (strcmp(tmp_word, "+stampa_filtrate") == 0) {
            return 3; // word is +stampa_filtrate
        } else {
            return 4; // word is +nuova_partita
        }
    }
}

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
    bool *is_present;
    bool *not_present;
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

struct constraint *newConstraint(char symbol, int b, int min, int exac) {
    // TODO: change the way nodes (both constraints and words are allocated):
    //    instead of this --> nodo = malloc(sizeof(nodo))
    //      nodo->stringa = malloc(sizeof(char)*length)
    //    do this --> malloc(sizeof(nodo)+sizeof(char)*length)

    struct constraint* temp = (struct constraint*)malloc(sizeof(struct constraint));
    temp->symbol = symbol;
    temp->belongs = b;
    if (b == true) {
        temp->is_present = (bool *) malloc(sizeof(bool) * k);
        temp->not_present = (bool *) malloc(sizeof(bool) * k);
        for (int i = 0; i < k; i++) {
            temp->is_present[i] = false;
            temp->not_present[i] = false;
        }
    } else {
        temp->is_present = NULL;
        temp->not_present = NULL;
    }
    temp->min_number = min;
    temp->exact_number = exac;
    temp->left = temp->right = NULL;
    return temp;
}

struct constraint* insertConstraint(struct constraint* node, char symbol, bool b, int min, int exac) {
    if (node == NULL)
        return newConstraint(symbol, b, min, exac);
    if (symbol < node->symbol)
        node->left = insertConstraint(node->left, symbol, b, min, exac);
    else if (symbol > node->symbol)
        node->right = insertConstraint(node->right, symbol, b, min, exac);
    return node;
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

void freeWord(struct node* root) {
    if (root == NULL) return;
    freeWord(root->left);
    freeWord(root->right);
    free(root->word);
    free(root);
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

void freeBST(struct constraint* node) {
    if (node == NULL) return;
    freeBST(node->left);
    freeBST(node->right);
    if (node->belongs) {
        free(node->is_present);
        free(node->not_present);
    }
    free(node);
}

void checkComp(struct node *node, struct constraint * constraintNode) {
    if (constraintNode != NULL) {
        checkComp(node, constraintNode->left);
        if (node->compatible) {
            if (constraintNode->belongs) {
                int tmp_count = 0;
                // if symbol is part of word
                for (int i = 0; i < k; i++) {
                    if (node->word[i] == constraintNode->symbol) {
                        tmp_count++;
                    }
                    if (constraintNode->not_present[i]) {
                        if (node->word[i] == constraintNode->symbol) {
                            node->compatible = false;
                            // printf("Word: %s, banned because symbol shouldn't be placed here [i = %d]\n", node->word, i);
                        }
                    }
                    if (constraintNode->is_present[i]) {
                        if (node->word[i] != constraintNode->symbol) {
                            node->compatible = false;
                            // printf("Word: %s, Banned because symbol should be placed here [i = %d]\n", node->word, i);
                        }
                    }
                }
                if (constraintNode->exact_number > 0) {
                    if (tmp_count != constraintNode->exact_number) {
                        node->compatible = false;
                        // printf("Word: %s, Banned because of exact_number of symbol: %c\n"
                        //       "Exact number of symbol is: %d\n", node->word, constraintNode->symbol, constraintNode->min_number);
                    }
                } else {
                    if (tmp_count < constraintNode->min_number) {
                        node->compatible = false;
                        // printf("Word: %s, Banned because of smaller min_number of symbol: %c\n"
                        //       "Min number of symbol is: %d\n", node->word, constraintNode->symbol, constraintNode->min_number);
                    }
                }
            } else {
                // if symbol doesn't belong to the word
                if (strchr(node->word, constraintNode->symbol) != NULL) {
                    node->compatible = false;
                    // printf("Word: %s, Banned because of not belonging symbol\n", node->word);
                }
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

int explore(char reference[], char new[], int pos, bool incr_flag) {
    int min_number = 0;
    exac_flag = false;
    int l = pos - 1;
    while (l >= 0) {
        if (visited[l] < 0) {
            if (new[l] == new[pos]) {
                if (reference[l] == new[l]) {
                    min_number++;
                    is_present[l] = true;
                } else {
                    if (counter(reference, new, l)) {
                        exac_flag = true;
                    } else {
                        incr_flag = true;
                    }
                    not_present[l] = true;
                }
                visited[l] = 1;
            }
        }
        l--;
    }
    if (incr_flag) {
        min_number++;
    }
    return min_number;
}

void constraintHandler(char symbol, bool belongs, int min_number, int exact_number) {
    struct constraint *constraintNode = isPresent(cstr, symbol);
    if (cstr == NULL) {
        cstr = insertConstraint(cstr, symbol, belongs, min_number, exact_number);
    } else {
        if (constraintNode == NULL) {
            // if symbol doesn't exist
            insertConstraint(cstr, symbol, belongs, min_number, exact_number);
        }
    }
    constraintNode = isPresent(cstr, symbol);
    if (constraintNode->belongs) {
        for (int i = 0; i < k; i++) {
            if (is_present[i]) {
                constraintNode->is_present[i] = true;
            }
            if (not_present[i]) {
                constraintNode->not_present[i] = true;
            }
        }
        if (constraintNode->min_number < min_number) {
            constraintNode->min_number = min_number;
        }
        if (exac_flag) {
            constraintNode->exact_number = min_number;
        }
    }
}

bool compare(char reference[], char new[]) {
    bool belongs_flag;
    memset(result, '\0', k + 1);
    int i = k - 1, min_number = 0, exact_number = 0;
    for (int j = 0; j < k; j++) {
        visited[j] = -1;
    }
    while (i >= 0) {
        for (int j = 0; j < k; j++) {
            is_present[j] = false;
            not_present[j] = false;
        }
        if (reference[i] == new[i]) {
            result[i] = '+';
            belongs_flag = true;
            is_present[i] = true;
            min_number = explore(reference, new, i, false) + 1;
        } else {
            if (strchr(reference, new[i]) == NULL) {
                result[i] = '/';
                belongs_flag =  false;
            } else if (counter(reference, new, i)) {
                result[i] = '/';
                exact_number = explore(reference, new, i, false);
                not_present[i] = true;
                belongs_flag = true;
            } else {
                result[i] = '|';
                not_present[i] = true;
                belongs_flag = true;
                min_number = explore(reference, new, i, true);
            }
        }
        constraintHandler(new[i], belongs_flag, min_number, exact_number);
        visited[i] = 1;
        i--;
    }
    if (strchr(result, '/') == NULL && strchr(result, '|') == NULL) {
        return false;
    } else {
        printf("%s\n", result);
        return true;
    }
}

int main() {
    int n = 0, word_count, compWordCount, scanf_return;
    char *new_word, *ref_word;

    struct node* root = NULL;

    int return_code;

    // read word length
    scanf_return = scanf("%d", &k);
    getchar_unlocked();

    // read admissible words
    do {
        new_word = (char *) malloc(sizeof(char) * (k + 1));
        // scanf_return = scanf("%s", new_word);
        return_code = wordHandler(new_word);
        if (return_code == 0) {
            if (root == NULL) {
                root = insert(root, new_word);
            } else {
                insert(root, new_word);
            }
        }
    } while (return_code != 4);
    // free(new_word);

    do {
        // initialize new game
        visited = (int *) malloc(sizeof(int) * (k));
        result = (char *) malloc(sizeof(char) * (k));
        is_present = (bool *) malloc(sizeof(bool) * (k));
        not_present = (bool *) malloc(sizeof(bool) * (k));

        word_count = 0;
        bool filtered_flag = false;
        bool print_flag = false;
        winner_flag = false;

        // read reference word
        ref_word = (char *) malloc(sizeof(char) * (k+1));
        // scanf_return = scanf("%s", ref_word);
        return_code = wordHandler(ref_word);

        // read number n of words
        // scanf_return = scanf("%d", &n);
        scanf_return = scanf("%d", &n);
        getchar_unlocked();

        // read words sequence
        do {
            new_word = (char *) malloc(sizeof(char) * (k+1));
            return_code = wordHandler(new_word);
            // scanf_return = scanf("%s", new_word);
            if (return_code == 3) {
                banWord(root, cstr);
                printFiltered(root);
            } else if (return_code == 1 && !filtered_flag) {
                filtered_flag = true;
            } else if (filtered_flag == true) {
                if (return_code == 2) {
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

        // freeing memory


        freeBST(cstr);
        free(ref_word);
        free(visited);
        free(result);
        free(is_present);
        free(not_present);
        cstr = NULL;
        visited = NULL;
        result = NULL;
        is_present = not_present = NULL;

        do {
            // TODO: complete this
            new_word = (char *) malloc(sizeof(char) * (k+1));
            return_code = wordHandler(new_word);
            if (return_code == 5) {
                break;
            }

            if (return_code != 4) {
                if (return_code == 1 && !filtered_flag) {
                    filtered_flag = true;
                } else {
                    if (return_code == 2) {
                        filtered_flag = false;
                    } else {
                        if (strlen(new_word) > 0) {
                            insert(root, new_word);
                        }
                    }
                }
            }
        } while (return_code != 4);

        // TODO: deallocate memory
        scanf_return = scanf_return + 1;
        setAllComp(root);

    } while (return_code != 5);
}