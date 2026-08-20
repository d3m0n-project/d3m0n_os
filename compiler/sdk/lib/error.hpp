#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>

using namespace std;

class NegativeValueException : public exception {
private:
    int value;
public:
    NegativeValueException(int val) : value(val) {}

    const char* what() const noexcept override {
        return "Negative value error occurred!";
    }

    int getValue() const {
        return value;
    }
};

#endif