 /*******************************************************************
 * File name: fp_overflow_checker.cpp                               *
 *                                                                  *
 * Author: Isabella Liduario Buzelin Godinho                        *
 * Date: 03/28/2025                                                 *
 * Homework Number 2                                                *
 * CS 3339 Spring 2025                                              *
 * *****************************************************************/

/* OUTPUT TEXT COLORS */
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#include <bitset>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

int IEEE_SINGLE_PRECISION = 32;

// A 'union' shares the memory locaion of its members, making it easier to retrieve the bit representation of a floating point value.
union NumberRepresentation {
    float float_value;
    uint32_t bit_value;
};


// Function to display the IEEE-formatted bit representation of a value.
void displayIEEEBitRepresentation(float value) {
    NumberRepresentation num;
    num.float_value = value;

    // Converting the previous bit representation of 'value' to a
    // single-precision IEEE 754 floating point format
    bitset<32> IEEE_bits(num.bit_value);

    // Ensuring proper formatting
    for (int i = (IEEE_SINGLE_PRECISION - 1); i >= 0; i--) {
        if (i == 31) cout << MAGENTA << IEEE_bits[i] << RESET;
        if (i == 30 || i == 22) cout << " ";
        if (i > 22 && i < 31) cout << GREEN << IEEE_bits[i] << RESET;
        else if (i <= 22) cout << CYAN << IEEE_bits[i] << RESET;
    }

    cout << endl;
}


// Function to check if adding both values cause an overflow and find the minimum threshold for the overflow to occur.
void overflowAndThresholdDetector(float loop_bound, float loop_counter) {
    // Since we can't assign loop_bound to loop_bound_bits directly, we have to extract the raw bits of each number.
    NumberRepresentation loop_bound_bits, loop_counter_bits;
    loop_bound_bits.float_value = loop_bound;
    loop_counter_bits.float_value = loop_counter;
    
    // Extracting the exponent loop_bound and loop_counter
    // To get the exponents, we shift its bits to the least significant bits position
    // and then zero out the rest by masking it using a bitwise AND operator that only keeps the last 8 bits
    unsigned int bound_exponent = (loop_bound_bits.bit_value >> 23) & 0xFF;
    unsigned int counter_exponent = (loop_counter_bits.bit_value >> 23) & 0xFF;

    // Converting to signed int for proper comparison (it had a bug when the loop_bound had a smaller exponent than loop_counter)
    int exponent_diff = static_cast<int>(bound_exponent) - static_cast<int>(counter_exponent);
    
    // The literature suggests that a difference of 24 in the order of magnitude might cause an overflow
    // In this case, if loop_counter is much smaller than loop_bound
    if (exponent_diff >= 24) {
        // Calculate the threshold value with which overflow would happen (the exponent which the difference from the loop counter is 24)
        // Thus, the threshold's exponent is the counter's exponent + 24
        // Additionally, we need to shift the counter's exponent bits back to the original position
        NumberRepresentation threshold;
        threshold.bit_value = (counter_exponent + 24) << 23;        

        cout << YELLOW << "Warning: Possible overflow!\n" << RESET << "Overflow threshold:\n\t";
        cout << threshold.float_value << "\n\t";
        displayIEEEBitRepresentation(threshold.float_value);
    } else
        cout << "There is no overflow!\n";
}


int main(int argc, char *argv[]) {
    // Checking for the appropriate number of command-line arguments
    if(argc != 3) {
        cerr << "Usage:\n\t./fp_overflow_checker loop_bound loop_counter\n\n";
        cerr << "\tloop_bound is a positive floating-point value\n";
        cerr << "\tloop_counter is a positive floating-point value\n";
        return -1;
    }

    // Getting both floating-point values from the command-line arguments and parsing the string as a floating-point number
    float loop_bound = stof(argv[1]),
          loop_counter = stof(argv[2]);

    // Displaying the IEEE-formatted bit representation of both values
    cout << "Loop bound:   ";
    displayIEEEBitRepresentation(loop_bound);

    cout << "Loop counter: ";
    displayIEEEBitRepresentation(loop_counter);
    
    overflowAndThresholdDetector(loop_bound, loop_counter);

    return 0;
}
