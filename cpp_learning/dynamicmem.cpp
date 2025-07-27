#include <iostream>
#include <istream>
#include <string>


int main() {
    std::string grades;

    std::cout << "Enter the grades seprated by space : ";
    std::cin >> std::ws;
    std::getline(std::cin, grades);

    std::string *pointer;

    pointer = new std::string[sizeof(grades)];

    std::cout << grades << '\n' << pointer << '\n' << *pointer;
   
    delete pointer;
    return 0;

}