
// TODO:    Adjust the token_stream class to allow for multiple input sources
//          such as files and predefined strings to allow for testing. (COMPLETED)

// TODO:    Negative Numbers (COMPLETED)

// TODO:    % (Remainer/Modulo)

// TODO:    Pre-defined symbolic values

// TODO:    Variables


#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <string>
#include "catch.hpp"


#define NOERR               0
#define PUTBACKFULL         1
#define BADTOKEN            2
#define MISSINGPARENTH      3
#define MISSINGPRIMARY      4
#define DIVIDEBYZERO        5
#define MODBYZERO           6
#define MAXEXPRESSIONS      7

// program error value used for testing
int calc_err = NOERR;

// Token stuff
// Token “kind” values:
char const number = '8';    // a floating-point number
char const quit = 'q';      // an exit command
char const print = ';';     // a print command
char const nullTerm = '\0'; // null term for strings

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
    bool full;       // is there a token in the buffer?
    token buffer;    // here is where we keep a Token put back using
                     // putback()
    std::istream* source = nullptr;

public:
    // user interface:
    token get();            // get a token
    void putback(token);    // put a token back into the token_stream
    void ignore(char c);    // discard tokens up to and including a c

    // constructor: make a token_stream, the buffer starts empty
    token_stream()
      : full(false)
      , buffer('\0')
      , source(&std::cin)
    {
    }

    token_stream(std::string const& str)
      : full(false)
      , buffer('\0')
    {
        set_source(str);
    }

    void set_source(std::string const& str)
    {
        source = new std::istringstream (str);
;
    }

    bool has_more_tokens()
    {
        return source->good();
    }
};

void token_stream::putback(token t)
{
    if (full) 
    {
        calc_err = PUTBACKFULL;
        throw std::runtime_error("putback() into a full buffer");
    }
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
    source->clear();

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
    case '%':
    case '\0':
        return token(ch);    // let each character represent itself
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
        calc_err = BADTOKEN;
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
    while(*source >> ch)
    {
        if (ch == c)
            break;
    }
}

class calculator {
    
    token_stream ts;
    double primary();
    double term();
    double expression();
    void clean_up_mess();

public:
    std::vector<double> calculate(std::string const& commands = "");

};

double calculator::primary()    // Number or ‘(‘ Expression ‘)’
{
    token t = ts.get();
    switch (t.kind())
    {
    case '(':    // handle ‘(’expression ‘)’
    {
        double d = expression();
        t = ts.get();
        if (t.kind() != ')')
        {
            calc_err = MISSINGPARENTH;
            throw std::runtime_error("')' expected");
        }
        return d;
    }
    case number:    // we use ‘8’ to represent the “kind” of a number
        return t.value();    // return the number’s value
    case '-':
        return -primary();
    default:
        calc_err = MISSINGPRIMARY;
        throw std::runtime_error("primary expected");
    }
}

// exactly like expression(), but for * and /
double calculator::term()
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
            {
                calc_err = DIVIDEBYZERO;
                throw std::runtime_error("divide by zero");
            }
            left /= d;
            break;
        }
        case '%':
        {
            double d = primary();
            if(d == 0)
            {
                calc_err = MODBYZERO;
                throw std::runtime_error("mod by zero");
            }
            left = (int)left % (int)d;
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
double calculator::expression()
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

void calculator::clean_up_mess()
{
    ts.ignore(print);
}

// Return set to vector<double> for testing purposes. The return vector's
// elements will be tested to confirm if results are what they are supposed to be.
std::vector<double> calculator::calculate(std::string const& commands)
{
    std::vector<double> allVals(5);
    int numOfVals = 0;
    if(!commands.empty()) ts = token_stream(commands);
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
                return allVals;    // ‘q’ for “quit”
            
            if (t.kind() == nullTerm)
            {
                std::cout << "Command String read..." << std::endl;
                continue;
            }
            ts.putback(t);

            double val = expression();
            std::cout << result << val << std::endl;

            // Used for testing purposes. Gathers results from each expression.
            if(numOfVals < allVals.capacity()) allVals[numOfVals] = val;
            else 
            {
                calc_err = MAXEXPRESSIONS;
                throw std::runtime_error("Only 5 expressions per string.");
            }
            numOfVals++;
        }
        catch (std::runtime_error const& e)
        {
            std::cerr << e.what() << std::endl;    // write error message
            clean_up_mess();                       // <<< The tricky part!
        }
    }

    return allVals;
}

// function for testing if doubles are almost equal.
bool equalDouble(double const a, double const b) {
    return std::fabs(a-b) <= ( std::max(std::abs(a), std::abs(b)) * DBL_EPSILON);
}

STUDENT_TEST("Test Case 1: Testing formatting and reading.") {

    calculator c;
    // std::vector<double>arr1 = c.calculate(" ; ;; ;;; ;;;; ;;;;;"); // reads all prints and returns <0,0,0,0,0>
    
    // CHECK(equalDouble(arr1[0], 0) == true);
    // CHECK(equalDouble(arr1[1], 0) == true);
    // CHECK(equalDouble(arr1[2], 0) == true);
    // CHECK(equalDouble(arr1[3], 0) == true);
    // CHECK(equalDouble(arr1[4], 0) == true);

    // std::vector<double>arr2 = c.calculate("q"); // should close program and return <0,0,0,0,0>
    
    // CHECK(equalDouble(arr2[0], 0) == true);
    // CHECK(equalDouble(arr2[1], 0) == true);
    // CHECK(equalDouble(arr2[2], 0) == true);
    // CHECK(equalDouble(arr2[3], 0) == true);
    // CHECK(equalDouble(arr2[4], 0) == true);

    // std::vector<double>arr3 = c.calculate(" 1; 1;; 1;;; 1;;;; 1;;;;;"); // should return <1,1,1,1,1>
    
    // CHECK(equalDouble(arr3[0], 1) == true);
    // CHECK(equalDouble(arr3[1], 1) == true);
    // CHECK(equalDouble(arr3[2], 1) == true);
    // CHECK(equalDouble(arr3[3], 1) == true);
    // CHECK(equalDouble(arr3[4], 1) == true);

    // std::vector<double>arr4 = c.calculate(" 1; 1;; 1;;;q 1;;;; q;;;;;"); // should return <1,1,1,0,0> 
    
    // CHECK(equalDouble(arr4[0], 1) == true);
    // CHECK(equalDouble(arr4[1], 1) == true);
    // CHECK(equalDouble(arr4[2], 1) == true);
    // CHECK(equalDouble(arr4[3], 0) == true);
    // CHECK(equalDouble(arr4[4], 0) == true);

    // std::vector<double>arr5 = c.calculate("1;;;;;;;;;;;;;;;;;;;;;;;;;;;;q"); // should return <1,0,0,0,0> 
    
    // CHECK(equalDouble(arr5[0], 1) == true);
    // CHECK(equalDouble(arr5[1], 0) == true);
    // CHECK(equalDouble(arr5[2], 0) == true);
    // CHECK(equalDouble(arr5[3], 0) == true);
    // CHECK(equalDouble(arr5[4], 0) == true);
    
    // std::vector<double>arr6 = c.calculate("q 25; 24+2; 300%10; 26/0; 234-2;"); // should return <0,0,0,0,0> 
    
    // CHECK(equalDouble(arr6[0], 0) == true);
    // CHECK(equalDouble(arr6[1], 0) == true);
    // CHECK(equalDouble(arr6[2], 0) == true);
    // CHECK(equalDouble(arr6[3], 0) == true);
    // CHECK(equalDouble(arr6[4], 0) == true);

    // std::vector<double>arr7 = c.calculate("23;23;34;6;               23;"); // should return <23,23,34,6,23> 
    
    // CHECK(equalDouble(arr7[0], 0) == true);
    // CHECK(equalDouble(arr7[1], 0) == true);
    // CHECK(equalDouble(arr7[2], 0) == true);
    // CHECK(equalDouble(arr7[3], 0) == true);
    // CHECK(equalDouble(arr7[4], 0) == true);
    
}

STUDENT_TEST("Test Case 2: Testing basic problems.") {

    calculator c;
    std::vector<double>arr1 = c.calculate("3+5; 24+2; 1*50; 3/3; (23*2)+(23/2);");
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

    std::vector<double>arr2 = c.calculate("(1+2)*(3-2)/(6-3); ((33+3)-(3/2)*25); (23+45)-25*(6000-23); ((23+45)-25)*(6000-23); ((23+45)-25)*((6000-23)-32*100);");
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

    std::vector<double>arr3 = c.calculate("(((34*3)))-(25-25)*10; 34*3*3*2; 3/3/3/3; 25; 32-2000;");
    // should return true
    CHECK(equalDouble(arr3[0], 102) == true);
    CHECK(equalDouble(arr3[1], 612) == true);
    CHECK(equalDouble(arr3[2], 0.1111111111111111111111111111) == true);
    CHECK(equalDouble(arr3[3], 25) == true);
    CHECK(equalDouble(arr3[4], -1968) == true);

    CHECK(equalDouble(arr3[0], 0.11111121212) == false);
    CHECK(equalDouble(arr3[1], -6142) == false);
    CHECK(equalDouble(arr3[2], -1/9) == false);
    CHECK(equalDouble(arr3[3], 25-25) == false);
    CHECK(equalDouble(arr3[4], -19.68) == false);
}

STUDENT_TEST("Test Case 3: Testing the limits of double equality.") {
    
    calculator c;
    std::vector<double> resultArray = c.calculate("(23*2)+(23/2);");
    // Should pass.
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

STUDENT_TEST("Test Case 4: Testing Feature 1, Negative Numbers.") {

    calculator c;
    std::vector<double> arr1 = c.calculate("(-15+-2); (-15+2); -(-15+-2); --(-15+-2); -(15+(-2));");

    CHECK(equalDouble(arr1[0], -17) == true);
    CHECK(equalDouble(arr1[1], -13) == true);
    CHECK(equalDouble(arr1[2], 17) == true);
    CHECK(equalDouble(arr1[3], -17) == true);
    CHECK(equalDouble(arr1[4], -13) == true);

    std::vector<double> arr2 = c.calculate("-(-2300-2300); -1-1-1; (--1); -42+42--36-3+(---3); (-(-(4)));");

    CHECK(equalDouble(arr2[0], 4600) == true);
    CHECK(equalDouble(arr2[1], -3) == true);
    CHECK(equalDouble(arr2[2], 1) == true);
    CHECK(equalDouble(arr2[3], 30) == true);
    CHECK(equalDouble(arr2[4], 4) == true);
}

STUDENT_TEST("Test Case 5: Testing Feature 2, Modulus Operations.") {

    calculator c;
    std::vector<double> arr1 = c.calculate("25%3; (3%3)%3\%10; (((((25)))%5+5)%5+10)%2; (((32-2)%(34-24)+10)%2)+13%2; ((360%45)+27)%4;");

    CHECK(equalDouble(arr1[0], 1) == true);
    CHECK(equalDouble(arr1[1], 0) == true);
    CHECK(equalDouble(arr1[2], 0) == true);
    CHECK(equalDouble(arr1[3], 1) == true);
    CHECK(equalDouble(arr1[4], 3) == true);

    std::vector<double> arr2 = c.calculate("(((34+64)%7)+17)%3; (45+5+2)%6; -(34)%12; 64-(54%24); (25%25)+(10%3)+(20%6);");

    CHECK(equalDouble(arr2[0], 2) == true);
    CHECK(equalDouble(arr2[1], 4) == true);
    CHECK(equalDouble(arr2[2], -10) == true);
    CHECK(equalDouble(arr2[3], 58) == true);
    CHECK(equalDouble(arr2[4], 3) == true);
}
