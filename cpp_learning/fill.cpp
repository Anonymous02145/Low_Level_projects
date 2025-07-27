
#include <iostream>
#include <string>


int main(){
    using std::string;

    string array[10];
    std::fill(array, array + 10, "$");

    for (string element : array) {
        std::cout << element << " ";
    }
    
    return 0;


}