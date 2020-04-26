#include <iostream>

using namespace std;

struct TreeNode
{
    double data; // ключ/данные
    TreeNode *left; // указатель на левого потомка
    TreeNode *right; // указатель на правого потомка
};
typedef TreeNode* ptrnode;

void preorderPrint(TreeNode *root,int h = 0)
{
    if (root == nullptr)   // Базовый случай
        return;
    for (int i = 0; i < h; i++)
        cout << '\t';
    //cout << root->data << endl;
    preorderPrint(root->left,h+1);   //рекурсивный вызов левого поддерева
    cout << root->data << endl;
    preorderPrint(root->right,h+1);  //рекурсивный вызов правого поддерева
    //cout << root->data << endl;
}

ptrnode maketree(int data[],int from,int n)
{
    int n1,n2;
    ptrnode tree;
    if (n == 0) return NULL;
    tree = new TreeNode;
    tree->data = data[from];
    n1 = n / 2;
    n2 = n - n1 - 1;
    tree->left = maketree(data,from + 1,n1);
    tree->right = maketree(data,from + 1 + n1,n2);
    return tree;
}

int main(int argc, char *argv[])
{
    int base[] = {45,19,5,24,34,4,22,17,87};
    preorderPrint(maketree(base,0,9));
    return 0;
}
