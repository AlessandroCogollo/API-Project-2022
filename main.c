#define CONSTQUANTITY 64

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// TODO: allocate whole pages to speed up
// TODO: remove global variables

int quantity, *modified_constraints;
bool * visited;

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
    bool good;
    struct nodeLIST *next;
};

struct nodeLIST * newNodeList(char *word) {
    struct nodeLIST * new_node;
    new_node = (struct nodeLIST *) malloc (sizeof(struct nodeLIST));
    new_node->good = true;
    new_node->word = word;
    new_node->next = NULL;
    return new_node;
}

void printList (struct nodeLIST * node) {
    struct nodeLIST *temp = node;
    while (temp != NULL) {
        if (temp->good) {
            printf("%s\n", temp->word);
        }
        temp = temp->next;
    }
}

// TODO: change this below, taken from the internet

void insertNode(struct nodeLIST ** root, struct nodeLIST * temp) {
	if (*root == NULL || strcmp((*root)->word, temp->word) > 0) {
		temp->next = *root;
		*root = temp;
		return;
	}
    struct nodeLIST* current = *root;
	while (current->next != NULL && strcmp(current->next->word, temp->word) < 0)
		current = current->next;

	temp->next = current->next;
	current->next = temp;
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
    int constraintValue;
    bool exists, end_flag, win_flag = true;
    constraintCell tempArrCell;
    // TODO : modified constraints might be speeded up by setting a value only when actually modified
    for (int i = 0; i < length; i++) {
        modified_constraints[i] = -1;
    }
    for (int i = 0; i < length; i++) {
        constraintValue = constraintMapper(new_word[i]);
        // ---- SAVE MODIFIED CONSTRAINTS ----
        end_flag = false;
        for (int j = 0; j < length && !end_flag; j++) {
            if (modified_constraints[j] == -1 || modified_constraints[j] == constraintValue) {
                modified_constraints[j] = constraintValue;
                end_flag = true;
            }
        }
        // -----------------------------------
        tempArrCell = cArr[constraintValue];
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
                    if (strchr(certain_word, new_word[i]) == NULL) {
                        int z = 0;
                        while (presence_needed[z] != '*' && z < length) {
                            z++;
                        }
                        presence_needed[z] = new_word[i];
                    }
                }
                tempArrCell.presence[i] = -1;
            }
            win_flag = false;
        }
    }
    updateMinCardinality(ref_word, new_word, result_word, cArr);
    return win_flag;
}

bool fastCheck(char * word, char * cw, char * pn, int k) {
    bool found = false;
    for (int i = 0; i < k; i++) {
        if (cw[i] != '*') {
            if (cw[i] != word[i]) {
                return true;
            }
        }
        if (pn[i] != '*') {
            found = false;
            for (int j = 0; j < k && !found; j++) {
                if (word[j] == pn[i]) {
                    found = true;
                }
            }
            if (!found) {
                return true;
            }
        }
    }
    return false;
}

bool heavyCheckBan(constraintCell * constraints, char * temp, char *cw, char *pn, int k) {
    int charCount;
    constraintCell tempConstraint;

    if (fastCheck(temp, cw, pn, k)) {
        return true;
    }

    memset(visited, false, k);

    for (int i = 0; i < k; i++) {
        if (!visited[i]) {
            tempConstraint = constraints[constraintMapper(temp[i])];
            if (tempConstraint.cardinality == -2) {
                return true;
            }
            charCount = 0;
            for (int z = i; z < k; z++) {
                if (temp[z] == temp[i]) {
                    visited[z] = true;
                    if (tempConstraint.presence[z] == -1) {
                        return true;
                    }
                    charCount++;
                    if (tempConstraint.exact_number) {
                        if (tempConstraint.cardinality < charCount) {
                            return true;
                        }
                    }
                }
            }
            if (tempConstraint.cardinality > charCount) {
                return true;
            } else {
                if (tempConstraint.exact_number) {
                    if (tempConstraint.cardinality != charCount) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool lightCheckBan(constraintCell * constraints, char * word, char *cw, char *pn, int k) {
    if (fastCheck(word, cw, pn, k)) {
        return true;
    }

    int charCount;
    constraintCell tempConstraint;

    for (int i = 0; i < k && modified_constraints[i] != -1; i++) {
        tempConstraint = constraints[modified_constraints[i]];
        if (tempConstraint.cardinality == -2) {
            for (int j = 0; j < k; j++) {
                if (constraintMapper(word[j]) == modified_constraints[i]) {
                    return true;
                }
            }
        } else {
            charCount = 0;
            for (int z = 0; z < k; z++) {
                if (constraintMapper(word[z]) == modified_constraints[i]) {
                    charCount++;
                    if (tempConstraint.presence[z] == -1) {
                        return true;
                    }
                    if (tempConstraint.exact_number) {
                        if (tempConstraint.cardinality < charCount) {
                            return true;
                        }
                    }
                }
            }
            if (tempConstraint.cardinality > charCount) {
                return true;
            } else {
                if (tempConstraint.exact_number) {
                    if (tempConstraint.cardinality != charCount) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void banwords(struct nodeLIST ** root, char * cw, char * pn, constraintCell * constraints, int k, bool lightMode) {
    // cw: certain_word, pn: presence_needed
    struct nodeLIST *temp = *root, *prev = NULL;
    while (temp != NULL) {
        //delete temp node
        if (lightMode) {
            if (lightCheckBan(constraints, temp->word, cw, pn, k)) {
                if (*root == temp) {
                    *root = (*root)->next;
                    free(temp);
                    temp = *root;
                } else {
                    prev->next = temp->next;
                    free(temp);
                    temp = prev->next;
                }
                quantity--;
            } else {
                prev = temp;
                temp = temp->next;
            }
        } else {
            if (heavyCheckBan(constraints, temp->word, cw, pn, k)) {
                if (*root == temp) {
                    *root = (*root)->next;
                    free(temp);
                    temp = *root;
                } else {
                    prev->next = temp->next;
                    free(temp);
                    temp = prev->next;
                }
                quantity--;
            } else {
                prev = temp;
                temp = temp->next;
            }
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

// --------------- RB TREE -----------------

struct nodeRB {
    char *word;
    struct nodeRB *left, *right;
};

struct nodeRB * newNodeRB(char *word) {
    struct nodeRB * new_node;
    new_node = (struct nodeRB *) malloc (sizeof (struct nodeRB));
    new_node->word = word;
    new_node->left = new_node->right = NULL;
    return new_node;
}

struct nodeRB * insertNodeRB(struct nodeRB *node, char *word) {
    if (node == NULL)
        return newNodeRB(word);
    if (strcmp(word, node->word) < 0) {
        node->left = insertNodeRB(node->left, word);
        // node->left->father = node;
    } else {
        node->right = insertNodeRB(node->right, word);
        // node->right->father = node;
    }
    return node;
}

void newListFiltered(constraintCell * constraints, struct nodeRB *node, struct nodeLIST **root, struct nodeLIST **head, char *cw, char *pn, int k) {
    if (node != NULL) {
        newListFiltered(constraints, node->left, root, head, cw, pn, k);
        if (!heavyCheckBan(constraints, node->word, cw, pn, k)) {
            if (*root == NULL) {
                *root = newNodeList(node->word);
                *head = *root;
            } else {
                (*head)->next = newNodeList(node->word);
                *head = (*head)->next;
            }
            quantity++;
        }
        newListFiltered(constraints, node->right, root, head, cw, pn, k);
    }
}

void newList(constraintCell * constraints, struct nodeRB *node, struct nodeLIST **root, struct nodeLIST **head, char *cw, char *pn, int k) {
    if (node != NULL) {
        if (node->left != NULL) {
            newList(constraints, node->left, root, head, cw, pn, k);
        }
        if (!lightCheckBan(constraints, node->word, cw, pn, k)) {
            if (*root == NULL) {
                *root = newNodeList(node->word);
                *head = *root;
            } else {
                (*head)->next = newNodeList(node->word);
                *head = (*head)->next;
            }
            quantity++;
        }
        if (node->right != NULL) {
            newList(constraints, node->right, root, head, cw, pn, k);
        }
    }
}

struct nodeRB * searchRB(struct nodeRB *node, char *word) {
    if (node == NULL) {
        return node;
    }
    int ret_val = strcmp(word, node->word);
    if (ret_val < 0) {
        return searchRB(node->left, word);
    } else if (ret_val == 0){
        return node;
    } else {
        return searchRB(node->right, word);
    }
}

int main() {
    bool winner_flag, filtered_flag, new_insertion_flag, used_word_flag, first_time_flag, light_mode;
    int i, k, n, code, rc;
    char *temp_word, *reference_word, *result_word, *certain_word, *presences_needed;
    struct nodeRB* rootRB = NULL;
    struct nodeLIST* rootLIST = NULL;
    struct nodeLIST* headLIST = NULL;
    constraintCell constraints[CONSTQUANTITY];

    // acquire length:
    rc = scanf("%d\n", &k);
    rc = rc + 1;

    // acquire words:
    do {
        temp_word = (char *) malloc(sizeof(char) * k);
        code = getWord(temp_word, k);
        if (code == 0) {
            if (rootRB == NULL) {
                rootRB = insertNodeRB(rootRB, temp_word);
            } else {
                insertNodeRB(rootRB, temp_word);
            }
        }
    } while (code == 0);

    resetConstraints(constraints, k, true);

    // define reference word:
    reference_word = (char *) malloc(sizeof(char) * k);
    result_word = (char *) malloc(sizeof(char) * k);
    certain_word = (char *) malloc(sizeof(char) * k);
    presences_needed = (char *) malloc(sizeof(char) * k);
    modified_constraints = (int *) malloc(sizeof(int) * k);
    visited = (bool *) malloc (sizeof(bool) * k);

    new_insertion_flag = false;

    // generate new list

    // new game begins
    do {
        quantity = 0;

        getWord(reference_word, k);

        // acquire tries quantity
        rc = scanf("%d\n", &n);
        rc = rc + 1;

        // set certain_word & presences_needed to '*'
        for (int u = 0; u < k; u++) {
            certain_word[u] = '*';
            presences_needed[u] = '*';
        }

        i = 0;
        winner_flag = filtered_flag = light_mode = false;
        used_word_flag = first_time_flag = true;

        do {
            if (used_word_flag) {
                temp_word = (char *) malloc(sizeof(char) * k);
                // abilitate this -> fixRBTree(rootRB, rootRB);
                used_word_flag = false;
            }
            code = getWord(temp_word, k);
            if (code == 0) {
                if (filtered_flag) {
                    insertNodeRB(rootRB, temp_word);
                    if (!heavyCheckBan(constraints, temp_word, certain_word, presences_needed, k)) {
                        quantity++;
                        struct nodeLIST * tempNode = newNodeList(temp_word);
                        insertNode(&rootLIST, tempNode);
                    }
                    used_word_flag = true;
                    light_mode = false;
                } else {
                    if (searchRB(rootRB, temp_word) != NULL) {
                        new_insertion_flag = false;
                        winner_flag = compare(reference_word, temp_word, result_word, certain_word, presences_needed, constraints, k);
                        if (first_time_flag) {
                            // if its the first time of a new game, init list by removing words
                            newList(constraints, rootRB, &rootLIST, &headLIST, certain_word, presences_needed, k);
                            first_time_flag = false;
                        } else {
                            // else, simply ban words
                            banwords(&rootLIST, certain_word, presences_needed, constraints, k, false);
                            if (light_mode == false) {
                                light_mode = true;
                            }
                        }
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
                if (first_time_flag) {
                    // if its the first time of a new game, init list by removing words
                    newListFiltered(constraints, rootRB, &rootLIST, &headLIST, certain_word, presences_needed, k);
                    first_time_flag = false;
                } else {
                    if (new_insertion_flag) {
                        new_insertion_flag = false;
                        light_mode = false;
                        banwords(&rootLIST, certain_word, presences_needed, constraints, k, false);
                    }
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
                    insertNodeRB(rootRB, temp_word);
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
