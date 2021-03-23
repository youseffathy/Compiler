#include <iostream>
#include <bits/stdc++.h>
#include <sstream>
#include <string>
#include <fstream>
#include <regex>
#include <stack>
#include "phase1.h"
#include "phase2_parser.h"
#include "phase2_leftrec_fact.h"
#include "phase2.h"
#define MAX 1000

using namespace std;

struct nonTerminal_parser;
vector <nonTerminal_parser> grammar;
/*
    elimination of left recursion algorithm

    assumptions:
      1) for any immediate recursion, it must be in the for of A -> Aa1 |.... | Aan | B1 | ... | Bn
         where B1 to Bn exist (cannot be in form A -> Aa1 |.... | Aan)
         otherwise, there will be a cycle in grammar (leads to infinite loop)
*/

void eliminatLeftRecusrion()
{

    grammar = readfile_syn_rules();
    //loop on terminals
    for(int i = 0; i < grammar.size(); i++)
    {
        //loop on productions of terminal i
        for(int k = 0; k < grammar.at(i).productions.size(); k++)
        {
            //loop on terminals preceding terminal i
            for(int j = 0; j < i; j++)
            {
                //check if Ai-> AjB where B is any sequence of grammar symbols and B must not be epsilon as there will be a cycle
                if(grammar.at(i).productions.at(k).at(0)==grammar.at(j).name)
                {
                    vector<string> iProducion = grammar.at(i).productions.at(k);
                    //erasing production (AjB) from non terminal Ai
                    grammar.at(i).productions.erase(grammar.at(i).productions.begin() + k);

                    //erase Aj from AjB to be B only
                    iProducion.erase(iProducion.begin());
                    //loop on every production in non terminal j and add each production ended with B to non terminal i
                    for(int v = 0; v < grammar.at(j).productions.size(); v++)
                    {
                        vector<string> newProducion = iProducion;
                        newProducion.insert(newProducion.begin(), grammar.at(j).productions.at(v).begin(), grammar.at(j).productions.at(v).end());
                        grammar.at(i).productions.insert(grammar.at(i).productions.begin() + k + v, newProducion);
                    }
                }
            }
        }

        //eliminating immediate left recursion
        //create new nonTerminal name as A--> A'
        string newNonterm = grammar.at(i).name + "\'";
        vector<vector<string> > recursiveProducions;
        for(int k = 0; k < grammar.at(i).productions.size(); k++)
        {
            if(grammar.at(i).productions.at(k).at(0) == grammar.at(i).name)
            {

                grammar.at(i).productions.at(k).erase(grammar.at(i).productions.at(k).begin());
                grammar.at(i).productions.at(k).push_back(newNonterm);
                recursiveProducions.push_back(grammar.at(i).productions.at(k));
                grammar.at(i).productions.erase(grammar.at(i).productions.begin() + k);
                k--;
            }
        }

        if(!recursiveProducions.empty())
        {
            // editing non recursive production in nonterminal i
            for(int k = 0; k < grammar.at(i).productions.size(); k++)
            {
                grammar.at(i).productions.at(k).push_back(newNonterm);
            }

            // create the new non terminal and add it to the grammar
            struct nonTerminal_parser nt;
            nt.name = newNonterm;
            //add epsilon to new non terminal productions
            vector<string> epsilon;
            epsilon.push_back(EMPTYSTR);
            recursiveProducions.push_back(epsilon);
            nt.productions = recursiveProducions;
            grammar.insert(grammar.begin() + i + 1, nt);
        }
    }


}


// left factoring algorithm
void leftFactoring()
{
    //looping on each non terminal
    for(int i = 0; i < grammar.size(); i++)
    {
        //count for non terminals that will be extracted from non terminal i
        int newCount = 0;
        //loop on each production and eliminate all other productions with similer first symbol
        for(int j = 0; j < grammar.at(i).productions.size(); j++)
        {
            //fisrt symbol
            string first = grammar.at(i).productions.at(j).at(0);
            //container of all matched productions with the same first symbol "first"
            vector<vector<string> >factoringProds;
            for(int k = j + 1; k <grammar.at(i).productions.size(); k++)
            {
                if(grammar.at(i).productions.at(k).at(0) == first)
                {
                    factoringProds.push_back(grammar.at(i).productions.at(k));
                    grammar.at(i).productions.erase(grammar.at(i).productions.begin() + k);
                    k--;
                }
            }

            //check whether there exists some productions with the same prifix first at least
            if(!factoringProds.empty())
            {
                newCount++;
                //erasing first production of matched productions in non terminal i
                vector<string> prodJ = grammar.at(i).productions.at(j);
                factoringProds.insert(factoringProds.begin(), prodJ);
                grammar.at(i).productions.erase(grammar.at(i).productions.begin() + j);
                j--;

                //find the longest prefix between matched productions
                int longestCount = MAX;
                for(int v = 0; v < factoringProds.size() - 1; v++)
                {
                    vector<string> prod1 = factoringProds.at(v);
                    vector<string> prod2 = factoringProds.at(v + 1);
                    int p = 0;
                    while(p < prod1.size() && p < prod2.size() &&  p < longestCount && prod1.at(p) == prod2.at(p))
                        p++;
                    longestCount = p;
                }

                //add new production in non terminal i with logest prefix appended with new non terminal that will be created
                vector<string> prefProd;
                for(int z = 0; z < longestCount; z++)
                {
                    prefProd.push_back(prodJ.at(z));
                }
                prefProd.push_back(grammar.at(i).name + to_string(newCount));
                grammar.at(i).productions.insert(grammar.at(i).productions.begin() + j + 1, prefProd);

                //creating new non terminal for matched productions
                struct nonTerminal_parser newNonterm;
                newNonterm.name = grammar.at(i).name  + to_string(newCount);
                for(int m = 0; m < factoringProds.size(); m++)
                {
                    factoringProds.at(m).erase(factoringProds.at(m).begin(), factoringProds.at(m).begin() + longestCount);

                    if(factoringProds.at(m).empty())
                    {
                        factoringProds.at(m).push_back(EMPTYSTR);
                    }
                }
                newNonterm.productions = factoringProds;
                //adding the new non terminal to our grammar
                grammar.push_back(newNonterm);
            }
        }

    }
}


// remove left recursion and apply left factoring to the grammar
vector<nonTerminal_parser> final_grammar()
{
    eliminatLeftRecusrion();
    leftFactoring();
    return grammar;
}






