#define CONSTQUANTITY 64

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// TODO: allocate whole pages to speed up

int quantity;

// ----------- CONSTRAINTS --------------

typedef struct {
    // not present: -1, not initialized: 0, present: 1
    int *presence;
    // doesn't belong: -2, not initialized: -1
    int cardinality;
    bool exact_number;
} constraintCell;

void resetConstraints(constraintCell * cArr, int length, bool firstTime) {
    for (int i = 0; i < CONSTQUANTITY; i++) {
        cArr[i].cardinality = -1;
        if (firstTime) {
            cArr[i].presence = (int *) malloc (sizeof(int) * length);
        }
        for (int j = 0; j < length; j++) {
            cArr[i].presence[j] = 0;
        }
        cArr[i].exact_number = false;
    }
}

int constraintMapper(char character) {
    int ASCII_value = (int) character;
    if (ASCII_value > 64 && ASCII_value < 91) {
        // Capital Letters (0 - 25)
        ASCII_value = ASCII_value - 65;
    } else if (ASCII_value > 96 && ASCII_value < 123) {
        // Lowercase Letters (26 - 50)
        ASCII_value = ASCII_value - 71;
    } else if (ASCII_value > 47 && ASCII_value < 58) {
        // Numbers (51 - 60)
        ASCII_value = ASCII_value + 4;
    } else if (ASCII_value == 45) {
        // Minus
        ASCII_value = 62;
    } else if (ASCII_value == 95) {
        // Lower bar
        ASCII_value = 63;
    }
    return ASCII_value;
}

void setConstraint(constraintCell * cArr, char character, int mode) {
    int val = constraintMapper(character);
    // TODO: complete this
}

// -------------- LIST -----------------

struct nodeLIST {
    char *word;
    struct nodeLIST *next;
};

struct nodeLIST * newNodeList(char *word) {
    struct nodeLIST * new_node;
    new_node = (struct nodeLIST *) malloc (sizeof(struct nodeLIST));
    new_node->word = word;
    new_node->next = NULL;
    return new_node;
}

void deleteNode(struct nodeLIST ** head_ref, char * word) {
    // TODO: change this below (from internet)
    struct nodeLIST *temp = *head_ref, *prev;

    if (temp != NULL && temp->word == word) {
        *head_ref = temp->next;
        free(temp);
        return;
    }

    while (temp != NULL && strcmp(temp->word, word)!= 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
        return;

    prev->next = temp->next;
    free(temp);
}

void printList (struct nodeLIST * node) {
    struct nodeLIST *temp = node;
    do {
        printf("%s\n", temp->word);
        temp = temp->next;
    } while (temp != NULL);
}

bool searchList (struct nodeLIST * node, char * word) {
    struct nodeLIST *temp = node;
    int ret_code;
    do {
        ret_code = strcmp(temp->word, word);
        temp = temp->next;
        if (ret_code == 0) {
            return true;
        } else if (ret_code < 0) {
            return false;
        }
    } while (temp != NULL);
    return false;
}

// --------------- BST -----------------

// TODO: change BST to RB-Tree

struct nodeBST {
    char *word;
    struct nodeBST *left, *right;
};

struct nodeBST * newNodeBST(char *word) {
    struct nodeBST * new_node;
    new_node = (struct nodeBST *) malloc (sizeof (struct nodeBST));
    new_node->word = word;
    new_node->left = new_node->right = NULL;
    return new_node;
}

struct nodeBST * insertNodeBST(struct nodeBST *node, char *word) {
    if (node == NULL)
        return newNodeBST(word);
    if (strcmp(word, node->word) < 0)
        node->left = insertNodeBST(node->left, word);
    else if (strcmp(word, node->word) > 0)
        node->right = insertNodeBST(node->right, word);
    return node;
}

void newList(struct nodeBST *node, struct nodeLIST **root, struct nodeLIST **head) {
    if (node != NULL) {
        newList(node->left, root, head);
        // TODO: manage duplicates
        if (*root == NULL) {
            *root = newNodeList(node->word);
            *head = *root;
        } else {
            (*head)->next = newNodeList(node->word);
            *head = (*head)->next;
        }
        quantity++;
        newList(node->right, root, head);
    }
}

// -------------- UTILS ----------------

bool counter(const char wordRef[], const char wordP[], int pos) {
    // TODO: check efficiency
    int n = 0, c = 0, d = 0, k;
    k = (int)strlen(wordRef);
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

bool compare(char * ref_word, char * new_word, char * result_word, constraintCell * cArr) {
    // TODO: change length acquisition: pass it as a parameter
    int length = (int) strlen(ref_word);
    bool exists, win_flag = true;
    for (int i = 0; i < length; i++) {
        if (new_word[i] == ref_word[i]) {
            result_word[i] = '+';
            setConstraint(cArr, new_word[i], 0);
        } else {
            exists = false;
            for (int j = 0; j < length; j++) {
                if (new_word[i] == ref_word[j]) {
                    exists = true;
                }
            }
            if (!exists || counter(ref_word, new_word, i)) {
                result_word[i] = '/';
                if (cArr[constraintMapper(new_word[i])].cardinality != -1) {
                    setConstraint(cArr, new_word[i], 1);
                } else {
                    setConstraint(cArr, new_word[i], 2);
                }
            } else {
                result_word[i] = '|';
                setConstraint(cArr, new_word[i], 3);
            }
            win_flag = false;
        }
    }
    return win_flag;
}

void banwords(struct nodeLIST * root, constraintCell * constraints) {
    struct nodeLIST *temp = root;
    bool to_ban_flag;
    do {
        // TODO: execute comparison with constraints
        if (to_ban_flag) {
            // TODO: delete node
            quantity--;
        }
        temp = temp->next;
    } while (temp != NULL);
}

int getWord(char *temp_word, int length) {
    int i = 0;
    do {
        temp_word[i] = (char) getchar_unlocked();
        i++;
    } while (temp_word[i-1] != '\n');
    if (temp_word[0] == '+') {
        switch (temp_word[12]) {
            case 't': // +nuova_partita
                return 1;
            case 'r': // +stampa_filtrate
                return 2;
            case 'n': // +inserisci_inizio
                return 3;
            case 'i': // +inserisci_fine
                return 4;
        }
    }
    temp_word[length] = '\0';
    return 0;
}

int main() {
    bool winner_flag, filtered_flag;
    int i, k, n, code;
    char *temp_word, *reference_word, *result_word;
    struct nodeBST* rootBST = NULL;
    struct nodeLIST* rootLIST = NULL;
    struct nodeLIST* headLIST = NULL;
    constraintCell constraints[CONSTQUANTITY];

    // acquire length:
    k = (int) getchar_unlocked() - 48;
    getchar_unlocked();

    // acquire words:
    do {
        temp_word = (char *) malloc(sizeof(char) * k);
        code = getWord(temp_word, k);
        if (code == 0) {
            if (rootBST == NULL) {
                rootBST = insertNodeBST(rootBST, temp_word);
            } else {
                insertNodeBST(rootBST, temp_word);
            }
        }
    } while (code == 0);

    resetConstraints(constraints, k, true);

    // new game begins
    do {
        // acquire reference word:
        reference_word = (char *) malloc(sizeof(char) * k);
        result_word = (char *) malloc(sizeof(char) * k);
        getWord(reference_word, k);

        // acquire tries quantity
        n = (int) getchar_unlocked() - 48;

        // generate new list
        quantity = 0;
        newList(rootBST, &rootLIST, &headLIST);

        i = 0;
        winner_flag = filtered_flag = false;

        do {
            code = getWord(temp_word, k);
            if (code == 2) {
                printList(rootLIST);
            } else if (code == 3 && !filtered_flag) {
                filtered_flag = true;
            } else if (filtered_flag == true) {
                if (code == 4) {
                    filtered_flag = false;
                    // TODO: insert in list
                } else {
                    insertNodeBST(rootBST, temp_word);
                }
            } else {
                if (!searchList(rootLIST, temp_word)) {
                    winner_flag = compare(reference_word, temp_word, result_word, constraints);
                    banwords(rootLIST, constraints);
                    if (!winner_flag) {
                        printf("%s, %d\n", result_word, quantity);
                    } else {
                        printf("ok\n");
                    }
                    i++;
                } else {
                    printf("not_exists\n");
                }
            }
        } while (i < n && !winner_flag);

        if (!winner_flag) {
            printf("ko\n");
        }

        do {
            code = getWord(temp_word, k);
            if (code == 1) {
                resetConstraints(constraints, k, false);
            } else {
                if (code == 3 && !filtered_flag) {
                    filtered_flag = true;
                } else {
                    if (code == 4) {
                        filtered_flag = false;
                        // TODO: insert in list
                    } else {
                        insertNodeBST(rootBST, temp_word);
                    }
                }
            }
        } while (code != 1);

    } while (true);
}
