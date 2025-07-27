#include <iostream>


int main() {
    using std::string;

    string array[3][4] = {{"Audi", "Escape", "F-150", "Mustang"},
                          {"Corvette", "Equinox", "Silverado", "Honda"},
                          {"Changeller", "Durango", "Ram 1500", "BMW"}};

    int colums = sizeof(array[0])/ sizeof(array[0][0]);
    int rows = sizeof(array)/sizeof(array[0]);

    for (int i = 0; i <= rows ; i++) {
        for (int j = 0; j <= colums; j++) {
            std::cout << array[i][j] << " ";
        }
        std::cout << '\n';
    }

}