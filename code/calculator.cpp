
// (COMPLETED)  TODO:   Fix std::cin/string read issues. Currently '\n', '\t' and others lock the program if
//                      they are at the end of the string/cin

// (COMPLETED)  TODO:   Adjust the token_stream class to allow for multiple input sources
//                      such as files and predefined strings to allow for testing.

// (COMPLETED)  TODO:   Negative Numbers

// (COMPLETED)  TODO:   % (Remainer/Modulo)

// TODO:    Pre-defined symbolic values

// (COMPLETED)  TODO:    Variables



#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <string>
#include "catch.hpp"

// calculator program errors constants.
#define NOERR               0
#define PUTBACKFULL         1
#define BADTOKEN            2
#define MISSINGPARENTH      3
#define MISSINGPRIMARY      4
#define MISSINGPRINT        5
#define DIVIDEBYZERO        6
#define MODBYZERO           7
#define MAXEXPRESSIONS      8

// program error value used for testing
int calc_err = NOERR;

// Token stuff
// Token “kind” values:
char const number = '8';    // a floating-point number
char const quit = 'q';      // an exit command
char const print = ';';     // a print command
char const nullTerm = '\0'; // null term for strings
char const newline = '\n';  // newline character

// User interaction strings:
std::string const prompt = "> ";
std::string const result = "= ";    // indicate that a result follows

// The "token" class represents a token gathered from the input stream
// used in "token_stream".
// Ex: string = "1 + 23;"
//      token num = token('1');     ()
//      num.kind() == '8';          (token that represents a number)
//      num.value() == 1;           (the value of the number token)
class token
{
    char kind_;       // what kind of token
    double value_;    // for numbers: a value

public:
    // user interface
    char kind() const;
    double value() const;
    void set_value(double);
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
};

// kind() will return the type of token it is.
// returning '8' would be a number token | returning 'q' would be a quit token, etc.
char token::kind() const
{
    return kind_;
}

// value() will return the numerical value of the token.
// Ex:  token t = token(25);
//      return t.value(); (t.value()) would be 25
double token::value() const
{
    return value_;
}

// set_value() will redefine the value_ variable in the token
// using the incoming newVal variable. 
void token::set_value(double newVal)
{
    value_ = newVal;
}

// The "token_stream" class represents an input stream and tools
// used to read this stream via tokens with "token" class. 
class token_stream
{
    // representation: not directly accessible to users:
    bool full;                          // is there a token in the buffer?
    token buffer;                       // here is where we keep a Token put back using
                                        // putback()
    std::istream* source = nullptr;     // the source of the stream of characters.
    bool string_input;                  // is the source a string input?

public:
    // user interface:
    token get();                            // get a token
    void putback(token);                    // put a token back into the token_stream
    void ignore(char c);                    // discard tokens up to and including a c
    void set_source(std::string const&);    // declaring another stream source after initial construction
    bool has_more_tokens();                 // returns if istream source has reached EOF

    // constructor: make a token_stream, the buffer starts empty
    token_stream()
      : full(false)
      , buffer(nullTerm)
      , source(&std::cin)
      , string_input(false)
    {
    }

    // constructor: unused for now. plans for creating a library for this calculator.
    token_stream(std::string const& str)
      : full(false)
      , buffer(nullTerm)
      , string_input(true)
    {
        set_source(str);
    }       

};

// set_source() allows declaring another stream source after initial construction. 
void token_stream::set_source(std::string const& str)
{
    source = new std::istringstream (str);
}

// has_more_tokens() allows token_stream to keep reading while the token_stream has not encountered EOF.
bool token_stream::has_more_tokens()
{
    return source->good();
}

// putback() inserts a token into a single slot buffer.
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

// get() reads a token from the istream* source.
token token_stream::get()
{
    // check if we already have a Token ready
    if (full)
    {
        full = false;
        return buffer;
    }

    // note that >> skips whitespace (space, newline, tab, etc.)
    char ch;

    // Code below is used to allow for cleaning up errors on the behalf of the user.
    if(source->eof()) return token(nullTerm);   // if at end of file, return nullterm... should lead to end of program.
    
    // for std::cin only, terminal input
    if(!string_input) 
    {
        // clean up whitespace until you find non-ws token or you find newline char.
        while(isspace(source->peek()))
        {
            if(source->peek() == newline)
            {
                source->get();
                return token(newline);
            }
            source->get();
            
        }
    }
    
    *source >> ch;

    if(isalpha(ch)) return token(ch);

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
    case '=':
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
        source->putback(ch);
        double val;
        *source >> val;
        return token(val);
    }
    default:
        calc_err = BADTOKEN;
        throw std::runtime_error("Bad Token");
    }
}

// ignore() discard tokens up to and including a c
void token_stream::ignore(char c)
{
    // first look in buffer:
    if (full && buffer.kind() == c) // if buffer contains 'c', delete it and return.
    {
        // checks if the expression that failed is the last one.
        // if so, cleans out all whitespace.
        full = false;
        char ch = source->get();
        while(isspace(ch)) {
            if(ch == newline) return;
            ch = source->get();
        }

        if(source->eof()) return;
        
        // if there is another expression, putsback token and program should continue onward.
        source->putback(ch);
        return;
    }
    else if (full && buffer.kind() == newline)
    {
        full = false;
        return;
    }
    else if (full && buffer.kind() == number)
    {
        return;
    }
    
    full = false;    // discard the contents of buffer

    // now search input src for c and delete until c, '\n', EOF is found
    char ch = source->get();
    while(ch != c)
    {
        if(ch == newline) return;
        if(source->eof()) return;
        ch = source->get();
    }

    source->get();

}

// The "calculator" class simulates a real calculator using the "token_stream"
// class to gather input such as variables, numbers, operators, parenthesis, etc.
class calculator {
    token_stream ts;
    std::vector<token> userVars;
    double primary();
    double term();
    double expression();
    void clean_up_mess(char const&);
    void set_variable();
    bool variable_exists(char const&);

public:
    std::vector<double> calculate(std::string const& commands = "");

};

// primary() represents a number, negative sign or ‘(‘ expression ‘)’
double calculator::primary()
{
    token t = ts.get();

    switch (t.kind())
    {
        case '(':    // handle ‘(’ expression ‘)’
        {
            double d = expression();
            t = ts.get();
            if (t.kind() != ')')
            {
                calc_err = MISSINGPARENTH;
                ts.putback(t);
                throw std::runtime_error("')' expected");
            }
            return d;
        }
        case number:    // we use ‘8’ to represent the “kind” of a number
            return t.value();    // return the number’s value
        case '-':
            return -primary();
        default:
        {
            if(isalpha(t.kind()))
            {

                // if the token is an existing variable, return its value.
                // finding existing variable should be made into a function.
                for(auto& tok: userVars) {
                    if(tok.kind() == t.kind())
                    {
                        t.set_value(tok.value());
                        return tok.value();
                    }
                }

                // else its a bad token
                calc_err = BADTOKEN;
                ts.putback(t);
                throw std::runtime_error("Bad Token");
            }
            else{
                // if its not a letter, number, '-', or '(', then throw excep
                calc_err = MISSINGPRIMARY;
                ts.putback(t);
                throw std::runtime_error("primary ( '(', {number}, '-' ) expected");
            }
        }
    }
}

// term() represents the *, /, %, operators.
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
                throw std::runtime_error("cannot divide by zero");
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
                throw std::runtime_error("cannot mod by zero");
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

// expression() represents the +, -, operators.
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

// clean_up_mess() calls the ignore() function for token_stream ts
// to handle exceptions during runtime.
void calculator::clean_up_mess(char const& c)
{
    ts.ignore(c);
}

// set_variable() begins the process to create and save a variable into
// the userVars vector.
// example: "let x = 5;"
void calculator::set_variable()
{
    // NOTE:    syntax for declaring a new variable is...
    //          "let x = 5;"
    //
    // new_variable() will attempt to check for syntax errors
    // along the way, such as a missing 't' for "let" or
    // forgetting the '=' operator.

    // check if 'e' comes after 'l'
    token t = ts.get();
    if(t.kind() != 'e')
    {
        // if not throw error showing what token was there instead
        calc_err = BADTOKEN;
        // below leads to undefined behavior but i want to get it to work... idk why its undefined
            // std::string c;
            // if(t.kind() == number) c.append(std::to_string(t.value()));
            // else c = std::string(1, t.kind());
        throw std::runtime_error("\"let\" expected");
    }
    
    // checks if 't' comes after 'e'
    t = ts.get();
    if(t.kind() != 't')
    {
        calc_err = BADTOKEN;
        // below leads to undefined behavior but i want to get it to work... idk why its undefined
            // std::string c;
            // if(t.kind() == number) c.append(std::to_string(t.value()));
            // else c = std::string(1, t.kind());
        throw std::runtime_error("\"let\" expected");
    }

    // checks if variable name comes after 't'
    token var = ts.get();
    if(isalpha(var.kind()))
    {
        // checks if variable name given is longer than one letter.
        token next = ts.get();
        if(isalpha(next.kind()))
        {
            calc_err = BADTOKEN;
            // below leads to undefined behavior but i want to get it to work... idk why its undefined
                // std::string c1 (1, var.kind());
                // std::string c2 (1, next.kind());
            throw std::runtime_error("one letter variable name expected");
        }
        t = next;

        // checks if already exists and doesnt allow for overwrite for right now.
        if(variable_exists(var.kind()))
        {
            calc_err = BADTOKEN;
            throw std::runtime_error("variable name already exists, no overwriting");
        }

    }
    else {
        // if not alphabetical then must be a mistake
        calc_err = BADTOKEN;
        // below leads to undefined behavior but i want to get it to work... idk why its undefined
            // std::string c;
            // if(var.kind() == number) c.append(std::to_string(var.value()));
            // else c = std::string(1, var.kind());
        throw std::runtime_error("variable name expected");
    }

    // checks if '=' comes after variable name
    if(t.kind() != '=')
    {
        // if not, throw error showing what was there instead
        calc_err = BADTOKEN;
        // below leads to undefined behavior but i want to get it to work... idk why its undefined
            // std::string c;
            // if(t.kind() == number) c.append(std::to_string(t.value()));
            // else c = std::string(1, t.kind());
        throw std::runtime_error("'=' expected to complete variable");
    }

    // gathers an expression which will equal variable value.
    double varValue = expression();
    var.set_value(varValue);

    // checks if ';' character comes after value
    t = ts.get();
    if(t.kind() != print)
    {
        // if not, throw error showing what was there instead
        calc_err = MISSINGPRINT;
        // below leads to undefined behavior but i want to get it to work... idk why its undefined
            // std::string c;
            // if(t.kind() == number) c.append(std::to_string(t.value()));
            // else c = std::string(1, t.kind());
        throw std::runtime_error("';' expected");
    }

    // variable passed all checks so it is set as a user variable
    userVars.push_back(var);
    std::cout << "Variable Created!" << std::endl;

    // if '\n' is all that is left... remove it ... else pushback()
    t = ts.get();
    if(t.kind() != '\n') ts.putback(t);
}

// variable_exists() checks if a variable exists
// with the character given.
bool calculator::variable_exists(char const& c)
{
    for(auto& tok: userVars)
    {
        if(tok.kind() == c)
            return true;
    }

    return false;
}

// calculate() begins the simulated calculator program and
// also returns a vector<double> for testing purposes. The 
// return vector's elements will be tested to confirm if 
// results for each expression are what they are supposed to be.
// NOTE: 
//      1.  vector<double> allVals elements default to 0. <0,0,0,0,0>
//      2.  if an expression throws an exception it does not count for
//          the expression counter 'numOfvals' and it will not be assigned
//          a spot in the allVals vector. Program will move to next expression.
std::vector<double> calculator::calculate(std::string const& commands)
{
    // Used for testing purposes.
    std::vector<double> allVals(5); // returns the calculated results as a vector.
    int numOfVals = 0;              // keeps track of the amount of values per string argument.
    calc_err = NOERR;

    // if a string was given ... set as source stream
    // else string_source remains false.
    bool string_source = false;
    if(!commands.empty())
    {
        string_source = true;
        ts = token_stream(commands);
    }

    // main calculator loop
    while (ts.has_more_tokens())
    {
        try
        {
            std::cout << prompt;    // print prompt
            token t = ts.get();

            // first discard all “prints”
            while (t.kind() == print)
                t = ts.get();

            // check if user typed ‘q’ for “quit”
            if (t.kind() == quit)
                return allVals;    
            
            // check if we reached the end of string.
            if (t.kind() == nullTerm)
            {
                std::cout << "Command String read..." << std::endl;
                return allVals;
            }

            // if token is a letter, check if its a variable or symbolic value
            if (isalpha(t.kind()))
            {
                if(t.kind() == 'l')
                {
                    set_variable();
                    continue;
                }
            }

            // check if we reached a newline char
            if (t.kind() == '\n')
                continue;
            
            // token is something useful like number, parenth, operator, etc.
            // lets try to make an expression with it.
            ts.putback(t);
            double val = expression();
            t = ts.get();

            // Used to check if user actually used a print message at the end of the expression as intended.
            if(t.kind() != print) 
            {
                calc_err = MISSINGPRINT;
                ts.putback(t);
                throw std::runtime_error("';' expected");
            }

            // Used for testing purposes. ONLY STRINGS
            // Gathers results from each expression and adds to allVals vector.
            // if max number of expressions per line is met, prints error message and returns vector.
            if(string_source)
            {
                if(numOfVals < allVals.size()) allVals[numOfVals] = val;
                else 
                {
                    calc_err = MAXEXPRESSIONS;
                    std::cerr << "Too many expressions in one line. Five only" << std::endl;
                    return allVals;
                }
                numOfVals++;
            }

            // printing end result per expression to terminal
            std::cout << result << val << std::endl;

            // if '\n' is all that is left... remove it ... else pushback()
            t = ts.get();
            if(t.kind() != '\n') ts.putback(t);
        }
        // any runtime error will be caught and we clean
        // up the expression that caused the exception. 
        catch (std::runtime_error const& e)
        {
            std::cerr << e.what() << std::endl;    // write error message
            clean_up_mess(print);                  // <<< The tricky part!
        }
    }

    return allVals;
}

// equalDouble function is used for testing if doubles are essentially equal.
bool equalDouble(double const a, double const b) {
    return std::fabs(a-b) <= ( std::max(std::abs(a), std::abs(b)) * DBL_EPSILON);
}

// Testing for formatting errors on the user's end.
// TODO: more assertions
TEST_CASE("Test Case 1: Testing formatting and reading.") 
{
    calculator c;
    std::vector<double>arr1 = c.calculate(" ; ;; ;;; ;;;; ;;;;;"); // reads all prints and returns <0,0,0,0,0>
    
    CHECK(equalDouble(arr1[0], 0) == true);
    CHECK(equalDouble(arr1[1], 0) == true);
    CHECK(equalDouble(arr1[2], 0) == true);
    CHECK(equalDouble(arr1[3], 0) == true);
    CHECK(equalDouble(arr1[4], 0) == true);

    std::vector<double>arr2 = c.calculate("q"); // should close program and return <0,0,0,0,0>
    
    CHECK(equalDouble(arr2[0], 0) == true);
    CHECK(equalDouble(arr2[1], 0) == true);
    CHECK(equalDouble(arr2[2], 0) == true);
    CHECK(equalDouble(arr2[3], 0) == true);
    CHECK(equalDouble(arr2[4], 0) == true);

    std::vector<double>arr3 = c.calculate(" 1; 1;; 1;;; 1;;;; 1;;;;;"); // should return <1,1,1,1,1>
    
    CHECK(equalDouble(arr3[0], 1) == true);
    CHECK(equalDouble(arr3[1], 1) == true);
    CHECK(equalDouble(arr3[2], 1) == true);
    CHECK(equalDouble(arr3[3], 1) == true);
    CHECK(equalDouble(arr3[4], 1) == true);

    std::vector<double>arr4 = c.calculate(" 1; 1;; 1;;;q 1;;;; q;;;;;"); // should return <1,1,1,0,0> 
    
    CHECK(equalDouble(arr4[0], 1) == true);
    CHECK(equalDouble(arr4[1], 1) == true);
    CHECK(equalDouble(arr4[2], 1) == true);
    CHECK(equalDouble(arr4[3], 0) == true);
    CHECK(equalDouble(arr4[4], 0) == true);

    std::vector<double>arr5 = c.calculate("1;;;;;;;;;;;;;;;;;;;;;;;;;;;;q"); // should return <1,0,0,0,0> 
    
    CHECK(equalDouble(arr5[0], 1) == true);
    CHECK(equalDouble(arr5[1], 0) == true);
    CHECK(equalDouble(arr5[2], 0) == true);
    CHECK(equalDouble(arr5[3], 0) == true);
    CHECK(equalDouble(arr5[4], 0) == true);
    
    std::vector<double>arr6 = c.calculate("q 25; 24+2; 300%10; 26/0; 234-2;"); // should return <0,0,0,0,0> 
    
    CHECK(equalDouble(arr6[0], 0) == true);
    CHECK(equalDouble(arr6[1], 0) == true);
    CHECK(equalDouble(arr6[2], 0) == true);
    CHECK(equalDouble(arr6[3], 0) == true);
    CHECK(equalDouble(arr6[4], 0) == true);

    std::vector<double>arr7 = c.calculate("23;23;34;6;               23;"); // should return <23,23,34,6,23> 
    
    CHECK(equalDouble(arr7[0], 23) == true);
    CHECK(equalDouble(arr7[1], 23) == true);
    CHECK(equalDouble(arr7[2], 34) == true);
    CHECK(equalDouble(arr7[3], 6) == true);
    CHECK(equalDouble(arr7[4], 23) == true);

    std::vector<double>arr8 = c.calculate(" 35+25; 35+25; 35+25; 35+25; 35+25; 35+25; 35+25; 35+25; 35+25; 35+25; 35+25; 35+25;"); // should return only <60,60,60,60,60>

    CHECK(equalDouble(arr8[0], 60) == true);
    CHECK(equalDouble(arr8[1], 60) == true);
    CHECK(equalDouble(arr8[2], 60) == true);
    CHECK(equalDouble(arr8[3], 60) == true);
    CHECK(equalDouble(arr8[4], 60) == true);
    CHECK(arr8.size() == 5);

    // fake test that should fail just to see std output on test run.
    //CHECK(arr7[4] == 0);
    
}

// Testing functionality of calculator via basic math problems.
// TODO: more assertions
TEST_CASE("Test Case 2: Testing basic problems.") 
{
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

// Testing the limits of double equality.
TEST_CASE("Test Case 3: Testing the limits of double equality.") 
{
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
    CHECK(!equalDouble(resultArray[0], 57.5000000000001));

    // They are similar enough and will pass an equality check.
    CHECK(equalDouble(resultArray[0], 57.50000000000001));
    CHECK(equalDouble(resultArray[0], 57.500000000000001));
    CHECK(equalDouble(resultArray[0], 57.5000000000000001));
    CHECK(equalDouble(resultArray[0], 57.50000000000000001));
    CHECK(equalDouble(resultArray[0], 57.500000000000000001));
    CHECK(equalDouble(resultArray[0], 57.5000000000000000001));
    CHECK(equalDouble(resultArray[0], 57.50000000000000000001));
    CHECK(equalDouble(resultArray[0], 57.500000000000000000001));
}

// Testing Feature 1: Negative Numbers.
// TODO: more assertions
TEST_CASE("Test Case 4: Testing Feature 1, Negative Numbers.") 
{
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

// Testing Feature 2: Modulus/Remainder Operations.
// TODO: more assertions
TEST_CASE("Test Case 5: Testing Feature 2, Modulus Operations.") 
{
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

// Testing if calculator is throwing errors when it's supposed to.
TEST_CASE("Test Case 6: Testing error messages.") 
{
    calculator c;
    std::vector<double> arr1;

    arr1 = c.calculate("(23-23); (430 - 400);");
    CHECK(equalDouble(arr1[0], 0) == true);
    CHECK(equalDouble(arr1[1], 30) == true);
    CHECK(calc_err == NOERR);

    arr1 = c.calculate("(300-(200+400)*32); (300-230-400)*(40*(30+2));");
    CHECK(equalDouble(arr1[0], -18900) == true);
    CHECK(equalDouble(arr1[1], -422400) == true);
    CHECK(calc_err == NOERR);

    arr1 = c.calculate("-(x-2); -(300) + 290;");
    CHECK(calc_err == BADTOKEN);
    CHECK(equalDouble(arr1[0], -10) == true);

    arr1 = c.calculate("(300-2)*x; 29-x; 900-600;");
    CHECK(calc_err == BADTOKEN);
    CHECK(equalDouble(arr1[0], 300) == true);

    arr1 = c.calculate("(230-300]); 400-200;");
    CHECK(calc_err == BADTOKEN);
    CHECK(equalDouble(arr1[0], 200) == true);

    arr1 = c.calculate("'300-2'; 200-300;");
    CHECK(calc_err == BADTOKEN);
    CHECK(equalDouble(arr1[0], -100) == true);

    arr1 = c.calculate("(23-23d); (23-24);");
    CHECK(calc_err == MISSINGPARENTH);
    CHECK(equalDouble(arr1[0], -1) == true);
    
    arr1 = c.calculate("(24+23; ((-(2)-(2)+4)+2)*(50-45);");
    CHECK(calc_err == MISSINGPARENTH);
    CHECK(equalDouble(arr1[0], 10) == true);

    arr1 = c.calculate("24+; (300-400+50)/10;");
    CHECK(calc_err == MISSINGPRIMARY);
    CHECK(equalDouble(arr1[0], -5) == true);
    
    arr1 = c.calculate("(200-340) + (230-; (((-400)+(--400))+20)/5;");
    CHECK(calc_err == MISSINGPRIMARY);
    CHECK(equalDouble(arr1[0], 4) == true);

    arr1 = c.calculate("24+23 23-(40+40);");
    CHECK(calc_err == MISSINGPRINT);
    CHECK(equalDouble(arr1[0], -57) == true);

    arr1 = c.calculate("let x = 10; (300-2)*x; 29-x");
    CHECK(calc_err == MISSINGPRINT);
    CHECK(equalDouble(arr1[0], 2980) == true);

    arr1 = c.calculate("(23/0);");
    CHECK(calc_err == DIVIDEBYZERO);
    CHECK(equalDouble(arr1[0], 0) == true);
    
    arr1 = c.calculate("(400/(320-200-120)); 25-3;");
    CHECK(calc_err == DIVIDEBYZERO);
    CHECK(equalDouble(arr1[0], 22) == true);

    arr1 = c.calculate("(23%0);");
    CHECK(calc_err == MODBYZERO);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("(400%(320-200-120)); 25-3;");
    CHECK(calc_err == MODBYZERO);
    CHECK(equalDouble(arr1[0], 22) == true);

    arr1 = c.calculate("35+25; 35+25; 35+25; 35+25; 35+25; 35+25;");
    CHECK(calc_err == MAXEXPRESSIONS);
    CHECK(arr1.size() == 5);

    // Making sure error var resets to NOERR when new calc call is made.
    arr1 = c.calculate("(23-23);");
    CHECK(calc_err == NOERR);
    CHECK(equalDouble(arr1[0], 0) == true);
}


// int main()
// {
//     try
//     {
//         calculator c1;
//         c1.calculate();
//         return 0;
//     }
//     catch (...)
//     {
//         // other errors (don't try to recover)
//         std::cerr << "exception\n";
//         return 2;
//     }
// }