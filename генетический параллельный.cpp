#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>


using namespace std;

struct Item {
    int value;
    int weight;
    Item(int v = 0, int w = 0) : value(v), weight(w) {}
};

mutex mtx;

int value(const vector<int>& individual, int N, int MaxWeight, const vector<Item>& items) {
    int totalValue = 0, totalWeight = 0;
    for (int i = 0; i < N; i++) {
        if (individual[i]) {
            totalValue += items[i].value;
            totalWeight += items[i].weight;
        }
    }

    if (totalWeight > MaxWeight) {
        return totalValue - (totalWeight - MaxWeight) * 10;
    }

    return totalValue;
}

int weight(const vector<int>& individual, int N, int MaxWeight, const vector<Item>& items) {
    int totalWeight = 0;
    for (int i = 0; i < N; i++) {
        if (individual[i]) {
            totalWeight += items[i].weight;
        }
    }
    return totalWeight;
}

vector<int> randomIndividual(int N) {
    vector<int> individual(N);
    for (int i = 0; i < N; i++) {
        individual[i] = rand() % 2;
    }
    return individual;
}

vector<int> Selection(const vector<vector<int>>& Population, int size, int N, int MaxWeight, const vector<Item>& items) {
    int i = rand() % size;
    int j = rand() % size;

    if (value(Population[i], N, MaxWeight, items) >= value(Population[j], N, MaxWeight, items)) {
        return Population[i];
    }
    return Population[j];
}

vector<int> crossover(const vector<int>& parent1, const vector<int>& parent2, int N, int MaxWeight, const vector<Item>& items) {
    vector<int> child(N);
    int point = rand() % N;
    for (int i = 0; i < N; i++) {
        if (i < point) child[i] = parent1[i];
        else child[i] = parent2[i];
    }
    return child;
}

void mutation(vector<int>& individual, int N, int MaxWeight, const vector<Item>& items, double probability_mutation) {
    for (int i = 0; i < N; i++) {
        if ((double)rand() / RAND_MAX < probability_mutation) {
            individual[i] = 1 - individual[i];
        }
    }
}

void process_population_chunk(int start, int end,
                              const vector<vector<int>>& Population,
                              vector<vector<int>>& new_Population,
                              int N, int MaxWeight, const vector<Item>& items,
                              double probability_crossover, double probability_mutation,
                              int& best_value, vector<int>& best_individual) {
    for (int i = start; i < end; i++) {
        vector<int> parent1 = Selection(Population, Population.size(), N, MaxWeight, items);
        vector<int> parent2 = Selection(Population, Population.size(), N, MaxWeight, items);

        vector<int> child;
        if ((double)rand() / RAND_MAX < probability_crossover) {
            child = crossover(parent1, parent2, N, MaxWeight, items);
        } else {
            child = parent1;
        }

        mutation(child, N, MaxWeight, items, probability_mutation);

        int current_value = value(child, N, MaxWeight, items);
        int current_weight = weight(child, N, MaxWeight, items);

        mtx.lock();
        new_Population.push_back(child);
        if (current_value > best_value && current_weight <= MaxWeight) {
            best_value = current_value;
            best_individual = child;
        }
        mtx.unlock();
    }
}

int main() {
    int N;
    int MaxWeight;

    cout << "Enter filename: ";
    string filename;
    cin >> filename;

    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "Error: can't open input file\n";
        return 1;
    }

    infile >> N >> MaxWeight;

    vector<Item> items;
    int value_i, weight_i;

    for (int i = 0; i < N; i++) {
        infile >> value_i >> weight_i;
        items.push_back(Item(value_i, weight_i));
    }

    int size = 100;
    int gen = 1000;
    double probability_crossover = 0.7;
    double probability_mutation = 0.01;

    vector<vector<int>> Population;


    for (int i = 0; i < size; i++) {
        Population.push_back(randomIndividual(N));
    }

    int best_value = 0;
    vector<int> best_individual;

    auto start1 = chrono::high_resolution_clock::now();
    for (int g = 0; g < gen; g++) {
        vector<vector<int>> new_Population;

        int thread_count = 1;
        vector<thread> threads;

        int chunk_size = size / thread_count;

        for (int t = 0; t < thread_count; t++) {
            int start = t * chunk_size;
            int end = (t == thread_count - 1) ? size : start + chunk_size;

            threads.push_back(thread(process_population_chunk, start, end,
                                     ref(Population), ref(new_Population),
                                     N, MaxWeight, ref(items),
                                     probability_crossover, probability_mutation,
                                     ref(best_value), ref(best_individual)));
        }

        for (auto& th : threads) {
            th.join();
        }

        Population = new_Population;
    }
    auto end1 = chrono::high_resolution_clock::now();


    cout << "Best value: " << best_value << endl;

    ofstream fout("output.txt");
    fout << filename << " " << best_value <<", "<<chrono::duration<double, milli>(end1 - start1).count()<<"ms"<< endl;
    fout.close();

    return 0;
}
