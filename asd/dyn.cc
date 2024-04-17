#include <iostream>

using namespace std;
using pii = pair<int, int>;

class Node {
   public:
  pii key; // element + krotnosc
  Node *left;
  Node *right;
  int height;
  long long size_l;
  long long size_r;
};

void printpii(pii key){
    cout << "(" << key.first << ", " << key.second << ")" << endl;
}

int height(Node *N) {
  if (N == NULL)
    return 0;
  return N->height;
}

int max(int a, int b) {
  return (a > b) ? a : b;
}

Node *newNode(pii key) {
  Node *node = new Node();
  node->key = key;
  node->left = NULL;
  node->right = NULL;
  node->height = 1;
  node->size_l = node->size_r = 0;
  return (node);
}

long long get_size_l(Node* node){
    if(node == NULL)
        return 0;
    return node->size_l;
}

long long get_size_r(Node* node){
    if(node == NULL)
        return 0;
    return node->size_r;
}

Node *rightRotate(Node *y) {
  Node *x = y->left;
  Node *T2 = x->right;
  x->right = y;
  y->left = T2;
  y->height = max(height(y->left),
          height(y->right)) +
        1;
  x->height = max(height(x->left),
          height(x->right)) +
        1;
  y->size_l = x->size_r;
  x->size_r = y->size_r + y->size_l + y->key.second;
  return x;
}

Node *leftRotate(Node *x) {
  Node *y = x->right;
  Node *T2 = y->left;
  y->left = x;
  x->right = T2;
  x->height = max(height(x->left),
          height(x->right)) +
        1;
  y->height = max(height(y->left),
          height(y->right)) +
        1;

  x->size_r = y->size_l;
  y->size_l = x->size_r + x->size_l + x->key.second;
  return y;
}

int getBalanceFactor(Node *N) {
  if (N == NULL)
    return 0;
  return height(N->left) -
       height(N->right);
}

Node *insertNode(Node *node, pii key, long long i) {
  if (node == NULL)
    return (newNode(key));
  if (node->size_l >= i){ // na lewo
    node->left = insertNode(node->left, key, i);
    node->size_l += key.second;
  } else if(node->size_l + node->key.second < i){ // na prawo
    node->right = insertNode(node->right, key, i - node->size_l - node->key.second);
    node->size_r += key.second;
  } else{ // tutaj rozdzielamy
    // nowy node zastepuje natrafiony, natrafiony (dzielimy na 2 i) dodajemy na (lewo i) prawo
    if(i == node->size_l + 1){ // idealnie na lewej granicy, nie dzielimy

        node->left = insertNode(node->left, key, i);
        node->size_l += key.second;
    } else{ // dzielimy na dwa
        pii new_key_b(node->key.first, node->key.second - (i - node->size_l) + 1);
        pii new_key_a(node->key.first, node->key.second - new_key_b.second);

        node->key = key;
        node->left = insertNode(node->left, new_key_a, node->size_l + 1);
        node->size_l += new_key_a.second;

        node->right = insertNode(node->right, new_key_b, 1);
        node->size_r += new_key_b.second;
    }
  }

  node->height = 1 + max(height(node->left),
               height(node->right));
  int balanceFactor = getBalanceFactor(node);
  if (balanceFactor > 1){
      return rightRotate(node);
  }
  if (balanceFactor < -1) {
      return leftRotate(node);
  }
  return node;
}

long long get(Node* node, long long i){
    if(node->size_l < i && node->size_l + node->key.second >= i)
        return node->key.first;
    else if(node->size_l >= i)
        return get(node->left, i);
    else
        return get(node->right, i - node->size_l - node->key.second);
}


int main(){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    int m;
    long long n = 0;
    cin >> m;
    char op;
    long long j, x, k, w = 0;
    cin >> op >> j >> x >> k;
    Node* avl = newNode({x, k});
    n += k;

    for(int i = 1; i < m; i++){
        cin >> op >> j;
        if(op == 'i'){
            j = (j + w) % (n + 1);
            cin >> x >> k;
            avl = insertNode(avl, {x,k}, j + 1);
            n += k;
        } else{
            j = (j + w) % n;
            w = get(avl, j + 1);
            cout << w << endl;
        }
    }

    return 0;
}
