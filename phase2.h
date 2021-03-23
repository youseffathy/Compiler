#ifndef PHASE2_H_INCLUDED
#define PHASE2_H_INCLUDED

//The empty string
#define EMPTYSTR "\\L"
//The end marker
#define END_MARKER "$"
//The synch. token
#define SYNCH "synch"

using namespace std;

// phase 3

class stack_node
{
private:
    string name;
    map<string, string> allAttr;
   // map<string, string> inhAttr;

public:
    stack_node(string name)
    {
        this->name = name;
    }

    string get_node_name()
    {
        return this->name;
    }

    string getAttr(string attrName)
    {
        auto it = allAttr.find(attrName);
        if(it == allAttr.end())
        {
            return "";

        }
        return it->second;
    }
    void setAttr(string attrName, string attVal)
    {
        auto it = allAttr.find(attrName);
        if(it == allAttr.end())
        {
            allAttr.insert(make_pair(attrName, attVal));
        }
        else
        {
            allAttr[attrName] = attVal;
        }

    }

};

//Data structures used
//A structure for the productions.
/*
     contains:
     A string vector representing the RHS string of grammar alphabets (terminals and nonterminals)
     A string vector for the first set of this production to be used in filling the parsing table.
*/
struct production
{
    vector<string> RHS;
    vector<string> first;
};

//A structure for the nonterminals
/*
     contains:
     A string for the nonterminal name
     vectors for its first and follow sets
     vector for the productions where this non terminal is the LHS
*/
struct non_terminal
{
    string name;
    vector<string> first;
    vector<string> follow;
    vector<production*> productions;
};



void add_all_nonterminals();
void run_stack();
// phase 3
void run_stack_nodes();
void main_synt_analyzer();


#endif // PHASE2_H_INCLUDED
