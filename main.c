#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

// --------------- BST structure -----------------

struct node {
    char key;
    struct node *left, *right;
};

// A utility function to create a new BST node
struct node* newNode(int item)
{
    struct node* temp = (struct node*)malloc(sizeof(struct node));
    temp->key = item;
    temp->left = temp->right = NULL;
    return temp;
}

void inorder(struct node* root)
{
    if (root != NULL) {
        inorder(root->left);
        printf("%d \n", root->key);
        inorder(root->right);
    }
}

struct node* insert(struct node* node, int key)
{
    /* If the tree is empty, return a new node */
    if (node == NULL)
        return newNode(key);

    /* Otherwise, recur down the tree */
    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);

    /* return the (unchanged) node pointer */
    return node;
}

// C function to search a given key in a given BST
struct node* search(struct node* root, int key) {
    // Base Cases: root is null or key is present at root
    if (root == NULL || root->key == key)
        return root;

    // Key is greater than root's key
    if (root->key < key)
        return search(root->right, key);

    // Key is smaller than root's key
    return search(root->left, key);
}

// -----------------------------------------------

bool counter(char wordRef[], const char wordP[], int pos) {
    int n = 0, c = 0, d = 0;
    for (int i = 0; i < (int) strlen(wordRef); i++) {
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
    // printf("n = %d, c = %d, d = %d\n", n, c, d);
    if (d >= (n-c)) {
        return true;
    }
    return false;
}

void compare(char refWord[], char newWord[]) {
    char *wordRes = (char*) malloc(sizeof(char)* strlen(refWord));
    for (int i = 0; i < (int) strlen(refWord); i++) {
        if (refWord[i] == newWord[i]) {
            wordRes[i] = '+';
        } else {
            if (!strpbrk(newWord, &refWord[i]) || counter(refWord, newWord, i)) {
                wordRes[i] = '/';
            } else {
                wordRes[i] = '|';
            }
        }
    }
    printf("%s", wordRes);
}

int main() {
    int k, n, count;
    char cmd_new_game[] = "+nuova_partita",
         cmd_print_filtered[] = "+stampa_filtrate",
         cmd_insert_begin[] = "+inserisci_inizio",
         cmd_insert_end[] = "+inserisci_fine";
    char *new_word, *ref_word, tmp_letter;

    // read word length
    k = (int) getchar_unlocked() - 48;

    // read admissible words
    do {
        count = 0;
        new_word = (char *) malloc(sizeof(char)*k);
        do {
            tmp_letter = (char) getchar_unlocked();
            if (tmp_letter != 10) {
                new_word[count] = tmp_letter;
                count++;
            }
        } while (tmp_letter != 10);
        if (strcmp(new_word, cmd_new_game) != 0 && strlen(new_word) == k) {
            // insert word in a "dictionary like" structure
        }
    } while (strcmp(new_word, cmd_new_game) != 0);

    // read reference word
    count = 0;
    ref_word = (char *) malloc(sizeof(char)*k);
    do {
        tmp_letter = (char) getchar_unlocked();
        if (tmp_letter != 10) {
            ref_word[count] = tmp_letter;
            count++;
        }
    } while (tmp_letter != 10);

    // read number n of words
    n = (int) getchar_unlocked() - 48;
    getchar_unlocked();

    // read words sequence
    for (int i = 0; i < n; i++) {
        count = 0;
        new_word = (char *) malloc(sizeof(char)*k);
        do {
            tmp_letter = (char) getchar_unlocked();
            if (tmp_letter != 10) {
                new_word[count] = tmp_letter;
                count++;
            }
        } while (tmp_letter != 10);
        compare(ref_word, new_word);
    }
}