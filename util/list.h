#include <stdlib.h>
#include <stdio.h>

//Basic list library 

struct double_ll{
    struct Node* head, *tail;
};

struct Node{
    void *value;
    struct Node* next, *prev;
};

struct double_ll* ll_create(void);
void ll_add(struct double_ll *ll, void *value);
void* ll_remove_first(struct double_ll *ll);
void ll_remove(struct double_ll *ll, struct Node* remove_node);
void ll_destroy(struct double_ll *ll);
int ll_length(struct double_ll *ll);
int ll_contains(struct double_ll *ll, void *value);

//Create a linked list.
struct double_ll* ll_create(void){
    struct double_ll* newList = malloc(sizeof(struct double_ll));
    newList->head = NULL;
    newList->tail = NULL;
    return newList;
}

//Add a value to the linked list.
void ll_add(struct double_ll *ll, void *value){
    struct Node *newNode = malloc(sizeof(struct Node));
    newNode->value = value;
    newNode->next = ll->head;
    newNode->prev = ll->tail;
    if(ll->head == NULL){ ll->head = newNode; }
    else{
        ll->tail->next = newNode;
        ll->head->prev = newNode;
    }
    ll->tail = newNode;
}

//remove_nodeove the first value from the linked list.
void* ll_remove_first(struct double_ll *ll){
    if(ll->head==NULL){ return NULL; }
    else{
        void* value = ll->head->value;
        if(ll->head==ll->tail){
            free(ll->head);
            ll->head = NULL;
            ll->tail = NULL;
        }
        else
        {
            free(ll->head);
            ll->head = ll->head->next;
            ll->tail->next = ll->head;
            ll->head->prev = ll->tail;
        }
        return value;
    }
}

//remove_node a certain node (denoted by parameter remove_node) from the linked list.
void ll_remove(struct double_ll *ll, struct Node* remove_node){
    struct Node* move = ll->head;
    while(move!=remove_node){
        if(move==ll->tail){ return; }
        move=move->next;
    }
    if(ll->head == remove_node && ll->tail == remove_node){
        ll->head = NULL;
        ll->tail = NULL;
        return;
    }
    if(ll->head == remove_node){ ll->head = remove_node->next; }
    if(ll->tail == remove_node){ ll->tail = remove_node->prev; }
    remove_node->prev->next = remove_node->next;
    remove_node->next->prev = remove_node->prev;
}

//Destroy a linked list.
void ll_destroy(struct double_ll *ll){
    while(!ll_remove_first(ll));
    free(ll);
}

//Calculates the length of an linked list.
int ll_length(struct double_ll *ll){
    if(ll->head==NULL){ return 0; }
    else{
        struct Node* move;
        move = ll->head;
        int count = 1;
        while(move->next!=ll->head){
            move = move->next;
            count++;
        }
        return count;
    }
}

//Searches for a certain value (denoted by parameter value) in the linked list.
int ll_contains(struct double_ll *ll, void *value){
    if(ll->head==NULL){ return 0; }
    else{
        struct Node* move;
        move = ll->head;
        int count = 1;
        while((move->value!=value) && move->next!=ll->head){
            move = move->next;
            count++;
        }
        if(move->value!=value){ count = 0; }
        return count;
    }
}

//Prints the linked list contents.
void ll_print(struct double_ll *ll){
    struct Node* move = ll->head;
    printf("List contents: \n");
    int ctr;
    while(move!=NULL){
        ctr++;
        printf("Obj#%d Location: %d\n",ctr ,move->value);
        if(move==ll->tail) break;
        move = move->next;
    }
    printf("End list\n");
}
