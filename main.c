#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define LENGTH 64

// ------------- GLOBAL VARIABLES ----------------

// TODO: Change cstr and make it local!
int k, * visited, comp_word;
char * result, * word_array;
bool * is_present;
bool * not_present;
bool exac_flag = false;
bool winner_flag = false;
char * secure_word;
struct constraint *cstr[LENGTH];
struct Trie* root;

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
    temp->not_present = (bool *) malloc(sizeof(bool) * k);
    for (int i = 0; i < k; i++) {
        temp->not_present[i] = false;
    }
    temp->min_number = min;
    temp->exact_number = exac;
    return temp;
}

// --------------- BST structure -----------------

struct Trie {
    int isLeaf; // 1 quando il nodo è un nodo foglia
    bool isCompatible;
    struct Trie* character[LENGTH];
};

struct Trie* getNewTrieNode() {
    struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
    node->isLeaf = 0;
    node->isCompatible = true;
    for (int i = 0; i < LENGTH; i++) {
        node->character[i] = NULL;
    }
    return node;
}

void insert(struct Trie *head, char* str) {
    struct Trie* curr = head;
    while (*str) {
        if (curr->character[lookUpFunction(*str)] == NULL) {
            curr->character[lookUpFunction(*str)] = getNewTrieNode();
        }
        curr = curr->character[lookUpFunction(*str)];
        str++;
    }
    curr->isLeaf = 1;
}

void printFiltered(struct Trie* root, int pos) {
    if(root == NULL)
        return;
    if(root->isLeaf == 1) {
        printf("%s\n", word_array);
    }
    // TODO: compatible?
    for(int i = 0; i < LENGTH; i++) {
        if(root->character[i] != NULL) {
            if (root->isCompatible) {
                word_array[pos] = i + 'a';
                printFiltered(root->character[i], pos+1);
            } else {
                return;
            };
        }
    }
}

void freeWord(struct node* node) {
    // TODO: complete deallocation for words
}

int hasChildren(struct Trie* curr) {
    for (int i = 0; i < LENGTH; i++) {
        if (curr->character[i]) {
            return 1;       // bambino trovato
        }
    }
    return 0;
}

void setAllComp(struct Trie* node) {
    if(root == NULL)
        return;
    for(int i = 0; i < LENGTH; i++) {
        if(node->character[i] != NULL) {
            if (!node->isCompatible) {
                node->isCompatible = true;
                comp_word++;
            }
        }
    }
}

struct Trie * search(struct Trie* head, char* str) {
    if (head == NULL) {
        return NULL;
    }
    struct Trie* curr = head;
    while (*str) {
        curr = curr->character[lookUpFunction(*str)];
        if (curr == NULL) {
            return NULL;
        }
        str++;
    }
    if (curr->isLeaf == 1) {
        return curr;
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

bool checkComp(struct Trie *node, struct constraint * constraintNode) {
    int tmp_count = 0;
    // if symbol is part of the word
    if (constraintNode->belongs) {
        for (int i = 0; i < k; i++) {
            if (node->word[i] == constraintNode->symbol) {
                tmp_count++;
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
        if (strchr(node->word, constraintNode->symbol) != NULL) {
            node->compatible = false;
            return true;
        }
    }
    return false;
}

void banWord(struct Trie* node, struct constraint* constraintNode) {
    if (node == NULL)
        return;
    for (int i = 0; i < LENGTH; i++) {
        if (node->character[i] != NULL) {
            if (node->isCompatible) {
                if (i != lookUpFunction(secure_word[i]) && secure_word[i] != '$') {
                    node->isCompatible = false;
                }
                if (node->isCompatible) {
                    checkComp(node, cstr[i]);
                    banWord(node->character[i], constraintNode);
                }
            }
        }
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
    struct Trie* root = getNewTrieNode();

    comp_word = 0;

    // read word length
    scanf_return = scanf("%d", &k);
    getchar_unlocked();

    // read admissible words
    do {
        new_word = (char *) malloc(sizeof(char) * (k + 1));
        return_code = wordHandler(new_word);
        if (return_code == 0) {
            insert(root, new_word);
        }
    } while (return_code != 4);
    // free(new_word);

    do {
        // initialize new game
        visited = (int *) malloc(sizeof(int) * (k));
        result = (char *) malloc(sizeof(char) * (k));
        is_present = (bool *) malloc(sizeof(bool) * (k));
        not_present = (bool *) malloc(sizeof(bool) * (k));
        word_array = (char *) malloc(sizeof(char) * k);

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
                printFiltered(root, 0);
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