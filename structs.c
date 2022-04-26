/*Chanany shok*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "error_handler.h"

/* Small macro to append new node at the end of a list, or initialize the head*/
#define APPEND_NEW { \
if(*head == NULL) {\
*head = tmpNew;\
return;\
}\
while(prev != NULL) {\
curr= prev;          \
prev = prev->next;\
}\
\
curr->next = tmpNew; \
}

/* Small macro to handle error in allocation*/
#define MALLOC_WITH_CHECK(tmpNew, type) {\
    tmpNew = (type *)malloc(sizeof(type));\
        if(tmpNew == NULL){\
            print_error("malloc failed, try calloc 5 times?erroe -2\n");\
         }\
    };

extern int gIc;


void add_to_symb_list(symbol_list **head, char *name, int address, Flag type){

    symbol_list *tmpNew, *prev , *curr;
    prev =  *head;
    MALLOC_WITH_CHECK(tmpNew, symbol_list);
    tmpNew->name =  (char *)malloc(sizeof(char)*(strlen(name)+1));
    strcpy(tmpNew->name, name);
    tmpNew->address = address;
    tmpNew->base = address -(address % 16);
    tmpNew->offSet = (address % 16);
  
    tmpNew->code =0;
    tmpNew->extrn =0;
    tmpNew->entry =0;
    tmpNew->data =0;
    switch(type){
        case(COMMAND):
            tmpNew->code = 1;
            break;
        case(STRING):
        case(DATA):
            tmpNew->data = 1;
            break;
        case(EXTERN):
            tmpNew->extrn =1;
            break;
    }
    tmpNew->next = NULL;

    /* small macro to append to end of list*/
    APPEND_NEW

}


void free_symb_list(symbol_list *head){
    symbol_list *cur, *next;

    if(head == NULL) return;

    for(cur =head; cur != NULL; cur = next){
        next = cur ->next;
        free(cur->name);
        free(cur);
    }
    head = NULL;

}



symbol_list * find_in_symb_list(symbol_list **head, char *name){
    symbol_list *ptr;
    ptr = *head;
    while(ptr != NULL){
        if((int)strlen(name) == (int)strlen(ptr->name)) {
            if (!(strcmp(ptr->name, name))) return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}


void add_to_code_list(code_list **head, code_list *bitFiled){
    code_list *tmpNew, *prev, *curr;

    prev = *head;
    MALLOC_WITH_CHECK(tmpNew, code_list);
    tmpNew->BitFiled.iter.T =bitFiled->BitFiled.iter.T;
    tmpNew->address = bitFiled->address;
    tmpNew->data_type = bitFiled ->data_type;
    tmpNew->next = NULL;

    /* small macro to append to end of list*/
    APPEND_NEW
}


void free_code_list(code_list *head){
    code_list *cur, *next;
    if(head == NULL) return;

    for(cur =head; cur != NULL; cur = next){
        next = cur ->next;
        free(cur);
    }
    head = NULL;

}

void add_to_list(generic_list **head, char *name, char * content, int address)

{
    generic_list *tmpNew, *prev, *curr;

    prev = *head;
    MALLOC_WITH_CHECK(tmpNew, generic_list);
    tmpNew->address = address;
    tmpNew->name =  (char *)malloc(sizeof(char)*(strlen(name)+1));
    tmpNew->content = (char *)malloc(sizeof(char)*(strlen(content)+1));
    strcpy(tmpNew->name, name);
    strcpy(tmpNew->content, content);

    tmpNew->next = NULL;
    /* small macro to append to end of list*/
    APPEND_NEW
}

generic_list * find_in_list(generic_list *ptr, char *name){

    while(ptr != NULL){
        if((int)strlen(ptr->name) == (int)strlen(name)) {
            if (!(strcmp(ptr->name, name))) return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}



void free_list(generic_list *head){
    generic_list *cur, *next;
    if(head == NULL) return;
    cur = head;
    for(cur =head; cur != NULL; cur = next){
        next = cur ->next;
        free(cur->name);
        free(cur->content);
        free(cur);

    }
    head = NULL;
}

code_list * rearrange_code(code_list *head){
    code_list *prev, *curr, *next;
    curr = head;
    if(head == NULL) return NULL;
    /* while the head of the list is data type, keep ypdate the head*/
    while(head->data_type == DATA_CODE){
         curr->address = curr->address + gIc;/* update the address*/
         curr->data_type = OPER_CODE; /* update the type*/
         next = head->next;
         curr->next = NULL;
         add_to_code_list(&next, curr);/* appending the "new" node to the end*/
         head = next;
         curr =head;
    }
    /* head is now instruction type, keep updating the rest of the list*/
    while(curr != NULL){
        if(curr->data_type == DATA_CODE){ /*find the data type node*/
            curr->address = curr->address + gIc; /*update address*/
            curr->data_type = OPER_CODE; /*update type*/
            if(curr->next == NULL) break;
            else{
                prev->next = curr->next; /*stitching the old place of the node*/
                curr ->next = NULL;
                add_to_code_list(&head, curr); /*append to end of the list*/
                curr = prev->next;
            }

        }
        else { /* not data type, continue to the next one*/
            prev =curr;
            curr = curr ->next;
        }
    }
    return head;
}

void update_symb_list(symbol_list *head){
    symbol_list *ptr;
    ptr = head;
    while(ptr != NULL){
        if(ptr->data == TRUE){
            ptr->address += gIc;
            ptr->base = ptr->address -(ptr->address % 16);
            ptr->offSet = (ptr->address % 16);
        }
        ptr =ptr->next;
    }
}
