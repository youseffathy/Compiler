#include <iostream>
#include <bits/stdc++.h>
#include <sstream>
#include <string>
#include <fstream>
#include <regex>
#include <stack>
#include "phase1.h"
#include "phase2_parser.h"
#include "phase2.h"

using namespace std;

ifstream infile_syntax_rules;
string syntax_rules_filename = "syntax_rules.txt";

/*
    assumptions:
      1) non terminal name contains only uppercase and lowercase letters , numbers ,
         underscore and single quotes
      2) for multi lines non terminal the first line starts with '#' and the next lines
         start with '|'
*/


//struct nonTerminal_parser;
struct nonTerminal_parser;


// function to remove last spaces of string until reaching a character
string remove_lastspaces(string str)
{
    int i= str.length()-1;
    while( i>=0 && (str[i]==' ' || str[i] == '\t' || str[i] == '\n' ) )
    {
        i--;
    }
    return str.substr(0,i+1);
}

// function to remove first and last spaces of string to start and end with character
string remove_all_spaces_out(string str)
{
    str = remove_firstspaces(str);
    str = remove_lastspaces(str);
    return str;
}

// split the input string according according to the input delimiter
vector<string> split_string(string str,char delimiter)
{
   stringstream ss(str);
   // vector to store the splitted strings
   vector<string> splitted_str;
   string item = "";
   while(getline(ss,item,delimiter)){
    item = remove_all_spaces_out(item);
    if(item != ""){
       //check if the string is terminal then remove the single quotes
      if(delimiter == ' ' && item[0] == '\'' && item[item.length()-1] == '\''){
        item = item.substr(1,item.length()-2);
      }
      splitted_str.push_back(item);
    }
   }
   return splitted_str;
}

// parse each line that represents a specific nonterminal
// and store it in struct non_terminal_parser
nonTerminal_parser parse_line_syn(string line)
{
    nonTerminal_parser non_term;
    vector<vector<string>> all_prods;
    // regex for line that represents a nonterminal
    regex reg_synrule("\\s*#\\s*([a-zA-Z0-9_\'&]+)\\s*\\=(.*)");
    smatch matches;
    regex_match(line,matches,reg_synrule);
    // non terminal name
    string g1 = remove_all_spaces_out(matches[1]);
    non_term.name = g1;
    // RHS of non terminal
    string g2 = remove_all_spaces_out(matches[2]);
    // split the all productions of nonterminal on '|' char
    vector<string> prods = split_string(g2,'|');
    for(int i=0;i<prods.size();i++){
        string prod = prods[i];
        // split each production the space
        vector<string> splitted_and = split_string(prod,' ');
        all_prods.push_back(splitted_and);
    }
    non_term.productions = all_prods;
    return non_term;

}

// read the rules from syntax file and return all non terminals lines
vector<nonTerminal_parser> readfile_syn_rules()
{
    vector<nonTerminal_parser> all_nonterminals;
    string line = "";
    string current_line = "";
    infile_syntax_rules.open(syntax_rules_filename);
    while(getline(infile_syntax_rules,line))
    {
        // get line from the file after removing the first spaces
        line = remove_firstspaces(line);
        if(line[0] == '#'){
            //add new non_terminal
            if(current_line!=""){
              all_nonterminals.push_back(parse_line_syn(current_line));
            }
            current_line = "";
        }
        // handling multi lines non terminal
        current_line += line;
    }
    if(current_line!=""){
       all_nonterminals.push_back(parse_line_syn(current_line));
    }
    infile_syntax_rules.close();
    return all_nonterminals;
}



