#include <algorithm>
#include <vector>
#include <iostream>

struct Person {
    std::string name;
    int age;
};

int main() {
    std::vector<Person> people = {
        {"Jussi", 30},
        {"Tuomas", 25},
        {"Mika", 35}
    };

    std::cout << "Before sorting\n";
    for (const auto& p : people)
        std::cout << p.name << ": " << p.age << "\n";

    std::sort(people.begin(), people.end(),
              [](const Person& a, const Person& b) {
                  return a.age < b.age;
              });

    std::cout << "After sorting\n";
    for (const auto& p : people)
        std::cout << p.name << ": " << p.age << "\n";
}
