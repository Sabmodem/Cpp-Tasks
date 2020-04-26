///////////////////////////////////////////////////////////////////////////
//Поиск в ширину в ориентированном графе.                                //
//Входные данные: 10 0 8 0 2 0 1 1 4 2 6 2 3 3 5 4 6 5 7 6 9 8 9 8 7     //
//Входные данные представляют собой число вершин и дуги графа. В данном  //
//случае 10 - число вершин, а за ним дуги. Левое число - начало дуги,    //
//Правое - конец.                                                        //
//Предпочтение отдано вершине с большим приоритетом, т.к. этого          //
//требовало условие задачи.                                              //
///////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <vector>
#include <queue>

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

void sorted(node* current,queue<node*>&visited, queue<node*>&notvisited)
{
    if(notvisited.size() > 0)
        notvisited.pop();
    if (current->color != 2)
        {
            current->color = 2;
            visited.push(current);
            if (current->ribs.size() > 0)
                for (int i = 0; i < current->ribs.size(); i++)
                    {
                        if(current->ribs.at(i)->color == 0)
                            {
                                notvisited.push(current->ribs.at(i));
                                current->ribs.at(i)->color = 1;
                            }
                    }
        }
    while(!notvisited.empty())
        sorted(notvisited.front(),visited,notvisited);

    while(!visited.empty())
        {
            cout << visited.front()->data + 1 << endl;
            visited.pop();
        }

    return;
}

int main(int argc, char *argv[])
{
    vector<node*>mass;
    int n = 0;
    cin >> n;

    queue<node*>notvisited;
    queue<node*>visited;

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
            mass.at(i)->print();
    cout << endl;

    sorted(mass.at(0),visited,notvisited);

        return 0;
}
