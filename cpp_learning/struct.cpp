#include <iostream>

struct student{
    std::string name;
    int age;
    int grade;
    bool entroled;
};

void printstudent(student &student);

int main() {

    student student1;

    student1.age = 16;
    student1.name = "Aarush";
    student1.entroled = true;
    student1.grade = 12;

    printstudent(student1);

    return 0; 

}
void printstudent(student &student){
    std::cout << student.age << '\n';
    std::cout << student.name << '\n';
    std::cout << student.entroled << '\n';
    std::cout << student.grade << '\n';

}