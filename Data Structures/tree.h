#include <iostream>
using namespace std;

class tree
{
    struct node
    {
        int key;
        node *left;
        node *right;
    };

    typedef node* ptrnode;

    ptrnode root;
    void del(ptrnode& r, ptrnode& q);

    ptrnode getroot(){ return root;};
    ptrnode addnode(int key,ptrnode& root);
    void dlt(ptrnode& root,int deldata);
    void delAllTree(ptrnode& root);
    void DiPrint(ptrnode root,int h);
    void BaPrint(ptrnode root,int h);
    void AnPrint(ptrnode root,int h);

public:
    tree() : root(nullptr){};
    ~tree();
    node *addnode(int key);
    void preorderPrint();
    void delOnTree(int deldata);
    void DiPrint();
    void BaPrint();
    void AnPrint();

};

tree::~tree()
{
    delAllTree(root);
}

tree::ptrnode tree::addnode(int key,ptrnode& root)
{
    if(root == nullptr)
        {
            root = new node;
            root->left = root->right = nullptr;
            root->key = key;
        }
    else if(key < root->key)
        root->left = addnode(key,root->left);
    else
        root->right = addnode(key,root->right);
    return(root);
}

void tree::DiPrint(ptrnode root, int h = 0)
{
    if (root == nullptr)
        return;
    for (int i = 0; i < h; i++)
        cout << '\t';
    cout << root->key << endl;
    DiPrint(root->left,h+1);
    DiPrint(root->right,h+1);
}

void tree::BaPrint(ptrnode root, int h = 0)
{
    if (root == nullptr)
        return;
    for (int i = 0; i < h; i++)
        cout << '\t';
    BaPrint(root->left,h+1);
    cout << root->key << endl;
    BaPrint(root->right,h+1);
}

void tree::AnPrint(ptrnode root, int h = 0)
{
    if (root == nullptr)
        return;
    for (int i = 0; i < h; i++)
        cout << '\t';
    AnPrint(root->left,h+1);
    AnPrint(root->right,h+1);
    cout << root->key << endl;
}

void tree::del(ptrnode& r, ptrnode& q)
{
    if (r->right != nullptr) del(r->right,q);
    else
        {
            q->key = r->key;
            q = r;
            r = r->left;
        }
}

void tree::dlt(ptrnode& root, int deldata)
{
    ptrnode q = nullptr;
    if (deldata < root->key) dlt(root->left,deldata);
    else if(deldata > root->key) dlt(root->right,deldata);
    else
        {
            q = root;
            if (q->right == nullptr) root = q->left;
            else if (q->left == nullptr) root = q->right;
            else del(q->left,q);
            delete(q);
        }
}

void tree::delAllTree(ptrnode& root)
{
    if (root != nullptr)
        {
            delAllTree(root->left);
            delAllTree(root->right);
            delete(root);
        }
}

tree::node *tree::addnode(int key)
{
    return addnode(key,root);
}

void tree::delOnTree(int deldata)
{
    dlt(root,deldata);
}

void tree::DiPrint()
{
    DiPrint(root);
}

void tree::BaPrint()
{
    BaPrint(root);
}

void tree::AnPrint()
{
    AnPrint(root);
}
