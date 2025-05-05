#ifndef RECCHECK
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <random>
#include <iomanip>
#include <fstream>
#include <exception>
#endif

#include "boggle.h"

std::vector<std::vector<char> > genBoard(unsigned int n, int seed)
{
    std::mt19937 r(seed);
    int freq[26] = {9,2,2,4,12,2,3,2,9,1,1,4,2,6,8,2,1,6,4,6,4,2,2,1,2,1};
    std::vector<char> letters;
    for(char c='A'; c<='Z';c++)
    {
        for(int i=0; i<freq[c-'A'];i++)
        {
            letters.push_back(c);
        }
    }
    std::vector<std::vector<char> > board(n);
    for(unsigned int i=0;i<n;i++)
    {
        board[i].resize(n);
        for(unsigned int j=0;j<n;j++)
        {
            board[i][j] = letters[(r() % letters.size())];
        }
    }
    return board;
}

void printBoard(const std::vector<std::vector<char> >& board)
{
    unsigned int n = board.size();
    for(unsigned int i=0;i<n;i++)
    {
        for(unsigned int j=0;j<n;j++)
        {
            std::cout << std::setw(2) << board[i][j];
        }
        std::cout << std::endl;
    }
}

std::pair<std::set<std::string>, std::set<std::string> > parseDict(std::string fname)
{
    std::ifstream dictfs(fname.c_str());
    if(dictfs.fail())
    {
        throw std::invalid_argument("unable to open dictionary file");
    } 
    std::set<std::string> dict;
    std::set<std::string> prefix;
    std::string word;
    while(dictfs >> word)
    {
        dict.insert(word);
        for(unsigned int i = 1; i < word.size(); ++i)
        {
            prefix.insert(word.substr(0,i));
        }
    }
    prefix.insert("");
    return make_pair(dict, prefix);
}

std::string boggleHelper(
    const std::set<std::string>& dict,
    const std::set<std::string>& prefix,
    const std::vector<std::vector<char> >& board,
    std::string word,
          std::vector<std::vector<bool>>& visited,
    int r, int c, int dr, int dc)









{
    int n = board.size();
    if (r < 0 || r >= n || c < 0 || c >= n || visited[r][c]) return "";

    word += board[r][c];
  

    visited[r][c] = true;
    std::string longest = "";



    if (dict.find(word) != dict.end())
    {
        longest = word;
    }

    std::string next = boggleHelper(dict, prefix, board, word, visited, r + dr, c + dc, dr, dc);
    if (next.length() > longest.length())
    {
        longest = next;
    }

    visited[r][c] = false;
    return longest;
}

std::set<std::string> boggle(const std::set<std::string>& dict, const std::set<std::string>& prefix, const std::vector<std::vector<char> >& board)
{
    std::set<std::string> result;
    unsigned int n = board.size();
    std::vector<std::vector<bool>> visited(n, std::vector<bool>(n, false));

    for(unsigned int i = 0; i < n; ++i)
    {
        for(unsigned int j = 0; j < n; ++j)
        {
            std::string word;













            word = boggleHelper(dict, prefix, board, "", visited, i, j, 0, 1); // right
            if(!word.empty()) result.insert(word);

            word = boggleHelper(dict, prefix, board, "", visited, i, j, 1, 0); // down
            if(!word.empty()) result.insert(word);

            word = boggleHelper(dict, prefix, board, "", visited, i, j, 1, 1); // diagonal down-right
            if(!word.empty()) result.insert(word);
        }
    }

    return result;
}