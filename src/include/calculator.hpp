
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cctype>

// This enum type defines the different causes of runtime_errors
// during the execution of the calculator program
enum class CPError
{
    NOERR,
    PUTBACKFULL,
    BADTOKEN,
    MISSINGPARENTH,
    MISSINGPRIMARY,
    MISSINGPRINT,
    MISSINGEQUAL,
    MISSINGKEYWORD,
    DIVIDEBYZERO,
    MODBYZERO,
    INVALIDVARNAME,
    VAROVERWRITE,
};

// program error value used for determining cause of exception 
extern CPError calc_err;

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

// The "calculator" class simulates a real calculator using the "token_stream"
// class to gather input such as variables, numbers, operators, parenthesis, etc.
class calculator {

private:
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

    // The "token_stream" class represents an input stream and tools
    // used to read this stream via tokens with "token" class. 
    class token_stream
    {
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

    // hidden
    token_stream ts;
    std::vector<token> userVars;
    double primary();
    double term();
    double expression();
    void clean_up_mess(char const&);
    void set_variable();
    bool variable_exists(char const&) const;

public:
    // user interface:
    void erase_user_variables();
    std::vector<double> calculate(std::string const& commands = "");

};
