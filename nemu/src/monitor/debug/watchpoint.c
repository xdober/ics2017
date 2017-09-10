#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].exp[0] = '\0';
    wp_pool[i].value = 0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
    if (free_!=NULL) {
        WP* newwp=free_;
        free_=free_->next;
        newwp->next=NULL;
        return newwp;
    }
    else {
        assert(0);
    }
    return NULL;
}

void free_wp(WP *wp) {
    wp->next=free_->next;
    free_->next=wp;
}

void setWP(char *args){
    bool isEXP = false;
    int Ovalue = expr(args, &isEXP);
    if (!isEXP) {
        printf("incorrect expression!\n");
        return ;
    }
    WP *new = new_wp();
    strcpy(new->exp, args);
    new->value = Ovalue;
    if (head==NULL) {
        new->NO = 0;
    }
    else {
        new->NO=head->NO+1;
    }
    new->next = head;
    head = new;
    printf("set wp NO.%d success.\n", head->NO);
    return ;
}

void delWP(int no) {
    if(head==NULL){
        printf("no any watchpoint now.\n");
        return ;
    }
    WP *temp = head, *pre = NULL;
    bool flag = false;
    while(temp!=NULL) {
        if(temp->NO==no) {
            flag = true;
            if (temp==head) {
                head = head->next;
                break;
            }
            pre->next = temp->next;
            break;
        }
        pre = temp;
        temp = temp->next;
    }
    if (!flag) {
        printf("no this watchpoint.\n");
        return ;
    }
    free_wp(temp);
    printf("No.%d watchpoint deleted.\n", no);
    return ;
}

void showWP(){
    printf("NO\texp\t\tvalue\t\n");
    WP *temp=head;
    while(temp!=NULL){
        printf("%d\t%s\t%d\n", temp->NO, temp->exp, temp->value);
        temp = temp->next;
    }
    return ;
}

bool triggerWP(int *no) {
    WP *temp = head;
    int Nvalue=0;
    bool flag = true;
    while(temp!=NULL){
        Nvalue = expr(temp->exp, &flag);
        if (Nvalue!=temp->value && flag) {
            temp->value = Nvalue;
            *no = temp->NO;
            return true;
        }
        temp = temp->next;
    }
    return false;
}
