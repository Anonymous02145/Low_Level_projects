#include <iostream>

void walk(int steps);

int main() {

    walk(6);

    return 0; 

}

void walk(int steps){

    if (steps > 0) {
        walk(steps - 1);
        std::cout << "Walked\n" << std::flush;
    
    }
}