
#include <calculator.hpp>
#include "catch.hpp"

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

    // fake test that should fail just to see std::cout output on test run.
    // CHECK(arr7[4] == 0);
    
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
    CHECK(calc_err == CPError::NOERR);

    arr1 = c.calculate("(300-(200+400)*32); (300-230-400)*(40*(30+2));");
    CHECK(equalDouble(arr1[0], -18900) == true);
    CHECK(equalDouble(arr1[1], -422400) == true);
    CHECK(calc_err == CPError::NOERR);

    arr1 = c.calculate("-(x-2); -(300) + 290;");
    CHECK(calc_err == CPError::BADTOKEN);
    CHECK(equalDouble(arr1[0], -10) == true);

    arr1 = c.calculate("(300-2)*x; 29-x; 900-600;");
    CHECK(calc_err == CPError::BADTOKEN);
    CHECK(equalDouble(arr1[0], 300) == true);

    arr1 = c.calculate("(230-300]); 400-200;");
    CHECK(calc_err == CPError::BADTOKEN);
    CHECK(equalDouble(arr1[0], 200) == true);

    arr1 = c.calculate("'300-2'; 200-300;");
    CHECK(calc_err == CPError::BADTOKEN);
    CHECK(equalDouble(arr1[0], -100) == true);

    arr1 = c.calculate("(23-23d); (23-24);");
    CHECK(calc_err == CPError::MISSINGPARENTH);
    CHECK(equalDouble(arr1[0], -1) == true);
    
    arr1 = c.calculate("(24+23; ((-(2)-(2)+4)+2)*(50-45);");
    CHECK(calc_err == CPError::MISSINGPARENTH);
    CHECK(equalDouble(arr1[0], 10) == true);

    arr1 = c.calculate("24+; (300-400+50)/10;");
    CHECK(calc_err == CPError::MISSINGPRIMARY);
    CHECK(equalDouble(arr1[0], -5) == true);
    
    arr1 = c.calculate("(200-340) + (230-; (((-400)+(--400))+20)/5;");
    CHECK(calc_err == CPError::MISSINGPRIMARY);
    CHECK(equalDouble(arr1[0], 4) == true);

    arr1 = c.calculate("24+23 23-(40+40);");
    CHECK(calc_err == CPError::MISSINGPRINT);
    CHECK(equalDouble(arr1[0], -57) == true);

    arr1 = c.calculate("let x = 10; (300-2)*x; 29-x");
    c.erase_user_variables();
    CHECK(calc_err == CPError::MISSINGPRINT);
    CHECK(equalDouble(arr1[0], 2980) == true);

    arr1 = c.calculate("let x = 10");
    CHECK(calc_err == CPError::MISSINGPRINT);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("let x 10;");
    CHECK(calc_err == CPError::MISSINGEQUAL);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("let x = 10; let y -200;");
    c.erase_user_variables();
    CHECK(calc_err == CPError::MISSINGEQUAL);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("l;");
    CHECK(calc_err == CPError::MISSINGKEYWORD);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("l");
    CHECK(calc_err == CPError::MISSINGKEYWORD);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("le");
    CHECK(calc_err == CPError::MISSINGKEYWORD);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("le;");
    CHECK(calc_err == CPError::MISSINGKEYWORD);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("(23/0);");
    CHECK(calc_err == CPError::DIVIDEBYZERO);
    CHECK(equalDouble(arr1[0], 0) == true);
    
    arr1 = c.calculate("(400/(320-200-120)); 25-3;");
    CHECK(calc_err == CPError::DIVIDEBYZERO);
    CHECK(equalDouble(arr1[0], 22) == true);

    arr1 = c.calculate("(23%0);");
    CHECK(calc_err == CPError::MODBYZERO);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("(400%(320-200-120)); 25-3;");
    CHECK(calc_err == CPError::MODBYZERO);
    CHECK(equalDouble(arr1[0], 22) == true);

    arr1 = c.calculate("let xy = 10;");
    CHECK(calc_err == CPError::INVALIDVARNAME);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("let xyz = ;");
    CHECK(calc_err == CPError::INVALIDVARNAME);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("let 25 = 200;");
    CHECK(calc_err == CPError::INVALIDVARNAME);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("let mx = 200;");
    CHECK(calc_err == CPError::INVALIDVARNAME);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("35+25; 35+25; 35+25; 35+25; 35+25; 35+25;");
    CHECK(calc_err == CPError::MAXEXPRESSIONS);
    CHECK(arr1.size() == 5);

    arr1 = c.calculate("let x = 200; let x = 25;");
    c.erase_user_variables();
    CHECK(calc_err == CPError::VAROVERWRITE);
    CHECK(equalDouble(arr1[0], 0) == true);

    arr1 = c.calculate("let x = 25; let y = 200*x; let z = x*y; let x = z;");
    c.erase_user_variables();
    CHECK(calc_err == CPError::VAROVERWRITE);
    CHECK(equalDouble(arr1[0], 0) == true);

    // Making sure error var resets to NOERR when new calc call is made.
    arr1 = c.calculate("(23-23);");
    CHECK(calc_err == CPError::NOERR);
    CHECK(equalDouble(arr1[0], 0) == true);
}
