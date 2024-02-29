#include <calculator.hpp>

int main()
{
    try
    {
        calculator c1;
        c1.calculate();
        return 0;
    }
    catch (...)
    {
        // other errors (don't try to recover)
        std::cerr << "exception\n";
        return 2;
    }
}