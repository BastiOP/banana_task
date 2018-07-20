#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"

typedef struct rbt_node
{
    void* data;
    int color; //BLACK = 1, RED = 2
    int key;
    struct rbt_node* left;
    struct rbt_node* right;
    struct rbt_node* parent;

}rbt_node;

typedef struct rbtree
{
    rbt_node* root;
    rbt_node* current;
    rbt_node* nil;

}RBTree;

RBTree* create_RBTree()
{
    RBTree* tree = (RBTree*) calloc(1, sizeof(RBTree));
    tree->root = tree->nil;
    return tree;
}

rbt_node* create_rbt_node(int key, void* data)
{
    rbt_node* node = (rbt_node*) calloc(1, sizeof(rbt_node));
    node->data = data;
    node->key = key;
    return node;
}

void left_rotate(RBTree* T, rbt_node* x)
{
    rbt_node* y = x->right;
    x->right = y->left;
    if (y->left != T->nil)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == T->nil)
        T->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void right_rotate(RBTree* T, rbt_node* x)
{
    rbt_node* y = x->left;
    x->left = y->right;
    if (y->right != T->nil)
        y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == T->nil)
        T->root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;
}

void RB_insert_fixup(RBTree* T, rbt_node* z)
{
    while (z->parent->color == 2)
    {
        if (z->parent == z->parent->parent->left)
        {
            rbt_node* y = z->parent->parent->right;
            if (y->color == 2)
            {
                z->parent->color = 1;
                y->color = 1;
                z->parent->parent->color = 2;
                z = z->parent->parent;
            }
            else if (z == z->parent->right)
            {
                z = z->parent;
                left_rotate(T, z);
            }
            else
            {
                z->parent->color = 1;
                z->parent->parent->color = 2;
                right_rotate(T, z->parent->parent);
            }
        }
        else
        {
            rbt_node* y = z->parent->parent->left;
            if (y->color == 2)
            {
                z->parent->color = 1;
                y->color = 1;
                z->parent->parent->color = 2;
                z = z->parent->parent;
            }
            else if (z == z->parent->left)
            {
                z = z->parent;
                right_rotate(T, z);
            }
            else
            {
                z->parent->color = 1;
                z->parent->parent->color = 2;
                left_rotate(T, z->parent->parent);
            }
        }
    }
    T->root->color = 1;
}

void RB_insert(RBTree* T, int key, void* data)
{
    rbt_node* z = create_rbt_node(key, data);
    rbt_node* y = T->nil;
    rbt_node* x = T->root;
    while (x != T->nil)
    {
        y = x;
        if (z->key < x->key)
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if (y == T->nil)
        T->root = z;
    else if (z->key < y->key)
        y->left = z;
    else
        y->right = z;
    z->left = T->nil;
    z->right = T->nil;
    z->color = 2;
    RB_insert_fixup(T, z);
}

void* tree_minimum(RBTree* T, rbt_node* x)
{
    while (x->left != T->nil)
        x = x->left;
    return x;
}

void RB_transplant(RBTree* T, rbt_node* u, rbt_node* v)
{
    if (u->parent == T->nil)
        T->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

void RB_delete_fixup(RBTree* T, rbt_node* x)
{
    while ((x != T->root)&&(x->color == 1))
    {
        if (x == x->parent->left)
        {
            rbt_node* w = x->parent->right;
            if (w->color == 2)
            {
                w->color = 1;
                x->parent->color = 2;
                left_rotate(T, x->parent);
                w = x->parent->right;
            }
            if ((w->left->color == 1)&&(w->right->color == 1))
            {
                w->color = 2;
                x = x->parent;
            }
            else if (w->right->color == 1)
            {
                w->left->color = 1;
                w->color = 2;
                right_rotate(T, w);
                w = x->parent->right;
            }
            else
            {
                w->color = x->parent->color;
                x->parent->color = 1;
                w->right->color = 1;
                left_rotate(T, x->parent);
                x = T->root;
            }
        }
        else
        {
            rbt_node* w = x->parent->left;
            if (w->color == 2)
            {
                w->color = 1;
                x->parent->color = 2;
                right_rotate(T, x->parent);
                w = x->parent->left;
            }
            if ((w->right->color == 1)&&(w->left->color == 1))
            {
                w->color = 2;
                x = x->parent;
            }
            else if (w->left->color == 1)
            {
                w->right->color = 1;
                w->color = 2;
                left_rotate(T, w);
                w = x->parent->left;
            }
            else
            {
                w->color = x->parent->color;
                x->parent->color = 1;
                w->left->color = 1;
                right_rotate(T, x->parent);
                x = T->root;
            }
        }
    }
    x->color = 1;
}

void RB_delete(RBTree* T, int key)
{
    rbt_node* z = RB_search(T, key);
    rbt_node* y = z;
    rbt_node* x;
    int yOriginalcolor = y->color;
    if (z->left == T->nil)
    {
        x = z->right;
        RB_transplant(T, z, z->right);
    }
    else if (z->right == T->nil)
    {
        x = z->right;
        RB_transplant(T, z, z->left);
    }
    else
    {
        y = tree_minimum(T, z->right);
        yOriginalcolor = y->color;
        x = y->right;
        if (y->parent == z)
            x->parent = y;
        else
        {
            RB_transplant(T, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        RB_transplant(T, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (yOriginalcolor == 1)
        RB_delete_fixup(T, x);
}

void* RB_search(RBTree* T, int key)
{

}

void* RB_first(RBTree* T)
{
    rbt_node* x = T->root;
    if (x != T->nil)
        return tree_minimum(T, x);
    return NULL;
}

void* RB_next(RBTree* T);
