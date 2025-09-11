#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

struct Item {
    int weight;
    int value;
    double ratio;
};


bool compare(Item a, Item b) {
    return a.ratio > b.ratio;
}

int main() {
    string filename;
    cout<<"Enter filename"<<endl;
    cin>>filename;
    ifstream inputFile(filename);
    int N, W;
    inputFile >> N >> W;
    vector<Item> items(N);

    for (int i = 0; i < N; i++) {
        int value, weight;
        inputFile >> value >> weight;
        items[i].weight = weight;
        items[i].value = value;
        items[i].ratio = (double)value/weight;


    }

    sort(items.begin(), items.end(), compare);

    int Weight = 0;
    double Value = 0;

    for (int i = 0; i < N; i++) {
        if (Weight + items[i].weight <= W) {
            Weight += items[i].weight;
            Value += items[i].value;
            cout<<items[i].value<<" "<<items[i].weight<<endl;
        }
    }

    inputFile.close();
    ofstream fout("output.txt");
    fout << filename<<": "<<Value;
    fout.close();
    cout << "max usefulness: " << Value << endl;
    cout << "weight: " << Weight << endl;

    return 0;
}
