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

void printList (struct nodeLIST * node) {
    struct nodeLIST *temp = node;
    while (temp != NULL) {
        printf("%s\n", temp->word);
        temp = temp->next;
    }
}

// TODO: change this below, taken from the internet
void insertNode(struct nodeLIST ** head_ref, char * word) {
    struct nodeLIST * temp = (struct nodeLIST *) malloc (sizeof(struct nodeLIST));
    temp->word = word;
	if (*head_ref == NULL || strcmp((*head_ref)->word, word) > 0) {
		temp->next = *head_ref;
		*head_ref = temp;
		return;
	}
    struct nodeLIST* current = *head_ref;
	while (current->next != NULL && strcmp(current->next->word, word) < 0)
		current = current->next;

	temp->next = current->next;
	current->next = temp;
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

void resetList(struct nodeLIST ** root) {
    struct nodeLIST * temp = *root, * succ;
    while (temp != NULL) {
        succ = temp->next;
        free(temp);
        temp = succ;
    }
    * root = NULL;
}

struct nodeBST * searchBST(struct nodeBST *node, char *word) {
    if (node == NULL || strcmp(word, node->word) == 0) {
        return node;
    } else if (strcmp(word, node->word) < 0) {
        return searchBST(node->left, word);
    } else {
        return searchBST(node->right, word);
    }
}

// -------------- UTILS ----------------

int counter(const char * wordRef, const char wordP[], int pos, int k) {
    int n = 0, c = 0, d = 0;
    for (int i = k; i >= 0; i--) {
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
        return d;
    } else {
        return -1;
    }
}

// TODO: speed this up
void updateMinCardinality(char *ref_word, char *new_word, char const *result_word, constraintCell *cArr) {
    int counter, val;
    bool incr_flag;
    for (int i = 0; i < strlen(ref_word); i++) {
        incr_flag = true;
        counter = 0;
        val = constraintMapper(new_word[i]);
        for (int j = 0; j < strlen(ref_word); j++) {
            if (new_word[j] == new_word[i]) {
                if (result_word[j] == '/' && cArr[val].presence > 0) {
                    cArr[val].exact_number = true;
                } else if (result_word[j] == '+') {
                    counter++;
                } else {
                    if (incr_flag) {
                        counter++;
                        incr_flag = false;
                    }
                }
            }
        }
        if (counter > cArr[val].cardinality) {
            cArr[val].cardinality = counter;
        }
    }
}

bool compare(char *ref_word, char *new_word, char *result_word, char *certain_word, char *presence_needed, constraintCell *cArr, int length) {
    bool exists, win_flag = true;
    constraintCell tempArrCell;
    for (int i = 0; i < length; i++) {
        tempArrCell = cArr[constraintMapper(new_word[i])];
        if (new_word[i] == ref_word[i]) {
            result_word[i] = '+';
            certain_word[i] = new_word[i];
            tempArrCell.presence[i] = 1;
        } else {
            exists = false;
            for (int j = 0; j < length; j++) {
                if (new_word[i] == ref_word[j]) {
                    exists = true;
                }
            }
            if (!exists || counter(ref_word, new_word, i, length) >= 0) {
                result_word[i] = '/';
                tempArrCell.presence[i] = -1;
                if (tempArrCell.cardinality != -1) {
                    tempArrCell.exact_number = true;
                } else {
                    tempArrCell.cardinality = -2;
                }
            } else {
                result_word[i] = '|';
                if (strchr(presence_needed, new_word[i]) == NULL) {
                    int z = 0;
                    while (presence_needed[z] != '*' && z < length) {
                        z++;
                    }
                    presence_needed[z] = new_word[i];
                }
                tempArrCell.presence[i] = -1;
            }
            win_flag = false;
        }
    }
    updateMinCardinality(ref_word, new_word, result_word, cArr);
    return win_flag;
}

int charCounter(const char * word, char letter, int k) {
    int temp_count = 0;
    for (int i = 0; i < k; i++) {
        if (word[i] == letter) {
            temp_count++;
        }
    }
    return temp_count;
}

void banwords(struct nodeLIST ** root, const char * certain_word, const char * presence_needed, constraintCell * constraints, int k) {
    int charCount;
    struct nodeLIST *temp = *root, *prec = NULL;
    constraintCell tempConstraint;
    bool to_ban_flag;
    while (temp != NULL) {
        to_ban_flag = false;
        for (int i = 0; i < k && !to_ban_flag; i++) {
            if (certain_word[i] != '*') {
                if (certain_word[i] != temp->word[i]) {
                    to_ban_flag = true;
                }
            }
            if (!to_ban_flag) {
                if (presence_needed[i] != '*') {
                    if (strchr(temp->word, presence_needed[i]) == NULL) {
                        to_ban_flag = true;
                    }
                }
            }
        }
        for (int i = 0; i < k && !to_ban_flag; i++) {
            tempConstraint = constraints[constraintMapper(temp->word[i])];
            if (tempConstraint.cardinality == -2) {
                to_ban_flag = true;
            } else {
                if (tempConstraint.presence[i] == -1) {
                    to_ban_flag = true;
                }
                if (!to_ban_flag) {
                    charCount = charCounter(temp->word, temp->word[i], k);
                    if (tempConstraint.exact_number) {
                        if (tempConstraint.cardinality != charCount) {
                            to_ban_flag = true;
                        }
                    } else {
                        if (tempConstraint.cardinality > charCount) {
                            to_ban_flag = true;
                        }
                    }
                }
            }
        }
        //delete temp node
        if (to_ban_flag) {
            if (*root == temp) {
                *root = (*root)->next;
                free(temp);
                temp = *root;
            } else {
                prec->next = temp->next;
                free(temp);
                temp = prec->next;
            }
            quantity--;
        } else {
            prec = temp;
            temp = temp->next;
        }
    }
}

int getWord(char *temp_word, int length) {
    int i = 0;
    char temp;
    do {
        temp = (char) getchar_unlocked();
        if (temp != EOF) {
            temp_word[i] = temp;
            i++;
        } else {
            return -1;
        }
    } while (temp != '\n');

    if (temp_word[0] == '+') {
        temp = temp_word[12];
        // memset(temp_word,0,strlen(temp_word));
        switch (temp) {
            case 't': // +nuova_partita
                return 1;
            case 'r': // +stampa_filtrate
                return 2;
            case 'n': // +inserisci_inizio
                return 3;
            case 'i': // +inserisci_fine
                return 4;
            default:
                return -1;
        }
    }
    temp_word[length] = '\0';
    return 0;
}

int main() {
    bool winner_flag, filtered_flag, new_insertion_flag, used_word_flag;
    int i, k, n, code, rc;
    char *temp_word, *reference_word, *result_word, *certain_word, *presences_needed;
    struct nodeBST* rootBST = NULL;
    struct nodeLIST* rootLIST = NULL;
    struct nodeLIST* headLIST = NULL;
    constraintCell constraints[CONSTQUANTITY];

    // acquire length:
    // TODO: might be needed to change this, since it acquires only one-digit numbers
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

    // define reference word:
    reference_word = (char *) malloc(sizeof(char) * k);
    result_word = (char *) malloc(sizeof(char) * k);
    certain_word = (char *) malloc(sizeof(char) * k);
    presences_needed = (char *) malloc(sizeof(char) * k);

    new_insertion_flag = false;

    // new game begins
    do {
        getWord(reference_word, k);

        // acquire tries quantity
        rc = scanf("%d\n", &n);
        rc = rc + 1;

        // generate new list
        quantity = 0;
        newList(rootBST, &rootLIST, &headLIST);

        // set certain_word & presences_needed to '*'
        memset(certain_word, '*', k);
        memset(presences_needed, '*', k);

        i = 0;
        winner_flag = filtered_flag = false;
        used_word_flag = true;

        do {
            if (used_word_flag) {
                temp_word = (char *) malloc(sizeof(char) * k);
                used_word_flag = false;
            }
            code = getWord(temp_word, k);
            if (code == 0) {
                if (filtered_flag) {
                    quantity++;
                    insertNodeBST(rootBST, temp_word);
                    insertNode(&rootLIST, temp_word);
                    used_word_flag = true;
                } else {
                    if (searchBST(rootBST, temp_word) != NULL) {
                        new_insertion_flag = false;
                        winner_flag = compare(reference_word, temp_word, result_word, certain_word, presences_needed, constraints, k);
                        banwords(&rootLIST, certain_word, presences_needed, constraints, k);
                        if (!winner_flag) {
                            printf("%s\n%d\n", result_word, quantity);
                        } else {
                            printf("ok\n");
                        }
                        i++;
                    } else {
                        printf("not_exists\n");
                    }
                }
            } else if (code == 2) {
                if (new_insertion_flag) {
                    new_insertion_flag = false;
                    banwords(&rootLIST, certain_word, presences_needed, constraints, k);
                }
                printList(rootLIST);
            } else if (code == 3) {
                filtered_flag = true;
            } else if (code == 4) {
                filtered_flag = false;
                new_insertion_flag = true;
            }
        } while (i < n && !winner_flag && code != -1);

        if (!winner_flag && code != -1) {
            printf("ko\n");
        }

        used_word_flag = true;

        do {
            if (used_word_flag) {
                temp_word = (char *) malloc(sizeof(char) * k);
                used_word_flag = false;
            }
            code = getWord(temp_word, k);
            switch (code) {
                case 0:
                    insertNodeBST(rootBST, temp_word);
                    used_word_flag = true;
                    break;
                case 1:
                    resetList(&rootLIST);
                    resetConstraints(constraints, k, false);
                    break;
                case 2:
                    break;
                case 3:
                    filtered_flag = true;
                    break;
                case 4:
                    filtered_flag = false;
                    new_insertion_flag = true;
                    break;
                default:
                    break;
            }
        } while (code != 1 && code != -1);
    } while (code != -1);
}
