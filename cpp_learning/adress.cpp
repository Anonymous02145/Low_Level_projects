#include <iostream>


int main() {

    std::string metadata = {"Aarush\n14\n16/09/2010"};
    int number = 90;

    std::cout << &metadata << '\n';
    std::cout << &number << '\n';

    return 0;

}