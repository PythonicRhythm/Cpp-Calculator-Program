
// TODO:    Adjust the token_stream class to allow for multiple input sources
//          such as files and predefined strings to allow for testing.

// TODO:    Negative Numbers

// TODO:    % (Remainer/Modulo)

// TODO:    Pre-defined symbolic values

// TODO:    Variables


#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include "catch.hpp"

// Token stuff
// Token “kind” values:
char const number = '8';    // a floating-point number
char const quit = 'q';      // an exit command
char const print = ';';     // a print command

class token
{
    char kind_;       // what kind of token
    double value_;    // for numbers: a value

public:
    // constructors
    token(char ch)
      : kind_(ch)
      , value_(0)
    {
    }
    token(double val)
      : kind_(number)    // let ‘8’ represent “a number”
      , value_(val)
    {
    }

    char kind() const
    {
        return kind_;
    }
    double value() const
    {
        return value_;
    }
};

// User interaction strings:
std::string const prompt = "> ";
std::string const result = "= ";    // indicate that a result follows

class token_stream
{
    // representation: not directly accessible to users:
    bool full;                          // is there a token in the buffer?
    token buffer;                       // here is where we keep a Token put back using
                                        // putback()
    std::istream* source = nullptr;     // source of the tokens, used for std::cin or strings given.
    
public:
    // user interface:
    token get();                            // get a token
    bool string_input;
    void putback(token);                    // put a token back into the token_stream
    void ignore(char c);                    // discard tokens up to and including a c
    void set_input(std::string const&);     // initializing source for token_stream

    // constructor: make a token_stream, the buffer starts empty
    // used for standard input.
    token_stream()
      : full(false)
      , buffer('\0')
      , source(&std::cin)
      , string_input(false)
    {
    }

    // constructor: make a token_stream, the buffer starts empty
    // used for given strings via testing.
    token_stream(std::string const& str)
      : full(false)
      , buffer('\0')
      , source(new std::istringstream(str))
      , string_input(true)
    {
    }

    //deallocating storage associated with scanner.
    // ~token_stream() {
    //     if(string_input) {
    //         delete source;
    //         source = nullptr;
    //     }
    // }

    bool has_more_tokens() 
    {
        return source->good();
    }

};

// single global instance of the token_stream
token_stream ts;

void token_stream::putback(token t)
{
    if (full)
        throw std::runtime_error("putback() into a full buffer");
    buffer = t;
    full = true;
}

token token_stream::get()    // read a token from the token_stream
{
    // check if we already have a Token ready
    if (full)
    {
        full = false;
        return buffer;
    }

    // note that >> skips whitespace (space, newline, tab, etc.)
    char ch;
    //std::cin >> ch;
    *source >> ch;
    std::cout << std::endl << "<" << ch << ">" << std::endl;

    switch (ch)
    {
    case '(':
    case ')':
    case ';':
    case 'q':
    case '+':
    case '-':
    case '*':
    case '/':
        return token(ch);    // let each character represent itself
    case '\0':
        return token('q');
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        //std::cin.putback(ch);    // put digit back into the input stream
        source->putback(ch);
        double val;
        //std::cin >> val;    // read a floating-point number
        *source >> val;
        return token(val);
    }
    default:
        //std::cout <<"["<< ch << "] [" << '\0' << "]" << std::endl;
        throw std::runtime_error("Bad token");
    }
}

// discard tokens up to and including a c
void token_stream::ignore(char c)
{
    // first look in buffer:
    if (full && c == buffer.kind())    // && means 'and'
    {
        full = false;
        return;
    }
    full = false;    // discard the contents of buffer

    // now search input:
    char ch = 0;
    //while (std::cin >> ch)
    while (*source >> ch)
    {
        if (ch == c)
            break;
    }
}

// declaration so that primary() can call expression()
double expression();

double primary()    // Number or ‘(‘ Expression ‘)’
{
    token t = ts.get();
    switch (t.kind())
    {
    case '(':    // handle ‘(’expression ‘)’
    {
        double d = expression();
        t = ts.get();
        if (t.kind() != ')')
            throw std::runtime_error("')' expected");
        return d;
    }
    case number:    // we use ‘8’ to represent the “kind” of a number
        return t.value();    // return the number’s value
    default:
        throw std::runtime_error("primary expected");
    }
}

// exactly like expression(), but for * and /
double term()
{
    double left = primary();    // get the Primary
    while (true)
    {
        token t = ts.get();    // get the next Token ...
        switch (t.kind())
        {
        case '*':
            left *= primary();
            break;
        case '/':
        {
            double d = primary();
            if (d == 0)
                throw std::runtime_error("divide by zero");
            left /= d;
            break;
        }
        default:
            ts.putback(t);    // <<< put the unused token back
            return left;      // return the value
        }
    }
}

// read and evaluate: 1   1+2.5   1+2+3.14  etc.
// 	 return the sum (or difference)
double expression()
{
    double left = term();    // get the Term
    while (true)
    {
        token t = ts.get();    // get the next token…
        switch (t.kind())      // ... and do the right thing with it
        {
        case '+':
            left += term();
            break;
        case '-':
            left -= term();
            break;
        default:
            ts.putback(t);    // <<< put the unused token back
            return left;      // return the value of the expression
        }
    }
}

void clean_up_mess()
{
    ts.ignore(print);
}

// calculate() returns a double array to provide a return value
// for the CHECK testing below. Max amount of values in one string
// given is 5.
double* calculate(std::string const& input = "")
{
    double* allValues = new double(5);
    int numOfVals = 0;
    if(input.empty()) {ts=token_stream();}
    else {ts=token_stream(input);}
    while (ts.has_more_tokens())
    {
        try
        {
            std::cout << prompt;    // print prompt
            token t = ts.get();

            // first discard all “prints”
            while (t.kind() == print)
                t = ts.get();

            if (t.kind() == quit)
                return allValues;    // ‘q’ for “quit”

            ts.putback(t);
            double numValue = expression();
            //std::cout << result << numValue << std::endl;
            //std::cout << "<" << t.kind() << "> ";
            
            // just for testing purposes
            if(numOfVals < 5) {
                allValues[numOfVals] = numValue;
                numOfVals++;
            }
            else {
                throw std::runtime_error("Only 5 expressions per string.");
            }
        }
        catch (std::runtime_error const& e)
        {
            std::cerr << e.what() << std::endl;    // write error message
            clean_up_mess();                       // <<< The tricky part!
        }
    }

    return allValues;
}

// int main()
// {
//     try
//     {
//         calculate();
//         return 0;
//     }
//     catch (...)
//     {
//         // other errors (don't try to recover)
//         std::cerr << "exception\n";
//         return 2;
//     }
// }

// function for testing if doubles are almost equal.
bool equalDouble(double const a, double const b) {
    return std::fabs(a-b) <= ( std::max(std::abs(a), std::abs(b)) * DBL_EPSILON);
}

STUDENT_TEST("Test Case 2: Testing basic problems.") {

    double* arr1 = calculate("3+5; 24+2; 1*50; 3/3; (23*2)+(23/2);");
    // should return true
    CHECK(equalDouble(arr1[0], 8) == true);
    CHECK(equalDouble(arr1[1], 26) == true);
    CHECK(equalDouble(arr1[2], 50) == true);
    CHECK(equalDouble(arr1[3], 1) == true);
    CHECK(equalDouble(arr1[4], 57.5) == true);

    // should return false
    CHECK(equalDouble(arr1[0], 7) == false);
    CHECK(equalDouble(arr1[1], 2) == false);
    CHECK(equalDouble(arr1[2], 40) == false);
    CHECK(equalDouble(arr1[3], 102) == false);
    CHECK(equalDouble(arr1[4], 7.5) == false);
    delete arr1;

    double* arr2 = calculate("(1+2)*(3-2)/(6-3); ((33+3)-(3/2)*25); (23+45)-25*(6000-23); ((23+45)-25)*(6000-23); ((23+45)-25)*((6000-23)-32*100);");
    // should return true
    CHECK(equalDouble(arr2[0], 1) == true);
    CHECK(equalDouble(arr2[1], -1.5) == true);
    CHECK(equalDouble(arr2[2], -149357) == true);
    CHECK(equalDouble(arr2[3], 257011) == true);
    CHECK(equalDouble(arr2[4], 119411) == true);
    
    // should return false
    CHECK(equalDouble(arr2[0], 3) == false);
    CHECK(equalDouble(arr2[1], -3.5) == false);
    CHECK(equalDouble(arr2[2], -357) == false);
    CHECK(equalDouble(arr2[3], 011) == false);
    CHECK(equalDouble(arr2[4], 411) == false);
    delete arr2;

}

STUDENT_TEST("Test Case 2: Testing the limits of double equality.") {
    
    double* resultArray = calculate("(23*2)+(23/2);");
    // Should pass.
    std::cout << "<" << resultArray[0] << ">" << std::endl;
    CHECK(equalDouble(resultArray[0], 57.50));
    // Testing the limits of equalDouble
    CHECK(!equalDouble(resultArray[0], 57.55));
    CHECK(!equalDouble(resultArray[0], 57.51));
    CHECK(!equalDouble(resultArray[0], 57.501));
    CHECK(!equalDouble(resultArray[0], 57.5001));
    CHECK(!equalDouble(resultArray[0], 57.50001));
    CHECK(!equalDouble(resultArray[0], 57.500001));
    CHECK(!equalDouble(resultArray[0], 57.5000001));
    CHECK(!equalDouble(resultArray[0], 57.50000001));
    CHECK(!equalDouble(resultArray[0], 57.500000001));
    CHECK(!equalDouble(resultArray[0], 57.5000000001));
    CHECK(!equalDouble(resultArray[0], 57.50000000001));
    CHECK(!equalDouble(resultArray[0], 57.500000000001));
}
