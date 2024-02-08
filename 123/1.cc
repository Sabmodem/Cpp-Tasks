#include <iostream>
#include <cstdlib>
#include <ctime>
#include <experimental/random>

using namespace std;

bool find_value(int* arr, int n, int val) {
    for(int i = 0; i < n; i++) {
        if (arr[i] == val) {
            return true;
        }
    };
    return false;
}

int* make_simple_array(int n) {
    int* arr = new int[n];
    int rand_val;
    for(int i = 0; i < n; i++) {
        rand_val = rand();
        if(find_value(arr, n, rand_val)) {
            continue;
        }
        arr[i] = rand_val;
    };
    return arr;
}

int** make_nested_array(int n) {
    int** nested_arr = new int*[n];
    for(int i = 0; i < n; i++) {
        nested_arr[i] = make_simple_array(n);
    };
    return nested_arr;
}

void print_simple_array(int* arr, int n) {
    cout << '[';
    for(int i = 0; i < n; i++) {
        cout << arr[i] << ',';
    };
    cout << ']' << endl;
}

void print_nested_array(int** arr, int n) {
    cout << '[' << endl;
    for(int i = 0; i < n; i++) {
        cout << '\t';
        print_simple_array(arr[i], n);
    };
    cout << ']' << endl;
}

int* get_order_of_elements(int** arr, int n, int max) {
    int* elements_order = new int[n];
    int rand_val = 0;
    for(int i = 0; i < n; i++) {
        while (find_value(elements_order, n, rand_val)) {
            rand_val = std::experimental::fundamentals_v2::randint(0,max);
        }
        elements_order[i] = rand_val;
    }

    for(int i = 0; i < n; i++) {
        elements_order[i] = elements_order[i]-1;
    }

    return elements_order;
}

int* make_simple_array_from_nested(int** nested_arr, int* elements_order, int n) {
    int* simple_arr = new int[n*n];
    int first_index;
    int second_index;
    int current_element = 0;
    for(int i = 0; i < n; i++) {
        for(int k = 0; k < n; k++) {
            first_index = elements_order[i];
            second_index = k;
            simple_arr[current_element] = nested_arr[first_index][second_index];
            current_element++;
        };        
    };
    return simple_arr;
}

int** make_empty_nested_array(int n) {
    int** nested_arr = new int*[n];
    for(int i = 0; i < n; i++) {
        nested_arr[i] = new int[n];
    };
    return nested_arr;
};

int** restore_nested_array(int* simple_arr, int* elements_order, int n) {
    int** nested_arr = make_empty_nested_array(n);
    int first_index;
    int second_index;
    int current_element = 0;
    for(int i = 0; i < n; i++) {
        for(int k = 0; k < n; k++) {
            first_index = elements_order[i];
            second_index = k;
            nested_arr[first_index][k] = simple_arr[current_element];
            current_element++;
        };        
    };
    return nested_arr;
}

void clear_nested_arr(int** arr, int n) {
    for(int i = 0; i < n; i++) {
        delete[] arr[i];
    }
}

int main() {
    srand(time(NULL));

    int len;

    cout << "enter len: ";
    cin >> len;

    int** nested_arr = make_nested_array(len);
    print_nested_array(nested_arr, len);
    int* elements_order = get_order_of_elements(nested_arr, len, len);
    print_simple_array(elements_order, len);
    int* simple_arr = make_simple_array_from_nested(nested_arr, elements_order, len);
    print_simple_array(simple_arr, len*len);

    int** restored_nested_array = restore_nested_array(simple_arr, elements_order, len);
    print_nested_array(nested_arr, len);

    clear_nested_arr(nested_arr, len);
    delete[] nested_arr;

    delete[] elements_order;

    delete[] simple_arr;

    clear_nested_arr(restored_nested_array, len);
    delete[] restored_nested_array;

    return 0;
}