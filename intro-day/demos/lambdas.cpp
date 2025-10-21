#include <iostream>


int main(int argc, char *argv[])
{
    int a = 1, b = 2, c = 3;
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    // Capture `a` by value
    auto func1 = [a](int x) { return a + x; };
    c = func1(4);  // 5
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    a = -1;
    c = func1(4);  // 5
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    // Capture to a new variable
    auto func2 = [d = 2*a](int x) { return d + x; };
    c = func2(4);  // 2
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    // This will fail; `b` not captured
//    auto func3 = [a](int x) { return b + x; };

    // Capture everything by value
    auto func3 = [=](int x) { return b + x; };
    c = func3(4);  // 6
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    // Capture `b` by reference
    auto func4 = [&b](int x) { return b + x; };
    c = func4(4);  // 6
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    b = -2;
    c = func4(4);  // 2
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    // Capture everything by reference
    auto func5 = [&](int x) { a = x; b = -x; };
    func5(4);  // a = 4, b = -4
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    // Mix and match
    auto func6 = [=,&b](int x) { return a + b + x; };
    c = func6(4);  // 4
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    a = b = 0;
    c = func6(4);  // 8
    std::cout << "line " << __LINE__ << ": a = " << a << ", b = " << b << ", c = " << c << std::endl;

    return 0;
}
