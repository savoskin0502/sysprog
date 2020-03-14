#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

int countOfElements = 0;
int cur_elem = 0;

struct node
{
    int val;
    struct node *left;
    struct node *right;
    struct node *prev;
    int pointer;
    int level;
    char filename[247*sizeof(char)];
};
struct node* newNode(int x, int pointer, char filename[])
{
    struct node* node = (struct node*)malloc(sizeof(struct node));
    node->val = x;
    node->level = -1;
    node->left = NULL;
    node->right = NULL;
    node->prev = NULL;
    node->pointer = pointer;
    strcpy(node->filename, filename);
    return (node);
}

struct node* add(struct node *root, int x, int pointer, char filename[]){
    struct node *n = newNode(x, pointer, filename);
    if(root == NULL){
        root = n;
        root->level = 0;
    }else{
        if (x <= root->val){
            if(root->left == NULL){
                root->left = n;
                n->prev = root;
                root->left->level = root->level + 1;
                countOfElements = countOfElements + 1;
            }else{
                add(root->left, x, pointer, filename);
            }
        }else if (x > root->val){
            if(root->right == NULL){
                root->right = n;
                root->right->level = root->level + 1;
                n->prev = root;
                countOfElements = countOfElements + 1;
            }else{
                add(root->right, x, pointer, filename);
            }
        }
    }
    return (root);
}


int rightmost(struct node * root){
    if(root->right == NULL){
        return root->val;
    }
    return rightmost(root->right);
    
}

struct node* del(struct node *root, int x){
    if (root == NULL) return root;
    if (root->val == x){
        if(root->right == NULL && root->left == NULL){
            free(root); 
            return NULL;
        }
        if(root->right != NULL && root->left == NULL){
            struct node * temp = root->right;
            free(root); 
            return temp;
        }
        if(root->right == NULL && root->left != NULL){
            struct node * temp = root->left;
            free(root); 
            return temp;
        }
        root->val = rightmost(root->left);
        root->right = del(root->right, root->val); 
    }
    if(x <= root->val){
        root->left = del(root->left, x);
    }else{
        root->right = del(root->right, x);
    }
    return root;
}

void print(struct node * n){
    if(n == NULL){
        return;
    }
    printf("%d\n" ,n->pointer);
    print(n->left);
    print(n->right);
}

int el_num(){
    return countOfElements;
}

int left_min(struct node *n, struct node *st){
    if(n->left==NULL){
        st[cur_elem] = *n;
        cur_elem = cur_elem + 1;
        return n->val;
    }
    return left_min(n->left, st);
}

int findfileSize(char f_n[]) {
    FILE* fp = fopen(f_n, "r");
    if (fp == NULL){
        printf("File Not Found!\n");
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    int res = ftell(fp);
    fclose(fp);
    return res;
}

int binarySearch(struct node *arr, int l, int r, int x, int group_choise) 
{ 
    if (r >= l) { 
        int mid = l + (r - l) / 2; 
        if (arr[mid].val == x) 
            return mid; 
        if (arr[mid].val > x) 
            return binarySearch(arr, l, mid - 1, x, group_choise); 
        return binarySearch(arr, mid + 1, r, x, group_choise); 
    } 
    return -1; 
}


int main(){
    struct node * root = NULL;
    DIR *dir;
    struct dirent *entry;
    struct dirent **entry1;
    dir = opendir(".");
    int indexed_pointer = 0;

    int group_choise;
    printf("\nPlease enter in which order to sort (1/0|alphabet/size)\n");
    scanf("%d",&group_choise);
    
    char group_action[40];
    
    if (dir)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if (*entry->d_name != '.'){
                int size;
                if (group_choise){
                    size = entry->d_name[0];
                    strcpy(group_action, " Sort in Alphabet order; ");
                }else{
                    size = findfileSize(entry->d_name);
                    strcpy(group_action, " Sort by Size; ");
                }
                printf("Size of %s: ", entry->d_name);
                root = add(root, size, indexed_pointer, entry->d_name);
                indexed_pointer = indexed_pointer + 1;
                printf("%d\n", size);
            }
        }
        scandir(".", &entry1, 0, alphasort);
        closedir(dir);
    }
    struct node *st;
    st = (struct node*)malloc((el_num())*sizeof(struct node));
    struct node *original = root;
    while(root->left!=NULL){
        del(root,left_min(root->left, st));
    }
    while(root->right!=NULL){
        del(root,left_min(root->right, st));
    }
    struct node *st_copy;
    st_copy = (struct node*)malloc((el_num()+1)*sizeof(struct node));
    int pointer_helper = 0;
    int added = 0;

    for (int l = 0; l < el_num() + 1; l = l + 1){
        if(st[l].filename == original->filename){
            added = 1;
        }
    }
    for (int l = 0; l < el_num() + 1; l = l + 1){
        if((st[l].val > original->val) & (added==0)){
            st_copy[pointer_helper] = *original;
            pointer_helper = pointer_helper + 1;
            st_copy[pointer_helper] = st[l];
            st_copy[pointer_helper].pointer = pointer_helper;
            st_copy[pointer_helper-1].pointer = pointer_helper-1;
            added = 1;
        }else{
            st_copy[pointer_helper] = st[l];
            st_copy[pointer_helper].pointer = pointer_helper;
            pointer_helper = pointer_helper + 1;
        }
    }


    int found = 0;
    int result = -1;
    int xx;
    if (!group_choise){
        printf("\nFile size: \n");
        scanf("%d",&xx);
        result = binarySearch(st_copy, 0, el_num() - 1, xx, group_choise); 
        if(result>=0){
            printf("The element is present at index %d\n", result);
            found = 1;
            
        }else{
            printf("The element is not present in array\n");
        }
    }

    time_t v = time(NULL);
    char tm[20*sizeof(char)];
    strftime(tm, 50, "%Y-%m-%d %H:%M:%S", localtime(&v));
    char binary_searched[100*sizeof(char)] = "";
    if (found){
        if (result >-1){
            snprintf(binary_searched, sizeof(binary_searched), "%s%s%s%s%s%d%s%d", tm ,group_action, "Filename ", st_copy[result].filename," with size ",st_copy[result].val," Found at index ", result);
        }
        
    }else{
        snprintf(binary_searched, sizeof(binary_searched), "%s%s", tm, group_action);
    }
    FILE *fp;
    fp = fopen("logs.txt", "a");
    fprintf(fp, "%s\n", binary_searched);
    fclose(fp);

    for (int i = 0; i < el_num(); i = i+1){
        printf("FILENAME:  %s\n", st_copy[i].filename);
    }

    return 0;
}