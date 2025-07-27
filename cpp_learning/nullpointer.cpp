#include <iostream>


int main() {

    int *ptr = nullptr;

    int x = 3;

    ptr = &x;

    if (ptr != nullptr) {
        std::cout << "Adress assigned : " << &x; 
    }
    else {
        std::cout << "Not assigned"; 
    }


   return 0; 

}