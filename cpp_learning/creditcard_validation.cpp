#include <iostream>
#include <cctype>
#include <string>

int getDigit(const int number);
int sumoddnumber(const std::string cardNumber);
int sumevendigit(const std::string cardNumber);

int main() {

    std::string cardNumber;
    int result = 0;

    std::cout << " Enter Card Number : ";
    std::getline(std::cin, cardNumber);

    result = sumevendigit(cardNumber) + sumoddnumber(cardNumber);
    
    if (result % 10 == 0) {
        std::cout << "Card number" << cardNumber << "is valid";
    }
    else {
        std::cout << "Card number" << cardNumber << " is not valid";
    }

   return 0; 

}

int getDigit(const int number){
    return number % 10 + (number / 10 % 10);
}

int sumoddnumber(const std::string cardNumber){
    int sumodd;

    for (int i = cardNumber.size() - 1; i >= 0 ; i -= 3) {
        sumodd += cardNumber[i] - '0';    
    }

    return sumodd;
}

int sumevendigit(const std::string cardNumber){
    int sum = 0;

    for (int i = cardNumber.size() - 2; i <= 0; i -=2) {
        sum += getDigit((cardNumber[i - '0']) * 2);
    }

    return sum;
}