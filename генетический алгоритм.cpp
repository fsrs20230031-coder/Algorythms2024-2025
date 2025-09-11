#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
struct Item {
    int value;
    int weight;
    Item (int v = 0, int w=0):value(v),weight(w){}
};


int value(const vector<int>& individual,int N,int MaxWeight,const vector<Item>& items) {
    int totalValue = 0,totalWeight = 0;

    for (int i = 0; i < N; i++) {
        if (individual[i]) {
            totalValue += items[i].value;
            totalWeight += items[i].weight;
        }
    }

    if (totalWeight > MaxWeight) {
        return totalValue - (totalWeight - MaxWeight)*10;
    }

    return totalValue ;
}
int weight(const vector<int>& individual,int N,int MaxWeight,const vector<Item>& items) {
    int totalWeight = 0;

    for (int i = 0; i < N; i++) {
        if (individual[i]) {
            totalWeight += items[i].weight;
        }
    }

    return totalWeight ;
}

vector<int> randomIndividual(int N) {
    vector<int> individual(N);
    for (int i = 0; i < N; i++) {
        individual[i] = rand() % 2;
    }
    return individual;
}

vector<int> Selection(const vector< vector<int> >& Population,int size,int N,int MaxWeight,const vector<Item>& items) {
    int i = rand() % size;
    int j = rand() % size;

    if (value(Population[i] , N  , MaxWeight ,  items) >= value(Population[j], N, MaxWeight,  items)) {
        return Population[i];
    }
    return Population[j];
}


vector<int> crossover(const vector<int>& parent1, const vector<int>& parent2,int N,int MaxWeight,const vector<Item>& items) {
    vector<int> child(N);
    int point = rand() % N;
    for (int i = 0; i < N; i++) {
        if (i < point) {
            child[i] = parent1[i];
        } else {
            child[i] = parent2[i];
        }
    }
    return child;
}

void mutation(vector<int>& individual,int N,int MaxWeight,const vector<Item>& items,double probability_mutation) {
    for (int i = 0; i < N; i++) {
        if ((double)rand() / RAND_MAX < probability_mutation) {
            if(individual[i]) individual[i] = 0;
            else individual[i] = 1;
        }
    }
}

int main() {
    int N;
    int MaxWeight;
    cout<<"Enter filename"<<endl;
    string filename;
    cin>>filename;
    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "Error:can't open input file\n";
        return 1;
    }
    infile >> N >> MaxWeight;
    vector<Item> items;
    int size = 100;
    int gen = 1000;
    double probability_crossover = 0.7;
    double probability_mutation = 0.01;
    vector< vector<int> > Population;
    for (int i = 0; i < size; i++) {
        Population.push_back(randomIndividual(N));
    }
    int best_value = 0;
    vector<int> best_individual;
    vector< vector<int> > new_Population;
    int value_i,weight_i;
    for (int i = 0; i < N; i++) {
        infile >> value_i >> weight_i;
        Item item(value_i,weight_i);
        items.push_back(item);
    }

    for (int i = 0; i < gen; i++) {
        for (int i = 0; i < size; i++) {
            vector<int> parent1 = Selection(Population,size,N,MaxWeight,items);
            vector<int> parent2 = Selection(Population,size,N,MaxWeight,items);

            vector<int> child;

            if ((double)rand() / RAND_MAX < probability_crossover) {
                child = crossover(parent1, parent2,N,MaxWeight,items);
            } else {
                child = parent1;
            }

            mutation(child,N,MaxWeight,items,probability_mutation);

            new_Population.push_back(child);

            int current_value = value(child,N,MaxWeight,items);
            int w = weight(child,N,MaxWeight,items);
            if (current_value > best_value && w<=MaxWeight) {
                best_value = current_value;
                best_individual = child;
            }
        }

        Population = new_Population;
        new_Population.clear();
    }

    cout << "Best value " << best_value << endl;
    ofstream fout("output.txt");
    fout<<filename<<" "<<best_value;
    fout.close();
    return 0;
}

