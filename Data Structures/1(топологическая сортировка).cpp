/////////////////////////////////////////////////////////////////
//Топологическая сортировка графа при помощи алгоритма Тарьяна //
//Входные данные представляют собой число вершин и дуги графа. //
//В данном случае 8 - число вершин, а за ним дуги              //
//Левое число - начало дуги, правое - конец.                   //
//Входные данные: 8 1 0 2 1 2 4 3 1 3 0 4 1 4 0 5 0 5 6 6 4 7 0//
/////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <stack>

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

void sorted(node* current,stack<short>&done)
{
    if (current->color == 0)
        {
            current->color = 1;
            if (current->ribs.size() > 0)
                for (int i = 0; i < current->ribs.size(); i++)
                    sorted(current->ribs.at(i),done);
            current->color = 2;
            done.push(current->data);
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
    stack<short>done;
    int n = 0;
    cin >> n; //Вводим количество вершин

    for (int i = 0; i < n; i++) //заполняем массив вершин
        {
            node* tmp = new node;
            tmp->data = i;
            tmp->color = 0;
            mass.push_back(tmp);
        }

    for (int i = 0; i < 11; i++) //Вводим дуги
        {
            pair<int,int>tmp;
            cin >> tmp.first >> tmp.second;
            mass.at(tmp.first)->ribs.push_back(mass.at(tmp.second));
        }

    for (int i = 0; i < mass.size(); i++) //Сортируем граф
        sorted(mass.at(i),done);

    for (int i = 0; i < mass.size(); i++) //Выводим граф на экран
        mass.at(i)->print();
    cout << endl;

    while(done.size() > 0) //Выводим вершины в новом порядке
        {
            cout << done.top() << endl;
            done.pop();
        }
    return 0;
}
