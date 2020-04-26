#include "tree.h"

int main(int argc, char *argv[])
{
    tree tr;
    tr.addnode(10);
    tr.addnode(4);
    tr.addnode(18);
    tr.addnode(15);
    tr.addnode(3);
    tr.addnode(12);
    tr.addnode(11);
    tr.addnode(5);
    tr.addnode(9);
    tr.addnode(17);
    tr.addnode(2);
    tr.addnode(24);

    cout << "direct bypass" << endl;
    tr.DiPrint();
    cout << "backward bypass" << endl;
    tr.BaPrint();
    cout << "annular bypass" << endl;
    tr.AnPrint();

    cout << "4 deleted" << endl;
    tr.delOnTree(4);
    tr.DiPrint();
    cout << "3 deleted" << endl;
    tr.delOnTree(3);
    tr.DiPrint();
    cout << "10 deleted" << endl;
    tr.delOnTree(10);
    tr.DiPrint();
    cout << "12 deleted" << endl;
    tr.delOnTree(12);
    tr.DiPrint();

    return 0;
}
