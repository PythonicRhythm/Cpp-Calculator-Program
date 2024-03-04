
// (COMPLETED)  TODO:   Fix std::cin/string read issues. Currently '\n', '\t' and others lock the program if
//                      they are at the end of the string/cin

// (COMPLETED)  TODO:   Adjust the token_stream class to allow for multiple input sources
//                      such as files and predefined strings to allow for testing.

// (COMPLETED)  TODO:   Negative Numbers

// (COMPLETED)  TODO:   % (Remainer/Modulo)

// TODO:    Before program gets more complex, switch to istringstream
//          usage. Grab from std::cin using getline and convert to
//          istringstream. Remove istream.

// TODO:    Pre-defined symbolic values
//          Requires major restructure. Need to accept multi letter
//          variable names. An example of symbolic values would be
//          pi. Important constants such as these should already be
//          defined in the calculator before runtime.

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
    if(!(source->good())) return token(nullTerm);   // if at end of file, return nullterm... should lead to end of program.
    
    *source >> ch;

    if(isalpha(ch)) return token(ch);
    if(ch == '@') return token(nullTerm);
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
        //std::cout << (int)ch << std::endl;
        throw std::runtime_error("Bad Token");
    }
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

// ignore() discard tokens up to and including a c
void calculator::token_stream::ignore(char c)
{
    // first look in buffer:
    if (full && buffer.kind() == c) // if buffer contains 'c', delete it and return.
    {
        // checks if the expression that failed is the last one.
        // if so, cleans out all whitespace.
        full = false;
        // char ch = source->get();
        // while(isspace(ch)) {
        //     if(ch == newline) return;
        //     ch = source->get();
        // }

        //if(source->eof()) return;
        
        // if there is another expression, putsback token and program should continue onward.
        //source->putback(ch);
        return;
    }
    // else if (full && buffer.kind() == newline)
    // {
    //     full = false;
    //     return;
    // }
    else if (full && buffer.kind() == number) return;
    else if(full && buffer.kind() == quit) return;
    
    full = false;    // discard the contents of buffer

    // now search input src for c and delete until c or EOF is found
    char ch = source->get();
    while(ch != c)
    {
        //if(ch == newline) return;
        if(source->eof()) {
            source->clear();
            putback(token('\0'));
            return;
        }
        ch = source->get();
    }

    return;
}

// set_source() allows declaring another stream source after initial construction. 
void calculator::token_stream::set_source(std::string const& str)
{
    source = new std::istringstream (str);
}

// has_more_tokens() allows token_stream to keep reading while the token_stream has not encountered EOF.
bool calculator::token_stream::has_more_tokens()
{
    return !(source->eof());
}

std::string calculator::variable::name() const 
{
    return varname;
}

double calculator::variable::value() const
{
    return varvalue;
}

void calculator::variable::update_value(double v)
{
    varvalue = v;
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
                std::string name (1, t.kind());
                t = ts.get();
                while(isalpha(t.kind()))
                {
                    name.append(1, t.kind());
                    t = ts.get();
                }
                ts.putback(t);

                int i = -1;
                if(variable_exists(name, &i))
                {
                    return userVars.at(i).value();
                }

                // else its a bad token
                calc_err = CPError::BADTOKEN;
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
        // if not 'e' throw error showing what token was there instead
        calc_err = CPError::MISSINGKEYWORD;
        ts.putback(t);
        std::string errmsg("\"let\" expected but given {l");
        if(iscntrl(t.kind()))
            errmsg += ' ';
        else if(t.kind() == number)
            errmsg += std::to_string(t.value());
        else
            errmsg += t.kind();
        errmsg += "}.";
        throw std::runtime_error(errmsg);
    }
    
    // checks if 't' comes after 'e'
    t = ts.get();
    if(t.kind() != 't')
    {
        // if not 't' throw error showing what token was there instead
        calc_err = CPError::MISSINGKEYWORD;
        ts.putback(t);
        std::string errmsg("\"let\" expected but given {le");
        if(iscntrl(t.kind()))
            errmsg += ' ';
        else if(t.kind() == number)
            errmsg += std::to_string(t.value());
        else
            errmsg += t.kind();
        errmsg += "}.";
        throw std::runtime_error(errmsg);
    }

    // checks if variable name comes after 't'
    // TODO: add check for variable name that conflicts
    // with key characters.. like 'q'
    t = ts.get();
    std::string name(1, t.kind());
    if(isalpha(t.kind()))
    {

        t = ts.get();
        while(isalpha(t.kind())) 
        {
            name.append(1, t.kind());
            t = ts.get();
        }

        if(name.size() > 1 && name[0] == quit)
            throw std::runtime_error("Cannot create a variable with only 'q' character.");

        // checks if variable name given is longer than one letter.
        // token next = ts.get();
        // if(isalpha(next.kind()))
        // {
        //     // if longer than one letter throw err and show what was given
        //     calc_err = CPError::INVALIDVARNAME;
        //     ts.putback(next);
        //     std::string errmsg("One letter var name expected but given {");
        //     errmsg += var.kind();
        //     errmsg += next.kind();
        //     errmsg += "...}.";
        //     throw std::runtime_error(errmsg);
        // }
        // t = next;

        // checks if variable with that name already exists
        // if(variable_exists(var.kind()))
        // {
        //     // if var exists, throw err, no overwrites for right now
        //     calc_err = CPError::VAROVERWRITE;
        //     //ts.putback(var);
        //     std::string errmsg("Variable name {");
        //     errmsg += var.kind();
        //     errmsg += "} already exists, no overwriting.";
        //     throw std::runtime_error(errmsg);
        // }

    }
    else {
        // if expected var name is not alphabetical
        // must be a mistake, throw err and show what was given. 
        calc_err = CPError::INVALIDVARNAME;
        std::string errmsg("Variable name expected but was given {");
        if(iscntrl(t.kind())) {
            ts.putback(t);
            errmsg += ' ';
        }
        else if(t.kind() == number)
            errmsg += std::to_string(t.value());
        else
            errmsg += t.kind();
        errmsg += "}.";
        throw std::runtime_error(errmsg);
    }

    // checks if '=' comes after variable name
    if(t.kind() != '=')
    {
        // if not, throw error showing what was there instead
        calc_err = CPError::MISSINGEQUAL;
        std::string errmsg("'=' expected but given {");
        if(iscntrl(t.kind())) {
            ts.putback(t);
            errmsg += ' ';
        }
        else if(t.kind() == number)
            errmsg += std::to_string(t.value());
        else 
            errmsg += t.kind();
        errmsg += "}.";
        throw std::runtime_error(errmsg);
    }

    // gathers an expression which will equal variable value.
    double varValue = expression();
    
    // checks if ';' character comes after value
    t = ts.get();
    if(t.kind() != print)
    {
        // if not, throw error showing what was there instead
        calc_err = CPError::MISSINGPRINT;
        std::string errmsg("';' expected but was given {");
        if(iscntrl(t.kind())) {
            ts.putback(t);
            errmsg += ' ';
        }
        else if(t.kind() == number)
            errmsg += std::to_string(t.value());
        else
            errmsg += t.kind();
        errmsg += "}.";
        throw std::runtime_error(errmsg);
    }

    int i = -1;
    if(variable_exists(name, &i))
    {
        userVars.at(i).update_value(varValue);
        std::cout << "Variable Overwritten!" << std::endl;
    }
    else 
    {
        variable var (name, varValue);
        userVars.push_back(var);
        std::cout << "Variable Created!" << std::endl;
    }

    // if '\n' is all that is left, remove it
    // else pushback()
    // t = ts.get();
    // if(t.kind() != '\0') ts.putback(t);
}

// variable_exists() checks if a variable exists
// with the name (string) given and if so, itll

bool calculator::variable_exists(std::string& name, int* index) const
{
    for(int i = 0; i < userVars.size(); i++)
    {
        if(userVars.at(i).name().compare(name) == 0)
        {
            *index = i;
            return true;
        }
    }
    
    *index = -1;
    return false;
}

// erase_user_variables() deletes all variables saved
// in the variable vector when called.
void calculator::erase_user_variables()
{
    userVars.clear();
}

// calculate() begins the simulated calculator program and
// also returns a vector<double> containing the answers to 
// the expressions given. The return vector's elements will
// be tested to confirm correctness in tests/test_filex.cpp.
// If you want to use calculate() via terminal, you must call
// calculate() with no arguments. Calling calculate() with an
// argument such as calculate("3+5;") will find the result for
// the expressions in this string and immediately close. It
// should not ask for more input, if so there is something wrong.
// NOTE: 
//      1.  When testing, keep in mind if no values are given or an exception
//          was thrown for every expression, allVals will remain at size 0.
//      2.  if an expression throws an exception, it will not be assigned
//          a spot in the allVals vector. Program will move to next expression.

std::vector<double> calculator::calculate(std::string const& commands)
{
    std::vector<double> allVals;    // returns the calculated results as a vector.
    calc_err = CPError::NOERR;      // make sure calc_error is reset
    bool str_input = false;         // keeps track if the string being worked on is given via parameter input.
    erase_user_variables();         // clears all user variables saved in the calculator.

    // print prompt
    std::cout << prompt;

    // if a string was given ... set as source stream
    if(!commands.empty())
    {
        str_input = true;
        ts = token_stream(commands);
    }
    else {
        std::string line;
        std::getline(std::cin, line);
        ts = token_stream(line);
    }

    // main calculator loop
    while (ts.has_more_tokens())
    {
        try
        {
            // std::cout << prompt;    // print prompt
            // if(!str_input) {
            //     std::string line;
            //     std::getline(std::cin, line);
            //     ts = token_stream(line);
            // }
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
                if(str_input) {
                    std::cout << "Command String read..." << std::endl;
                    return allVals;
                }

                std::cout << prompt;
                std::string line;
                std::getline(std::cin, line);
                ts = token_stream(line);
                continue;

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
            // if (t.kind() == '\n')
            //     continue;
            
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
            
            allVals.push_back(val);
            std::cout << result << val << std::endl;

            // if '\n' is all that is left... remove it ... else putback()
            //t = ts.get();
            //if(t.kind() != '\n') ts.putback(t);
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