#include <iostream>


void swap(std::string X, std::string Y);


int main() {

    std::string x = "Orange ";
    std::string y = "Apple ";
    swap(x, y);
    std::cout << '\n';
    swap(y, x);

   return 0; 

}

void swap(std::string &X, std::string &Y){
    std::string temp;

    temp = X;
    X = Y;
    Y = temp;

    std::cout << " X : " << X;
    std::cout << " Y : " << Y;
}