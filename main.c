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
    struct constraint* temp = (struct constraint*)malloc(sizeof(struct constraint));
    temp->symbol = symbol;
    temp->belongs = b;
    if (b == true) {
        temp->is_present = (bool *) malloc(sizeof(bool) * k);
        temp->not_present = (bool *) malloc(sizeof(bool) * k);
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

// #pragma clang diagnostic push
// #pragma ide diagnostic ignored "EndlessLoop"

int main() {
    int n = 0, word_count, compWordCount, scanf_return;
    char cmd_new_game[] =       "+nuova_partita",
         cmd_print_filtered[] = "+stampa_filtrate",
         cmd_insert_begin[] =   "+inserisci_inizio",
         cmd_insert_end[] =     "+inserisci_fine";
    char *new_word, *ref_word;
    char read_char;

    struct node* root = NULL;

    // read word length
    // TODO: change scanf with a more performing input function
    scanf_return = scanf("%d", &k);

    visited = (int *) malloc(sizeof(int) * k);
    result = (char *) malloc(sizeof(char) * k);
    is_present = (bool *) malloc(sizeof(bool) * k);
    not_present = (bool *) malloc(sizeof(bool) * k);

    // read admissible words
    do {
        new_word = (char *) malloc(sizeof(char) * k);
        scanf_return = scanf("%s", new_word);
        if (strcmp(new_word, cmd_new_game) != 0) {
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

        // read number n of words
        scanf_return = scanf("%d", &n);

        // read words sequence
        do {
            // setFlagFalse(cstr);
            new_word = (char *) malloc(sizeof(char) * k);
            scanf_return = scanf("%s", new_word);
            // TODO: copy tmp_word into new_word
            if (strcmp(new_word, cmd_print_filtered) == 0) {
                banWord(root, cstr);
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
            int i = 0;
            new_word = (char *) malloc(sizeof(char) * k);
            do {
                read_char = (char) getchar_unlocked();
                if (read_char == EOF) {
                    break;
                } else {
                    if (read_char != 10) {
                        new_word[i] = read_char;
                    }
                    i++;
                }
            } while (read_char != 10);

            if (read_char == EOF) {
                break;
            }

            if (strcmp(new_word, cmd_new_game) != 0) {
                if (strcmp(new_word, cmd_insert_begin) == 0 && !filtered_flag) {
                    filtered_flag = true;
                } else {
                    if (strcmp(new_word, cmd_insert_end) == 0) {
                        filtered_flag = false;
                    } else {
                        if (strlen(new_word) > 0) {
                            insert(root, new_word);
                        }
                    }
                }
            }
        } while (strcmp(new_word, cmd_new_game) != 0);

        // TODO: deallocate memory
        scanf_return = scanf_return + 1;
        cstr = NULL;
    } while (read_char != EOF);
}