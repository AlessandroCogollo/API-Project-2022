#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// ------------- GLOBAL VARIABLES ----------------

// TODO: Change cstr and make it local!
int k, * visited, comp_word;
char * result, * secure_word;
bool exac_flag = false;
bool * is_present;
bool * not_present;
bool winner_flag = false;
struct constraint* cstr;
struct node* root;
struct node* op_root;

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
    bool applied;
    bool *is_present;
    bool *not_present;
    int min_number;
    int exact_number;
    int color;
    struct constraint *left, *right, *parent;
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
    //    instead of this --> node = malloc(sizeof(node))
    //      node->string = malloc(sizeof(char)*length)
    //    do this --> malloc(sizeof(node)+sizeof(char)*length)

    struct constraint* temp = (struct constraint*)malloc(sizeof(struct constraint));
    temp->symbol = symbol;
    temp->belongs = b;
    temp->applied = false;
    if (b == true) {
        // temp->is_present = (bool *) malloc(sizeof(bool) * k);
        temp->not_present = (bool *) malloc(sizeof(bool) * k);
        for (int i = 0; i < k; i++) {
            // temp->is_present[i] = false;
            temp->not_present[i] = false;
        }
    } else {
        temp->is_present = NULL;
        temp->not_present = NULL;
    }
    temp->min_number = min;
    temp->exact_number = exac;
    temp->color = 1;
    temp->left = temp->right = temp->parent = NULL;
    return temp;
}

void constraintRightRotate(struct constraint * temp) {
    struct constraint* left = temp->left;
    temp->left = left->right;
    if (temp->left)
        temp->left->parent = temp;
    left->parent = temp->parent;
    if (!temp->parent)
        cstr = left;
    else if (temp == temp->parent->left)
        temp->parent->left = left;
    else
        temp->parent->right = left;
    left->right = temp;
    temp->parent = left;
}

void constraintLeftRotate(struct constraint* temp) {
    struct constraint* right = temp->right;
    temp->right = right->left;
    if (temp->right)
        temp->right->parent= temp;
    right->parent = temp->parent;
    if (!temp->parent)
        cstr = right;
    else if (temp == temp->parent->left)
        temp->parent->left = right;
    else
        temp->parent->right = right;
    right->left = temp;
    temp->parent = right;
}

struct constraint* insertConstraint(struct constraint* node, char symbol, bool b, int min, int exac) {
    if (node == NULL)
        return newConstraint(symbol, b, min, exac);
    if (symbol < node->symbol) {
        node->left = insertConstraint(node->left, symbol, b, min, exac);
        node->left->parent = node;
    }
    else if (symbol > node->symbol) {
        node->right = insertConstraint(node->right, symbol, b, min, exac);
        node->right->parent = node;
    }
    return node;
}

void constraintFixup(struct constraint* node, struct constraint* pt) {
    struct constraint* parent_pt = NULL;
    struct constraint* grand_parent_pt = NULL;

    while ((pt != node) && (pt->color != 0) && (pt->parent->color == 1)) {
        parent_pt = pt->parent;
        grand_parent_pt = pt->parent->parent;

        if (parent_pt == grand_parent_pt->left) {
            struct constraint* uncle_pt = grand_parent_pt->right;

            if (uncle_pt != NULL && uncle_pt->color == 1) {
                grand_parent_pt->color = 1;
                parent_pt->color = 0;
                uncle_pt->color = 0;
                pt = grand_parent_pt;
            } else {
                if (pt == parent_pt->right) {
                    constraintLeftRotate(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }
                constraintRightRotate(grand_parent_pt);
                int t = parent_pt->color;
                parent_pt->color = grand_parent_pt->color;
                grand_parent_pt->color = t;
                pt = parent_pt;
            }
        } else {
            struct constraint* uncle_pt = grand_parent_pt->left;
            if ((uncle_pt != NULL) && (uncle_pt->color == 1)) {
                grand_parent_pt->color = 1;
                parent_pt->color = 0;
                uncle_pt->color = 0;
                pt = grand_parent_pt;
            } else {
                if (pt == parent_pt->left) {
                    constraintRightRotate(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }
                constraintLeftRotate(grand_parent_pt);
                int t = parent_pt->color;
                parent_pt->color = grand_parent_pt->color;
                grand_parent_pt->color = t;
                pt = parent_pt;
            }
        }
    }
    node->color = 0;
}

// --------------- BST structure -----------------

struct node {
    char *word;
    bool compatible;
    bool new;
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
    temp->new = true;
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

void freeBST(struct constraint* node) {
    if (node == NULL) return;
    freeBST(node->left);
    freeBST(node->right);
    if (node->belongs) {
        // free(node->is_present);
        free(node->not_present);
    }
    free(node);
}

struct node* minValueNode(struct node* node) {
    struct node* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

struct node* deleteNode(struct node* node, char * word) {
    if (node == NULL)
        return node;
    if (strcmp(word, node->word) > 0)
        node->left = deleteNode(node->left, word);
    else if (strcmp(word, node->word) < 0)
        node->right = deleteNode(node->right, word);
    else {
        if (node->left == NULL && node->right == NULL)
            return NULL;
        else if (node->left == NULL) {
            struct node* temp = node->right;
            free(node);
            return temp;
        }
        else if (node->right == NULL) {
            struct node* temp = node->left;
            free(node);
            return temp;
        }
        struct node* temp = minValueNode(node->right);
        strcpy(node->word, temp->word);
        node->right = deleteNode(node->right, temp->word);
    }
    return node;
}

bool checkComp(struct node *node, struct constraint * constraintNode, bool flag) {
    if (!flag) {
        if (constraintNode != NULL && !constraintNode->applied) {
            if (constraintNode->belongs) {
                int tmp_count = 0;
                // if symbol is part of the word
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
                // if symbol doesn't belong to the word
                if (strchr(node->word, constraintNode->symbol) != NULL) {
                    node->compatible = false;
                    return true;
                }
            }
            if (node->compatible) {
                checkComp(node, constraintNode->left, flag);
                checkComp(node, constraintNode->right, flag);
            }
        }
    } else {
        if (constraintNode != NULL) {
            if (constraintNode->belongs) {
                int tmp_count = 0;
                // if symbol is part of the word
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
                // if symbol doesn't belong to the word
                if (strchr(node->word, constraintNode->symbol) != NULL) {
                    node->compatible = false;
                    return true;
                }
            }
            if (node->compatible) {
                checkComp(node, constraintNode->left, flag);
                checkComp(node, constraintNode->right, flag);
            }
        }
    }
    return false;
}

void banWord(struct node* node, struct constraint* constraintNode, bool flag) {
    if (node != NULL) {
        if (flag) {
            banWord(node->left, constraintNode, flag);
            if (node->compatible && node->new) {
                for (int i = 0; i < k && node->compatible; i++) {
                    if (node->word[i] != secure_word[i] && secure_word[i] != '$') {
                        node->compatible = false;
                    }
                }
                if (node->compatible) {
                    checkComp(node, constraintNode, flag);
                }
                if (node->compatible) {
                    comp_word++;
                }
                node->new = false;
            }
            banWord(node->right, constraintNode, flag);
        } else {
            banWord(node->left, constraintNode, flag);
            if (node->compatible) {
                for (int i = 0; i < k && node->compatible; i++) {
                    if (node->word[i] != secure_word[i] && secure_word[i] != '$') {
                        node->compatible = false;
                    }
                }
                if (node->compatible) {
                    checkComp(node, constraintNode, flag);
                }
                if (node->compatible) {
                    comp_word++;
                }
                node->new = false;
            }
            banWord(node->right, constraintNode, flag);
        }
    }
}

void delete (struct node * node) {
    if (node != NULL) {
        delete(node->left);
        if (!node->compatible) {
            op_root = deleteNode(op_root, node->word);
        }
        delete(node->right);
    }
}

void setAllApplied (struct constraint * node) {
    if (node != NULL) {
        setAllApplied(node->left);
        if (!node->applied) {
            node->applied = true;
        }
        setAllApplied(node->right);
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
                    secure_word[l] = new[l];
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
            constraintNode = insertConstraint(cstr, symbol, belongs, min_number, exact_number);
            constraintFixup(cstr, constraintNode);
        }
    }
    constraintNode = isPresent(cstr, symbol);
    if (constraintNode->belongs) {
        for (int i = 0; i < k; i++) {
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
    constraintNode->applied = false;
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
            // is_present[j] = false;
            not_present[j] = false;
        }
        if (reference[i] == new[i]) {
            result[i] = '+';
            belongs_flag = true;
            is_present[i] = true;
            secure_word[i] = new[i];
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

struct node* cloneBinaryTree(struct node* node){
    if(node == NULL)
        return NULL;
    /* create a copy of root node */
    struct node * tmp_node = newNode(node->word);
    /* Recursively create clone of left and right sub tree */
    tmp_node->left = cloneBinaryTree(node->left);
    tmp_node->right = cloneBinaryTree(node->right);
    /* Return root of cloned tree */
    return tmp_node;
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

        op_root = cloneBinaryTree(root);

        memset(secure_word, '$', k);

        word_count = 0;
        bool filtered_flag = false;
        bool print_flag = false;
        winner_flag = false;

        // read reference word
        ref_word = (char *) malloc(sizeof(char) * (k+1));
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
                printFiltered(op_root);
            } else if (return_code == 1 && !filtered_flag) {
                filtered_flag = true;
            } else if (filtered_flag == true) {
                if (return_code == 2) {
                    comp_word = 0;
                    // applica solo alle nuove parole
                    banWord(op_root, cstr, true);
                    setAllApplied(cstr);
                    delete(op_root);
                    filtered_flag = false;
                } else {
                    insert(root, new_word);
                    insert(op_root, new_word);
                }
            } else {
                if (search(root, new_word) != NULL) {
                    print_flag = compare(ref_word, new_word);
                    // applica solo i nuovi vincoli
                    if (print_flag) {
                        comp_word = 0;
                        banWord(op_root, cstr, false);
                        // delete(op_root);
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

        freeBST(cstr);
        freeWord(op_root);
        free(visited);
        free(result);
        free(is_present);
        free(not_present);
        op_root = NULL;
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
        comp_word = 0;

    } while (return_code != 5);
}