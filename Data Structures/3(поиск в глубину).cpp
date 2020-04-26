///////////////////////////////////////////////////////////////
//Поиск в глубину в графе. Входные данные - количество вершин//
//И описание дуг графа. В данном случае 10 - число вершин,   //
//За ним дуги. Первое число - начало дуги, второе - конец.   //
//Выходные данные - перечисление вершин графа в порядке      //
//поиска в глубину.                                          //
//Входные данные:                                            //
//10 0 8 0 2 0 1 1 4 2 6 2 3 3 5 4 6 5 7 6 9 8 9 8 7         //
///////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>

using namespace std;

struct node
{
    short data;
    short color;
    vector<node*>ribs;

    void print()
    {
        cout << "data:\t" << data << "\tribs:" << endl;
        for (int i = 0; i < ribs.size(); i++)
            cout << ribs.at(i)->data << endl;
    }
};

void sorted(node* current,vector<short>&done)
{
    if (current->color == 0)
        {
            current->color = 1;
            done.push_back(current->data);
            if (current->ribs.size() > 0)
                for (int i = 0; i < current->ribs.size(); i++)
                    sorted(current->ribs.at(i),done);
            current->color = 2;
            return;
        }
    else if(current->color == 1)
        {
            //cout << "Loop found. Topological sorting not possible" << endl;
            return;
        }
}

int main(int argc, char *argv[])
{
    vector<node*>mass;
    vector<short>done;
    int n = 0;
    cin >> n;

    for (int i = 0; i < n; i++)
        {
            node* tmp = new node;
            tmp->data = i;
            tmp->color = 0;
            mass.push_back(tmp);
        }

    for (int i = 0; i < 12; i++)
        {
            pair<int,int>tmp;
            cin >> tmp.first >> tmp.second;
            mass.at(tmp.first)->ribs.push_back(mass.at(tmp.second));
        }

    for (int i = 0; i < mass.size(); i++)
        sorted(mass.at(i),done);

    for (int i = 0; i < mass.size(); i++)
        mass.at(i)->print();
    cout << endl;

    for (int i = 0; i < done.size(); i++)
        cout << done.at(i) + 1 << endl;
    return 0;
}
