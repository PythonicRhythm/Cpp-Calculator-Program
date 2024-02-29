
// (COMPLETED)  TODO:   Fix std::cin/string read issues. Currently '\n', '\t' and others lock the program if
//                      they are at the end of the string/cin

// (COMPLETED)  TODO:   Adjust the token_stream class to allow for multiple input sources
//                      such as files and predefined strings to allow for testing.

// (COMPLETED)  TODO:   Negative Numbers

// (COMPLETED)  TODO:   % (Remainer/Modulo)

// TODO:    Pre-defined symbolic values

// (COMPLETED)  TODO:    Variables

#include <calculator.hpp>

CPError calc_err = CPError::NOERR;

// kind() will return the type of token it is.
// returning '8' would be a number token | returning 'q' would be a quit token, etc.
char calculator::token::kind() const
{
    return kind_;
}

// value() will return the numerical value of the token.
// Ex:  token t = token(25);
//      return t.value(); (t.value()) would be 25
double calculator::token::value() const
{
    return value_;
}

// set_value() will redefine the value_ variable in the token
// using the incoming newVal variable. 
void calculator::token::set_value(double newVal)
{
    value_ = newVal;
}

// set_source() allows declaring another stream source after initial construction. 
void calculator::token_stream::set_source(std::string const& str)
{
    source = new std::istringstream (str);
}

// has_more_tokens() allows token_stream to keep reading while the token_stream has not encountered EOF.
bool calculator::token_stream::has_more_tokens()
{
    return source->good();
}

// putback() inserts a token into a single slot buffer.
void calculator::token_stream::putback(token t)
{
    if (full) 
    {
        calc_err = CPError::PUTBACKFULL;
        throw std::runtime_error("putback() into a full buffer");
    }
    buffer = t;
    full = true;
}

// get() reads a token from the istream* source.
calculator::token calculator::token_stream::get()
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
        calc_err = CPError::BADTOKEN;
        throw std::runtime_error("Bad Token");
    }
}

// ignore() discard tokens up to and including a c
void calculator::token_stream::ignore(char c)
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
                calc_err = CPError::MISSINGPARENTH;
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
                calc_err = CPError::BADTOKEN;
                ts.putback(t);
                throw std::runtime_error("Bad Token");
            }
            else{
                // if its not a letter, number, '-', or '(', then throw excep
                calc_err = CPError::MISSINGPRIMARY;
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
                calc_err = CPError::DIVIDEBYZERO;
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
                calc_err = CPError::MODBYZERO;
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
        calc_err = CPError::BADTOKEN;
        throw std::runtime_error("\"let\" expected");
    }
    
    // checks if 't' comes after 'e'
    t = ts.get();
    if(t.kind() != 't')
    {
        calc_err = CPError::BADTOKEN;
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
            calc_err = CPError::BADTOKEN;
            throw std::runtime_error("one letter variable name expected");
        }
        t = next;

        // checks if already exists and doesnt allow for overwrite for right now.
        if(variable_exists(var.kind()))
        {
            calc_err = CPError::BADTOKEN;
            throw std::runtime_error("variable name already exists, no overwriting");
        }

    }
    else {
        // if not alphabetical then must be a mistake
        calc_err = CPError::BADTOKEN;
        throw std::runtime_error("variable name expected");
    }

    // checks if '=' comes after variable name
    if(t.kind() != '=')
    {
        // if not, throw error showing what was there instead
        calc_err = CPError::BADTOKEN;
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
        calc_err = CPError::MISSINGPRINT;
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
bool calculator::variable_exists(char const& c) const
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
    calc_err = CPError::NOERR;      // make sure calc_error is reset

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
                calc_err = CPError::MISSINGPRINT;
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
                    calc_err = CPError::MAXEXPRESSIONS;
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