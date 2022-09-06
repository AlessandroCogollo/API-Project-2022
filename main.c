#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define LENGTH 64

// ------------- GLOBAL VARIABLES ----------------

// TODO: Change cstr and make it local!
int k, * visited, comp_word;
char * result;
bool * is_present;
bool * not_present;
bool exac_flag = false;
bool winner_flag = false;
char * secure_word;
struct constraint *cstr[LENGTH];
struct node* root;

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
    bool *not_present;
    int min_number;
    int exact_number;
};

int lookUpFunction(char symbol) {
    int value = (int) symbol;
    if (value >= 65 && value <= 90) {
        value = value - 65;
    } else if (value >= 97 && value <= 122) {
        value = value - 71;
    } else if (value >= 48 && value <= 57) {
        value = value + 4;
    } else if (value == 45) {
        // char: -
        value = 62;
    } else if (value == 95) {
        value = 63;
    }
    return value;
}

struct constraint* isPresent(char symbol) {
    int value = lookUpFunction(symbol);
    if (cstr[value] != NULL) {
        return cstr[value];
    } else {
        return NULL;
    }
}

struct constraint *newConstraint(char symbol, int b, int min, int exac) {
    // TODO: change the way nodes (both constraints and words are allocated):
    //    instead of this --> node = malloc(sizeof(node))
    //      node->string = malloc(sizeof(char)*length)
    //    do this --> malloc(sizeof(node)+sizeof(char)*length)

    struct constraint * temp = (struct constraint *)malloc(sizeof(struct constraint));
    temp->symbol = symbol;
    temp->belongs = b;
    if (b == true) {
        temp->not_present = (bool *) malloc(sizeof(bool) * k);
        for (int i = 0; i < k; i++) {
            temp->not_present[i] = false;
        }
    } else {
        temp->not_present = NULL;
    }
    temp->min_number = min;
    temp->exact_number = exac;
    return temp;
}

// --------------- BST structure -----------------

struct node {
    char *word;
    bool compatible;
    int color;
    struct node *parent, *left, *right;
};

void rightRotate(struct node* temp) {
    struct node* left = temp->left;
    temp->left = left->right;
    if (temp->left)
        temp->left->parent = temp;
    left->parent = temp->parent;
    if (!temp->parent)
        root = left;
    else if (temp == temp->parent->left)
        temp->parent->left = left;
    else
        temp->parent->right = left;
    left->right = temp;
    temp->parent = left;
}

void leftRotate(struct node* temp) {
    struct node* right = temp->right;
    temp->right = right->left;
    if (temp->right)
        temp->right->parent= temp;
    right->parent = temp->parent;
    if (!temp->parent)
        root = right;
    else if (temp == temp->parent->left)
        temp->parent->left = right;
    else
        temp->parent->right = right;
    right->left = temp;
    temp->parent = right;
}

struct node* newNode(char scannedWord[]) {
    struct node* temp = (struct node*)malloc(sizeof(struct node));
    temp->word = scannedWord;
    temp->color = 1; // red is 1, black is 0;
    temp->compatible = true;
    temp->left = temp->right = temp->parent = NULL;
    return temp;
}

void printFiltered(struct node* node) {
    if (node != NULL) {
        printFiltered(node->left);
        if (node->compatible) {
            printf("%s\n", node->word);
        }
        printFiltered(node->right);
    }
}

void freeWord(struct node* node) {
    if (node == NULL) return;
    freeWord(node->left);
    freeWord(node->right);
    free(node->word);
    free(node);
}

int printCompWord(struct node* node, bool print) {
    if (node == NULL)
        return 0;
    else {
        if (node->compatible) {
            return 1 + printCompWord(root->left, print) + printCompWord(root->right, print);
        } else {
            return printCompWord(root->left, print) + printCompWord(root->right, print);
        }
    }
}

struct node* insert(struct node* node, char word[]) {
    if (node == NULL) {
        comp_word++;
        return newNode(word);
    }
    if (strcmp(word, node->word) < 0) {
        node->left = insert(node->left, word);
        node->left->parent = node;
    } else if (strcmp(word, node->word) > 0) {
        node->right = insert(node->right, word);
        node->right->parent = node;
    }
    return node;
}

void fixup(struct node* node, struct node* pt) {
    struct node* parent_pt = NULL;
    struct node* grand_parent_pt = NULL;

    while ((pt != node) && (pt->color != 0) && (pt->parent->color == 1)) {
        parent_pt = pt->parent;
        grand_parent_pt = pt->parent->parent;

        if (parent_pt == grand_parent_pt->left) {
            struct node* uncle_pt = grand_parent_pt->right;

            if (uncle_pt != NULL && uncle_pt->color == 1) {
                grand_parent_pt->color = 1;
                parent_pt->color = 0;
                uncle_pt->color = 0;
                pt = grand_parent_pt;
            } else {
                if (pt == parent_pt->right) {
                    leftRotate(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }
                rightRotate(grand_parent_pt);
                int t = parent_pt->color;
                parent_pt->color = grand_parent_pt->color;
                grand_parent_pt->color = t;
                pt = parent_pt;
            }
        } else {
            struct node* uncle_pt = grand_parent_pt->left;
            if ((uncle_pt != NULL) && (uncle_pt->color == 1)) {
                grand_parent_pt->color = 1;
                parent_pt->color = 0;
                uncle_pt->color = 0;
                pt = grand_parent_pt;
            } else {
                if (pt == parent_pt->left) {
                    rightRotate(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }
                leftRotate(grand_parent_pt);
                int t = parent_pt->color;
                parent_pt->color = grand_parent_pt->color;
                grand_parent_pt->color = t;
                pt = parent_pt;
            }
        }
    }
    node->color = 0;
}

void setAllComp(struct node* node) {
    if (node != NULL) {
        setAllComp(node->left);
        node->compatible = true;
        comp_word++;
        setAllComp(node->right);
    }
}

struct node* search(struct node* node, char* word) {
    if (node != NULL) {
        if (strcmp(node->word, word) == 0){
            return node;
        }
        if (strcmp(word, node->word) < 0) {
            return search(node->left, word);
        } else {
            return search(node->right, word);
        }
    } else {
        return NULL;
    }
}

void freeBST() {
    for (int i = 0; i < 54; i++) {
        if (cstr[i] != NULL) {
            free(cstr[i]);
        }
    }
}

bool checkComp(struct node *node, struct constraint * constraintNode) {
    if (constraintNode->belongs) {
        int tmp_count = 0;
        // if symbol is part of the word
        for (int i = 0; i < k; i++) {
            if (node->word[i] == constraintNode->symbol) {
                tmp_count++;
            }
            if (node->word[i] == constraintNode->symbol) {
                if (constraintNode->not_present[i]) {
                    node->compatible = false;
                    return true;
                }
            }
        }
        if (constraintNode->exact_number > 0) {
            if (tmp_count != constraintNode->exact_number) {
                node->compatible = false;
                return true;
            }
        } else {
            if (tmp_count < constraintNode->min_number) {
                node->compatible = false;
                return true;
            }
        }
    } else {
        // if symbol doesn't belong to the word
        if (strchr(node->word, constraintNode->symbol) != NULL) {
            node->compatible = false;
            return true;
        }
    }
    return false;
}

void banWord(struct node* node, struct constraint* constraintNode) {
    if (node != NULL) {
        banWord(node->left, constraintNode);
        if (node->compatible) {
            bool flag = false;
            for (int i = 0; i < LENGTH && !flag; i++) {
                if (i < k) {
                    if (secure_word[i] != node->word[i] && secure_word[i] != '$') {
                        node->compatible = false;
                        flag = true;
                    }
                }
                if (cstr[i] != NULL && !flag) {
                    flag = checkComp(node, cstr[i]);
                }
            }
            if (node->compatible) {
                comp_word++;
            }
        }
        banWord(node->right, constraintNode);
    }
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
    int lookupvalue = lookUpFunction(symbol);
    if (cstr[lookupvalue] == NULL) {
        cstr[lookupvalue] = newConstraint(symbol, belongs, min_number, exact_number);
    }
    if (cstr[lookupvalue]->belongs) {
        for (int i = 0; i < k; i++) {
            if (is_present[i]) {
                secure_word[i] = symbol;
            }
            if (not_present[i]) {
                cstr[lookupvalue]->not_present[i] = true;
            }
        }
        if (cstr[lookupvalue]->min_number < min_number) {
            cstr[lookupvalue]->min_number = min_number;
        }
        if (exac_flag) {
            cstr[lookupvalue]->exact_number = min_number;
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
    int n = 0, word_count, scanf_return, return_code;
    char *new_word, *ref_word;
    struct node * temp_ptr = NULL;

    comp_word = 0;

    // read word length
    scanf_return = scanf("%d", &k);
    getchar_unlocked();

    // read admissible words
    do {
        new_word = (char *) malloc(sizeof(char) * (k + 1));
        return_code = wordHandler(new_word);
        if (return_code == 0) {
            if (root == NULL) {
                root = insert(root, new_word);
            } else {
                temp_ptr = insert(root, new_word);
                fixup(root, temp_ptr);
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

        secure_word = (char *) malloc(sizeof(char) * k);
        memset(secure_word, '$', k);
        for (int i = 0; i < LENGTH; i++) {
            cstr[i] = NULL;
        }

        word_count = 0;
        bool filtered_flag = false;
        bool print_flag = false;
        winner_flag = false;

        // read reference word
        ref_word = (char *) malloc(sizeof(char) * (k+1));
        // scanf_return = scanf("%s", ref_word);
        wordHandler(ref_word);

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
                printFiltered(root);
            } else if (return_code == 1 && !filtered_flag) {
                filtered_flag = true;
            } else if (filtered_flag == true) {
                if (return_code == 2) {
                    comp_word = 0;
                    banWord(root, *cstr);
                    filtered_flag = false;
                } else {
                    insert(root, new_word);
                }
            } else {
                if (search(root, new_word) != NULL) {
                    print_flag = compare(ref_word, new_word);
                    comp_word = 0;
                    banWord(root, *cstr);
                    if (print_flag) {
                        // compWordCount = printCompWord(root, false);
                        printf("%d\n", comp_word);
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


        freeBST();
        free(ref_word);
        free(visited);
        free(result);
        free(is_present);
        free(not_present);
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
        comp_word = 0;
        setAllComp(root);

    } while (return_code != 5);
}