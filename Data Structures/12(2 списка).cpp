#include <iostream>
#include <algorithm>

using namespace std;

struct Node
{
    int data;
    Node *next;

    void operator()(Node* t)
    {
        while (t->next != NULL)
            {
                if (t->data < 0)
                    cout << t->data << endl;
                t = t->next;
            }
    }
};

class List
{
private:
    Node *head; //"голова" связанного списка
    Node *tail;
public:
    List()
    {
        head = NULL;
        tail = NULL;
    }

    Node* gethead()
    {
        return head;
    }

    Node* gettail()
    {
        return tail;
    }

    void addNode(int d)
    {
        Node *nd = new Node;
        nd->data = d;
        nd->next = NULL;
        if(head == NULL)
            head = nd;
        else
            {
                Node *current = head;
                while(current->next != NULL)
                    current = current->next;
                current->next = nd;
                tail = nd;
            }
    }
    void printList()
    {
        Node *current = head;
        while(current != NULL)
            {
                cout << current->data << endl;
                current = current->next;
            }
    }

        void operator()(Node* t)
        {
            while (t != NULL)
                {
                if (t->data < 0)
                    t->data = 0;
                t = t->next;
                }
        }
};

int main(int argc, char *argv[])
{

    int n1,n2;

    cout << "size of the 1 list: ";
    cin >> n1;
    cout << "size of the 2 list: ";
    cin >> n2;
    List first,second;

    int tmp = 0;
    for (int i = 0; i < n1; i++)
        {
            cout << "in first  ";
            cin >> tmp;
            first.addNode(tmp);
        }

    first.printList();

    for (int i = 0; i < n2; i++)
        {
            cout << "in second ";
            cin >> tmp;
            second.addNode(tmp);
        }
    second(second.gethead());
    second.printList();
    Node *hd1 = first.gethead(); //
    Node* nd = NULL;
    while(hd1->next != NULL)
        {
            if (hd1->next->data % 2 != 0)
                nd = hd1;
            hd1 = hd1->next;
        }
    hd1 = nd->next;

    cout << endl;
    nd->next = second.gethead();
    second.gettail()->next = hd1;

    hd1 = first.gethead();
    while(hd1 != NULL)
        {
            cout << hd1->data << endl;
            hd1 = hd1->next;
        }

    return 0;
}
