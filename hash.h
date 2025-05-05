#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cctype>
typedef std::size_t HASH_INDEX_T;

struct MyStringHash {
    HASH_INDEX_T rValues[5] { 983132572, 1468777056, 552714139, 984953261, 261934300 };
    MyStringHash(bool debug = true)
    {
        if(false == debug){
            generateRValues();
        }
    }
    // hash function entry point (i.e. this is h(k))
    HASH_INDEX_T operator()(const std::string& k) const
    {
        // Add your code here
        std::string lower;
        for(char ch : k)
        {
            if(std::isalnum(ch)) {
                lower += std::tolower(ch);
            }
        }

        const int groupSize = 6;
        const int maxGroups = 5;
        unsigned long long w[maxGroups] = {0};

        int groupIndex = maxGroups - 1;
        int i = lower.length();
        while(i > 0 && groupIndex >= 0)
        {
            int start = std::max(0, i - groupSize);
            unsigned long long value = 0;
            unsigned long long base = 1;
            for(int j = i - 1; j >= start; --j)
            {
                value += letterDigitToNumber(lower[j]) * base;
                base *= 36;
            }
            w[groupIndex] = value;
            i -= groupSize;
            groupIndex--;
        }

        unsigned long long hash = 0;
        for(int i = 0; i < maxGroups; ++i)
        {
            hash += rValues[i] * w[i];
        }
        return hash;
    }

    // A likely helper function is to convert a-z,0-9 to an integral value 0-35
    HASH_INDEX_T letterDigitToNumber(char letter) const
    {
        // Add code here or delete this helper function if you do not want it
        letter = std::tolower(letter);
        if(letter >= 'a' && letter <= 'z')
        {
            return letter - 'a';
        }
        else if(letter >= '0' && letter <= '9')
        {
            return 26 + (letter - '0');
        }
        return 0;
    }

    // Code to generate the random R values
    void generateRValues()
    {
        // obtain a seed from the system clock:
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator (seed);  // mt19937 is a standard random number generator

        // Simply call generator() [it has an operator()] to get another random number
        for(int i{ 0 }; i < 5; ++i)
        {
            rValues[i] = generator();
        }
    }
};

#endif

