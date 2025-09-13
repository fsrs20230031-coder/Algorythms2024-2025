#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct Item {
    int value;
    int weight;
    double ratio;
};

struct Node {
    int level;
    int value;
    int weight;
    double bound;
};

double bound(Node node, int n, int W, const vector<Item>& items) {
    if (node.weight >= W) return 0;

    double valueBound = node.value;
    int j = node.level + 1;
    int totalWeight = node.weight;

    while (j < n && totalWeight + items[j].weight <= W) {
        totalWeight += items[j].weight;
        valueBound += items[j].value;
        j++;
    }

    if (j < n) {
        valueBound += (W - totalWeight) * items[j].ratio;
    }

    return valueBound;
}

bool compare(Item a, Item b) {
    return a.ratio > b.ratio;
}

int main() {
    string filename;
    cout << "Enter filename" << endl;
    cin >> filename;
    ifstream inputFile(filename);
    int N, W;
    inputFile >> N >> W;

    vector<Item> items(N);
    for (int i = 0; i < N; i++) {
        inputFile >> items[i].value >> items[i].weight;
        items[i].ratio = double(items[i].value) / items[i].weight;
    }
    inputFile.close();

    sort(items.begin(), items.end(), compare);

    queue<Node> Q;
    Node root = {-1, 0, 0, bound(root, N, W, items)};
    Q.push(root);

    int maxValue = 0;

    while (!Q.empty()) {
        Node node = Q.front();
        Q.pop();

        if (node.level + 1 < N) {
            Node left = {node.level + 1, node.value + items[node.level + 1].value, node.weight + items[node.level + 1].weight};
            left.bound = bound(left, N, W, items);
            if (left.weight <= W && left.value > maxValue) {
                maxValue = left.value;
            }
            if (left.bound > maxValue) {
                Q.push(left);
            }

            Node right = {node.level + 1, node.value, node.weight};
            right.bound = bound(right, N, W, items);
            if (right.bound > maxValue) {
                Q.push(right);
            }
        }
    }

    cout << "Maximum Value: " << maxValue << endl;
    ofstream fout("output.txt");
    fout << filename<<": "<<maxValue;
    fout.close();
    return 0;
}
