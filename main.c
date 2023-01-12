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
    do {
        printf("%s\n", temp->word);
        temp = temp->next;
    } while (temp != NULL);
}

struct nodeLIST * searchList (struct nodeLIST * node, char * word) {
    struct nodeLIST *temp = node;
    int ret_code;
    do {
        ret_code = strcmp(temp->word, word);
        if (ret_code == 0) {
            return temp;
        }
        temp = temp->next;
    } while (temp != NULL && ret_code < 0);
    return NULL;
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

// -------------- UTILS ----------------

bool counter(const char * wordRef, const char wordP[], int pos, int k) {
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
        return true;
    } else {
        return false;
    }
}

bool compare(char *ref_word, char *new_word, char *result_word, constraintCell *cArr, int length) {
    int val;
    bool exists, win_flag = true;
    for (int i = 0; i < length; i++) {
        val = constraintMapper(new_word[i]);
        if (new_word[i] == ref_word[i]) {
            result_word[i] = '+';
            cArr[val].presence[i] = 1;
            if (cArr[val].cardinality < 0) {
                cArr[val].cardinality = 1;
            } else {
                for (int j = 0; j < length; j++) {
                    if (cArr[val].presence[j] > 1) {
                        cArr[val].cardinality++;
                    }
                }
            }
        } else {
            exists = false;
            for (int j = 0; j < length; j++) {
                if (new_word[i] == ref_word[j]) {
                    exists = true;
                }
            }
            if (!exists || counter(ref_word, new_word, i, length)) {
                result_word[i] = '/';
                if (cArr[constraintMapper(new_word[i])].cardinality != -1) {
                    cArr[val].exact_number = true;
                } else {
                    cArr[val].cardinality = -2;
                }
            } else {
                result_word[i] = '|';
                cArr[val].presence[i] = -1;
            }
            win_flag = false;
        }
    }
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

void banwords(struct nodeLIST ** root, constraintCell * constraints, int k) {
    int charCount;
    struct nodeLIST *temp = *root, *prec = *root;
    constraintCell tempConstraint;
    bool to_ban_flag;
    do {
        to_ban_flag = false;
        for (int i = 0; i < k && !to_ban_flag; i++) {
            tempConstraint = constraints[constraintMapper(temp->word[i])];
            if (tempConstraint.cardinality == -2) {
                // char doesn't belong to the word
                to_ban_flag = true;
            } else {
                // char belongs to the word
                if (tempConstraint.presence[i] == -1) {
                    // char belongs, but is not in [i] position
                    to_ban_flag = true;
                }
                charCount = charCounter(temp->word, temp->word[i], k);
                if (tempConstraint.exact_number) {
                    // char belongs, but its exact number differs
                    if (tempConstraint.cardinality != charCount) {
                        to_ban_flag = true;
                    }
                } else {
                    // char belongs, but its cardinality is different from min number
                    if (tempConstraint.cardinality > 0 && tempConstraint.cardinality < charCount) {
                        to_ban_flag = true;
                    }
                }
            }
        }
        if (to_ban_flag) {
            // delete node
            if (temp == *root) {
                *root = temp->next;
                prec = *root;
            } else {
                prec->next = temp->next;
            }
            free(temp);
            quantity--;
        }
        temp = prec->next->next;
        prec = prec->next;
    } while (temp->next != NULL);
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
        switch (temp_word[12]) {
            case 't': // +nuova_partita
                memset(temp_word,0,strlen(temp_word));
                return 1;
            case 'r': // +stampa_filtrate
                memset(temp_word,0,strlen(temp_word));
                return 2;
            case 'n': // +inserisci_inizio
                memset(temp_word,0,strlen(temp_word));
                return 3;
            case 'i': // +inserisci_fine
                memset(temp_word,0,strlen(temp_word));
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
        getchar_unlocked();

        // generate new list
        quantity = 0;
        newList(rootBST, &rootLIST, &headLIST);

        i = 0;
        winner_flag = filtered_flag = false;

        do {
            temp_word = (char *) malloc(sizeof(char) * k);
            code = getWord(temp_word, k);
            switch (code) {
                case 0:
                    if (filtered_flag) {
                        quantity++;
                        insertNodeBST(rootBST, temp_word);
                        insertNode(&rootLIST, temp_word);
                    } else {
                        if (searchList(rootLIST, temp_word) != NULL) {
                            winner_flag = compare(reference_word, temp_word, result_word, constraints, k);
                            banwords(&rootLIST, constraints, k);
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
                    break;
                case 1:
                    break;
                case 2:
                    printList(rootLIST);
                    break;
                case 3:
                    filtered_flag = true;
                    break;
                case 4:
                    filtered_flag = false;
                    break;
                default:
                    break;
            }
        } while (i < n && !winner_flag && code != -1);

        if (!winner_flag) {
            printf("ko");
        }

        do {
            temp_word = (char *) malloc(sizeof(char) * k);
            code = getWord(temp_word, k);
            switch (code) {
                case 0:
                    insertNodeBST(rootBST, temp_word);
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
                    break;
                default:
                    break;
            }
        } while (code != 1 && code != -1);
    } while (code != -1);
}
