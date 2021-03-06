#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <fstream>
#include <regex>
#include <stack>
#include "phase1.h"

//A char indicating no input, can be updated to other value
#define NOINPUT '\0'
using namespace std;

/*
Assumptions:
1- The input alphabet is of type character.
2- States are represented using numbers from 0 to n where n is number of states - 1
3- For regular definitions, we need a vector of values for its values.
*/

ifstream infile;
ofstream outfile;
///////////////// global variables for initial parsing/////////////
string input_filename_rules = "rules.txt";


///////////////// global variables for final parsing/////////////
string output_filename = "output_lexical.txt";
string input_filename = "input.txt";


//define the identifier token
string identifier;


///////////////  Data structures for final parser////////////////
map<string,pair<int,string>> symbol_table;
vector<string> all_tokens;
vector<string> all_tokens_values;
int location_in_local_array = 1;

///////////filled containers by initial parser//////////
vector<string> input;
vector <char> punctuations;
vector <char> reservedSymbols;
vector<regularDefinition> definitions;
vector<regularExpression> expressions;
vector<keyWord> keyWords;

///////////////////////////// Data structures for NFA////////////////
//A class representing our grammar:
class Grammar
{
    //The data structures required to be filled by parsing the file:4
private:
    //Contains keywords as if:
    vector<string> keywords;
    //Contains the punctuation characters as ;:
    vector<char> punc_sym;
    //Contains the reserved characters:
    vector<char> reserved;
    //A vector containing the regular definitions as letter etc:
    vector<Regular_Definition> reg_def;
    //A vector containing the NFAs representing the regular expression productions in this grammar
    vector<NFA> productions;

public:
    //constructor
    Grammar()
    {
        //To do if needed..
    }
    //setters and getters:
    void set_keywords(vector<string> keywords)
    {
        this ->keywords = keywords;
    }
    vector<string> get_keywords()
    {
        return this->keywords;
    }
    void set_punc_sym(vector<char> punc_sym)
    {
        this ->punc_sym = punc_sym;
    }
    vector<char> get_punc_sym()
    {
        return this->punc_sym;
    }
    void set_reserved(vector<char> reserved)
    {
        this ->reserved = reserved;
    }
    vector<char> get_reserved()
    {
        return this->reserved;
    }
    void set_reg_def(vector<Regular_Definition> reg_def)
    {
        this ->reg_def = reg_def;
    }
    vector<Regular_Definition> get_reg_def()
    {
        return this->reg_def;
    }
    void set_productions(vector<NFA> productions)
    {
        this ->productions = productions;
    }
    vector<NFA> get_productions()
    {
        return this->productions;
    }

    //Utility functions, to add an element to a DS etc..:
    void add_keyword(string keyword)
    {
        keywords.push_back(keyword);
    }
    void add_punc_sym(char symbol)
    {
        punc_sym.push_back(symbol);
    }
    void add_reserved(char symbol)
    {
        reserved.push_back(symbol);
    }
    void add_reg_def(Regular_Definition def)
    {
        reg_def.push_back(def);
    }
    void add_reg_def(vector<char> values,string name)
    {
        Regular_Definition def;
        def.values = values;
        def.name = name;
        reg_def.push_back(def);
    }
    void add_production(NFA production)
    {
        this -> productions.push_back(production);
    }
    //Grammar based functions
    //returns true if given character is a reserved character
    bool isreserved(char symbol)
    {
        return contains(reserved,symbol);
    }
    bool iskeyword(string str)
    {
        return contains(keywords,str);
    }
//Private functions used
private:
    bool contains(vector<char> v, char c)
    {
        for(int i = 0; i < v.size(); i++)
            if(v[i] == c)
                return true;
        return false;
    }
    bool contains(vector<string> v, string str)
    {
        for(int i = 0; i < v.size(); i++)
            if(!(v[i].compare(str)))
                return true;
        return false;
    }

} Grammar;
//A name is placed as it is a singleton



//////////////////////////  initial parser functions  ///////////

map<string,pair<int,string>> get_symbol_table(){
    return symbol_table;
}

void set_symbol_table(map<string,pair<int,string>> temp_map){
    symbol_table = temp_map;
}

// function to remove first spaces of string until reaching a character
string remove_firstspaces(string str)
{
    int i=0;
    while( (str[i]==' ' || str[i] == '\t' || str[i] == '\n' ) &&i<str.length())
    {
        i++;
    }
    return str.substr(i,str.length()-i);
}


// get string between brackets and parse it again
vector<string> get_betweenbrackets(string expression)
{
    // regex for getting operation and second operand
    regex reg_operation_operand("\\s*([\\s\\|])(.*)");

    smatch matches;

    // positions of first opening bracket and its closing bracket
    int positions [2] = {0,0};
    vector <string> str_brackets ;
    stack <char> s;
    // start position of opening bracket
    int j=0;
    while(expression.size() >0   && expression[j] == ' ' )
    {
        expression.erase(0,1);
    }
    positions[0] = j;
    if(j<expression.size())
    {
        s.push(expression[j]);
    }
    // push and pop from stack until reaching the closing bracket
    for(j=j+1; j<expression.length(); j++)
    {
        if(expression[j]== '(' )
        {
            s.push(expression[j]);
        }
        if(expression[j] == ')')
        {
            s.pop();
            positions [1] = j;
            if(s.empty())
            {
                break;
            }
        }
    }
    // check if the operand has * or + operations sand concatenate it
    // to operand if exists
    int start_index = positions[1];
    string expr_with_brackets = expression.substr(positions[0],positions[1]-positions[0]+1);
    if (positions[1]+1 < expression.length() &&
            (expression[positions[1]+1] == '*' || expression[positions[1]+1] == '+'))
    {
        expr_with_brackets += expression[positions[1]+1];
        start_index += 1;
    }
    // push first operand to returned vector
    str_brackets.push_back(expr_with_brackets);
    if(start_index < expression.length()-1)
    {
        start_index +=1;

        string afterbrackets = expression.substr(start_index,expression.length()-start_index);
        if(regex_match(afterbrackets,matches,reg_operation_operand))
        {
            // check if operation and second operand exists then
            // push them to returned vector
            string g1 = matches[1];
            string g2 =remove_firstspaces(matches[2]);
            if(g2.length()!=0)
            {
                str_brackets.push_back(g1);
                str_brackets.push_back(g2);
            }
        }
    }
    return str_brackets;
}

// method to get postfix of input and get required informations
vector<Node *> getNodes(string line)
{
    //save operations to achieve precedence
    stack<Node *> stck;
    // create a node # to prevent underflow
    Node *dash = new Node;
    dash->setName("hashTag");
    stck.push(dash);
    // postfix nodes of line
    vector <Node *> postfix ;

    regex reg("\\s*((\\([^\\)]*\\)[\\*\\+]?)|([a-z]\\s*\\-\\s*[a-z])|([A-Z]\\s*\\-\\s*[A-Z])|([0-9]\\s*\\-\\s*[0-9])|([^\\s\\|]*))(\\s*\\|?)(.*)");
    smatch matches;
    regex_match(line, matches, reg);

    // regex to match
    // group 1 -> parent group with children of any operands

    //children of parent group
    // group 2 -> brackets
    // group 3, 4, 5 -> ranges
    // group 6 -> any other operands
    // group 7 -> operation (if any)
    // group 8 -> second operand (if any)
    string g1, g2, g3, g4, g5, g6, g7, g8;
    while(line.size() != 0)
    {
        regex_match(line, matches, reg);
        g1 = matches[1];
        g2 = matches[2];
        g3 = matches[3];
        g4 = matches[4];
        g5 = matches[5];
        g6 = matches[6];
        g7 = matches[7];
        g8 = matches[8];

        Node *node = new Node;
        //used to add unary operator node
        bool hasUnaryOp = false;
        char unaryOp;

        //case of brackets
        if(g1 == g2 && g1.size() > 0)
        {
            // get the first operand bracket
            vector<string> lineWithBracket = get_betweenbrackets(line);

            if(lineWithBracket.size() >= 1) // for assurance only
            {
                if(lineWithBracket.size() > 1) // for two operand and an operation
                {
                    g7 = lineWithBracket.at(1);
                    g8 = lineWithBracket.at(2);
                }
                else
                {
                    g7 = g8 = "";
                }
                g1 = lineWithBracket.at(0);

                //checking for unary operations for the whole bracket
                if(g1[g1.size() - 1] == '*' || g1[g1.size() - 1] == '+' )
                {
                    hasUnaryOp = true;
                    unaryOp = g1[g1.size() - 1];
                    //removing unary operation
                    g1 = g1.erase(g1.size()- 1,1);
                }
                // removing brackets
                g1 = g1.erase(0, 1);
                g1 = g1.erase(g1.size() - 1, 1);

                // recursive call for what is inside the bracket
                vector<Node *> postFixOfBracket = getNodes(g1);

                //adding to postFix
                for(int i = 0; i < postFixOfBracket.size(); i++)
                {
                    postfix.push_back(postFixOfBracket.at(i));
                }
            }
            else
            {
                //error
            }

        }
        // case of ranges
        else if((g1 == g3 || g1 == g4 || g1 == g5) && g1.size() > 0)
        {
            string range = "";
            //ORing all range values
            for(char i = g1[0]; i <= g1[g1.size() - 1]; i++)
            {
                range += " ";
                range += i;
                if(i != g1[g1.size() - 1])
                {
                    range += " ";
                    range += "|";
                }
            }

            // recursive call to get the postfix of the range
            vector<Node *> ranges = getNodes(range);

            //adding to postFix
            for(int i = 0; i < ranges.size(); i++)
            {
                postfix.push_back(ranges.at(i));
            }

        }
        //case of any other operands
        else if(g1 == g6 && g1.size() > 0)
        {
            //to prevent taking + * in (addop: \+ | -) or (mulop: \* | /) as a unary operation on operands
            // or to take an operand like * + as unary operations
            if(g1 != "\\+" && g1 != "\\*" && g1 != "+" && g1 != "*")
            {
                //checking for unary operations
                if(g1[g1.size() - 1] == '*' || g1[g1.size() - 1] == '+' )
                {
                    hasUnaryOp = true;
                    unaryOp = g1[g1.size() - 1];
                    //removing unary operation
                    g1 = g1.erase(g1.size()- 1,1);
                }
            }

            // to keep backslashes only in case of \ alone or \L
            if(g1.compare("\\") != 0 && g1.compare("\\L") != 0)
            {
                // erasing all backslashes if any and saving reserved symbol;
                for(int i = 0; i < g1.size(); i++ )
                {
                    if(g1[i] == '\\')
                    {
                        g1 = g1.erase(i, 1);
                        bool exist = false;
                        for(char c : reservedSymbols)
                        {
                            if(c == g1[i])
                            {
                                exist = true;
                                break;
                            }
                        }
                        if(!exist)
                            reservedSymbols.push_back(g1[i]);
                    }
                }
            }

            bool exist = false;
            vector<Node *> nodes;
            //check for exist definitions
            for(int i = 0; i < definitions.size(); i++)
            {
                if(definitions.at(i).definitionName == g1)
                {
                    exist = true;
                    nodes = definitions.at(i).postfixNodes;
                    break;

                }
            }
            for(int i = 0; i < expressions.size(); i++)
            {
                if(expressions.at(i).expressionName == g1)
                {
                    exist = true;
                    nodes = expressions.at(i).postfixNodes;
                    break;
                }
            }
            if(exist) // is preidentified definition or expression
            {

                for(int i = 0; i < nodes.size(); i++)
                {
                    postfix.push_back(nodes.at(i));
                }
            }
            else // new def. or exp.
            {
                if(g1.size() > 1) // as == ,  != or any other operand consists of more than one character
                {
                    string combinedChars = "";
                    //ANDing the character of operand
                    for(int i = 0; i < g1.size(); i++)
                    {
                        combinedChars += g1[i];
                        if(i != g1.size() - 1)
                        {
                            combinedChars += " ";
                        }
                    }

                    //recursive call to get the postfix of operand characters
                    vector<Node *> combinedNodes = getNodes(combinedChars);
                    //adding to postFix
                    for(int i = 0; i < combinedNodes.size(); i++)
                    {
                        postfix.push_back(combinedNodes.at(i));
                    }
                }
                // base case in which operand is one character
                // no need for condition but to confirm
                else if(g1.size() == 1)
                {
                    node->setName(g1);
                    postfix.push_back(node);
                }
            }

        }

        /* operation and second operand checking */

        //checking for unary operations
        if(hasUnaryOp)
        {
            Node *opNode = new Node;
            string op(1, unaryOp);
            opNode->setName(op);
            opNode->setPrecedence(3);// not needed as it will not be pushed to stack. Hence, it can be deleted
            opNode->setHasunaryOperation(true);
            postfix.push_back(opNode);
        }

        //check for binary (AND OR) operations
        if(g7.size() != 0 && g8.size() != 0)
        {
            string binaryOperation;
            Node *binaryOp = new Node;
            if(g7[g7.size() - 1] == ' ')
            {
                binaryOperation = "AND";
                binaryOp->setPrecedence(2);
            }
            else
            {
                binaryOperation = "OR";
                binaryOp->setPrecedence(1);
            }

            binaryOp->setName(binaryOperation);

            //conversion of infix to postfix stack manipulations
            if(binaryOp->getPrcendence() > stck.top()->getPrcendence())
            {
                stck.push(binaryOp);
            }
            else
            {
                while(stck.top()->getName() != "hashTag" && binaryOp->getPrcendence() <= stck.top()->getPrcendence())
                {
                    postfix.push_back(stck.top());
                    stck.pop();
                }
                stck.push(binaryOp);
            }
            //prepare the second operand for the next loop
            line = g8;
        }
        else //line has been parsed
        {
            line = "";
        }
    }

    //conversion of infix to postfix stack manipulations
    while(stck.top()->getName() != "hashTag")
    {
        postfix.push_back(stck.top());
        stck.pop();
    }
    return postfix;
}


// read lines of input file
vector<string> read_file(string filename)
{
    infile.open(filename);
    vector<string> all_data;
    string line = "";
    while(getline(infile,line))
    {
        all_data.push_back(line);
    }
    infile.close();
    return all_data;
}


// main function to parse input rules and fill the data structures needed
void parseInput()
{

    // read from file
    input = read_file(input_filename_rules);


    // regex to match
    // group 1 -> (anyID (:=))
    // group 2 -> anyID
    // group 3 -> : or =
    // group 4 -> keywords
    // group 5 -> punctuations
    regex reg("\\s*(([A-Za-z]+)\\s*([=:]))?\\s*(\\{[^\\}]*\\})?(\\[[^\\]]*\\])?((\\([^\\)]*\\)[\\*\\+]?)|([a-z]\\s*\\-\\s*[a-z])|([A-Z]\\s*\\-\\s*[A-Z])|([0-9]\\s*\\-\\s*[0-9])|([^\\s\\{\\[\\|\\(\\)]*))(\\s*\\|?)(.*)");
    smatch matches;
    string line;

    //parse input lines
    for(int i = 0; i < input.size(); i++)
    {
        line = input.at(i);
        regex_match(input.at(i), matches, reg);
        string g1 = matches [1];
        string g2 = matches [2];
        string g3 = matches [3];
        string g4 = matches [4];
        string g5 = matches [5];
        //case 1 : keywords
        //termination group
        if(g4.size() != 0)
        {
            // erase {} at the two ends
            g4 =  g4.erase(0,1);
            g4 =  g4.erase(g4.size() -1,1);

            //loop to get each key word alone in vector result
            std::vector<std::string> result;
            std::istringstream iss(g4);
            for(std::string g4; iss >> g4;)
                result.push_back(g4);

            //creating postFix for each keyword
            for(int i = 0; i < result.size(); i++)
            {
                struct keyWord word;
                word.keyWordName = result.at(i);
                word.postfixNodes = getNodes(result.at(i));
                keyWords.push_back(word);
            }
        }
        //case 2 : punctuations
        //termination group
        else if(g5.size() != 0)
        {
            //erasing  [ ] at the two ends
            g5 =  g5.erase(0,1);
            g5 =  g5.erase(g5.size() -1,1);
            for(int i = 0; i < g5.size(); i++)
            {
                if(g5[i] != ' ')
                {
                    //checking for reserved symbols
                    if(g5[i] == '\\')
                    {
                        g5.erase(i, 1);
                        while(g5[i] == ' ')
                        {
                            i++;
                        }
                        bool exist = false;
                        //check if symbol is already reserved
                        for(char c : reservedSymbols)
                        {
                            if(c == g5[i])
                            {
                                exist = true;
                                break;
                            }
                        }
                        if(!exist)
                            reservedSymbols.push_back(g5[i]);
                    }
                    punctuations.push_back(g5[i]);
                }
            }
        }
        // case of regular definition or expression
        else if(g1.size() != 0)
        {
            if(matches[3] == "=") // case of regular definition
            {
                struct regularDefinition def;
                def.definitionName = g2;
                // trim Name and  = tokens from input line
                line = line.replace(line.begin(), line.begin() + line.find("=") + 1, "");

                vector<Node *>nodes = getNodes(line);
                def.postfixNodes = nodes;
                definitions.push_back(def);

            }
            else // case of regular expression
            {
                struct regularExpression exp;
                exp.expressionName = g2;
                // trim Name and  : tokens from input line
                line = line.replace(line.begin(), line.begin() + line.find(":") + 1, "");
                vector<Node *>nodes = getNodes(line);
                exp.postfixNodes = nodes;
                expressions.push_back(exp);
            }
        }
    }
}




//////////////////////   NFA functions   /////////////////////

string get_states(vector<node *> states)
{
    string s = "";
    for(int i = 0; i < states.size(); i++)
    {
        stringstream ss;
        ss << states[i]->node_number;
        string str = ss.str();
        s+=str;
        s+=" ";
    }
    return s;
}

NFA combine(vector<NFA> machines)
{
    node * start = new node;
    start->node_number = 1;
    NFA combined(start);
    combined.add_state(start);
    int total_size = 1;
    for(int i = 0; i < machines.size(); i++)
    {
        combined.push_new_states(machines[i]);
        machines[i].inc_nodes(total_size);
        combined.add_translation(combined.get_start(),machines[i].get_start(),NOINPUT);
        total_size += machines[i].get_num_states();
    }
    combined.set_num_states(total_size);
    return combined;
}


//Functions to create the NFA from data read from file
//A function returns the or operation of 2 NFAs
NFA or_op(NFA machine1,NFA machine2)
{
    vector<NFA> machines;
    machines.push_back(machine1);
    machines.push_back(machine2);
    return(combine(machines));
}





//A function returns the concatenation of two machines
//ASSUME: the left parameter is the back machine and
//the right parameter is the front machine
NFA conc_op(NFA back_mc,NFA front_mc)
{
    NFA result(back_mc.get_start());
    vector<node *> final_states = back_mc.get_final_states();
    node * combiner = front_mc.get_start();
    //Join final states of back machine into a single node
    for(int i =0; i<final_states.size(); i++)
    {
        final_states[i]->transition[NOINPUT].push_back(combiner);
    }
    front_mc.inc_nodes(back_mc.get_num_states());
    //push all states of back machine as non final states of results
    vector<node *> new_states = back_mc.get_states();
    for(int i = 0; i<new_states.size(); i++)
    {
        result.add_state(new_states[i]);
        //set final states as non-final
        if(is_final(new_states[i]))
            new_states[i]->token = "";
    }
    //Add the states of the front machine
    result.push_new_states(front_mc);
    result.set_num_states(front_mc.get_num_states() + back_mc.get_num_states());
    return result;
}
//creates the original NFA given a single input character and a token corresponding to a leaf node
//The machine contains two nodes with the input joining them
NFA create_machine(char input,string token)
{
    node * start = new node;
    start->node_number = 1;
    node * final_node = new node;
    final_node->node_number = 2;
    final_node->token = token;
    start->transition[input].push_back(final_node);
    NFA result(start);
    result.add_state(start);
    result.add_final_state(final_node);
    result.set_num_states(2);
    return result;
}
//A function that takes a machine and sets the token of all it's final states to a certain string
void set_token(NFA machine,string token)
{
    vector <node *> final_states = machine.get_final_states();
    for(int i = 0; i < final_states.size(); i++)
        final_states[i]->token = token;
}
//A function that performs the star operation on a machine
NFA star_op(NFA machine)
{
    node* start = new node;
    start->node_number = 1;
    node* final_node = new node;
    //Add 1 for added start node
    final_node->node_number = 1;
    final_node->token = machine.get_final_states()[0]->token;
    start->transition[NOINPUT].push_back(final_node);
    vector<node*> final_states = machine.get_final_states();
    for(int i =0; i < final_states.size(); i++)
    {
        final_states[i]->transition[NOINPUT].push_back(machine.get_start());
    }
    //perform concatenation to get result
    NFA accept_mc = NFA(final_node);
    accept_mc.add_final_state(final_node);
    accept_mc.set_num_states(1);
    NFA start_mc = NFA(start);
    start_mc.add_final_state(start);
    start_mc.set_num_states(1);
    NFA temp = conc_op(start_mc,machine);
    return conc_op(temp,accept_mc);
}
//Simply performs concatenation of this machine with its star
NFA plus_op(NFA machine)
{

    NFA copied = copy_machine(machine);
    return(conc_op(copied,star_op(machine)));
}
NFA copy_machine(NFA machine)
{
    node* start = new node;
    NFA copied(start);
    //copying start
    node* original_start = machine.get_start();
    start->node_number = original_start->node_number;
    start->token = original_start->token;
    if(!is_final(original_start))
        copied.add_state(start);
    else
        copied.add_final_state(start);
    vector<node *> states = machine.get_states();
    //Copying nodes
    for(int i = 0; i < states.size(); i++)
    {
        if(states[i]->node_number == copied.get_start()->node_number)
            continue;
        node* new_node = new node;
        new_node->node_number = states[i]->node_number;
        new_node->token = states[i]->token;
        if(!is_final(new_node))
            copied.add_state(new_node);
        else
            copied.add_final_state(new_node);
    }
    //copying transitions---difficult part
    vector<node*> copied_states = copied.get_states();
    for(int i = 0; i < copied_states.size(); i++)
    {
        node* current_node = copied_states[i];
        node* copied_from = machine.get_state(current_node->node_number);
        std::map<char, vector<node*> >::iterator it = copied_from->transition.begin();
        while(it!= copied_from->transition.end())
        {
            char input = it->first;
            vector<node *> outputs = it->second;
            for(int j = 0; j < outputs.size(); j++)
            {
                current_node->transition[input].push_back(copied.get_state(outputs[j]->node_number));
            }
            it++;
        }
    }
    copied.set_num_states(machine.get_num_states());
    return copied;
}


/////////link NFA to parser functions  ///////////////////////

bool is_alphabet(char c)
{
    if( (c>='a' && c <='z') || (c>='A' && c <='Z'))
    {
        return true;
    }
    return false;
}

bool is_binary_op(string str)
{
    if(str.compare("OR") == 0 || str.compare("AND") == 0)
    {
        return true;
    }
    return false;
}

// get name of identifier inserted in the rules file
// to print it in symbol table
string get_identifier(vector<regularExpression> reg_exs)
{
    string id_name = "";
    for(int i=0; i<reg_exs.size(); i++)
    {
        vector<Node*> postfix = reg_exs.at(i).postfixNodes;
        if(postfix.size() > 0)
        {
            string char_name = postfix.at(0)->getName();
            if(is_alphabet(char_name[0]))
            {
                id_name = reg_exs.at(i).expressionName;
            }
        }
    }
    return id_name;
}

// set identifier name to fill it in symbol table
void set_identifier_global ()
{
    identifier = get_identifier(expressions);
}


// main functions which form the NFA machine according to nodes
// pf expression tree which is represented in postfix form.
// it depends on the four main machines of basic operation
// then merge them together ( a|b , a b , a* , a+) keeping
// the precedence
/////////////////////// old one /////////////////

NFA get_nfa_expression(string token,vector < Node* > postfix)
{
    // stack to process the postfix expression and build the NFA
    stack <NFA> stack_process;
    for(int i=0; i<postfix.size(); i++)
    {
        string temp = postfix[i]->getName();
        bool is_unary = postfix[i]->iShasUnaryOperation();
        // check if current character is not operation
        // to push it in stack
        if(! is_unary && !is_binary_op(temp))
        {
            NFA nfa_leaves = create_machine(temp[0],token);
            stack_process.push(nfa_leaves);
        }
        // if the current character is operation then pop
        // operands from the stack according to type of operation
        else if(is_unary)
        {
            // it is unary operation then pop the last machine and
            // apply operation to it then push it back.
            NFA nfa_first = stack_process.top();
            stack_process.pop();
            if(temp.compare("*") == 0)
            {
                nfa_first = star_op(nfa_first);
            }
            else if (temp.compare("+") == 0)
            {
                nfa_first = plus_op(nfa_first);
            }
            stack_process.push(nfa_first);
        }
        else if(is_binary_op(temp))
        {
            // it is binary operation then pop the last two machines and
            // apply operation to them then push it back.
            NFA nfa_second = stack_process.top();
            stack_process.pop();
            NFA nfa_first = stack_process.top();
            stack_process.pop();
            if(temp.compare("AND") == 0)
            {
                NFA nfa_comined = conc_op(nfa_first,nfa_second);
                stack_process.push(nfa_comined);
            }
            else if (temp.compare("OR") == 0)
            {
                NFA nfa_comined = or_op(nfa_first,nfa_second);
                stack_process.push(nfa_comined);
            }
        }
    }
    NFA final_one = stack_process.top();
    stack_process.pop();
    return final_one;
}

// get NFA for punctuations
NFA get_nfa_punctuation(vector <char> punctuations)
{
    vector <NFA> nfa_punct;
    for(int i=0; i<punctuations.size(); i++)
    {
        NFA temp = create_machine(punctuations.at(i),string (1,punctuations.at(i)));
        nfa_punct.push_back(temp);
    }
    return combine(nfa_punct);

}

// create NFA for each regular expression
vector <NFA> get_NFA_expressions(vector<regularExpression> expressions)
{
    vector <NFA> all_expressions;
    for(int i=0; i<expressions.size(); i++)
    {
        string token = expressions.at(i).expressionName;
        vector<Node *> postfix_expression = expressions.at(i).postfixNodes;
        NFA current_machine = get_nfa_expression(token,postfix_expression);
        all_expressions.push_back(current_machine);
    }
    return all_expressions;
}

// create NFA for each regular definition
vector <NFA> get_NFA_definitions(vector<regularDefinition> definitions)
{
    vector <NFA> all_definitions;
    for(int i=0; i<definitions.size(); i++)
    {
        string token = definitions.at(i).definitionName;
        vector<Node *> postfix_expression = definitions.at(i).postfixNodes;
        NFA current_machine = get_nfa_expression(token,postfix_expression);
        all_definitions.push_back(current_machine);
    }
    return all_definitions;
}

// create NFA  for each of keywords
vector <NFA> get_NFA_keywords(vector<keyWord> keywords)
{
    vector <NFA> all_keywords;
    for(int i=0; i<keywords.size(); i++)
    {
        string token = keywords.at(i).keyWordName;
        vector<Node *> postfix_expression = keywords.at(i).postfixNodes;
        NFA current_machine = get_nfa_expression(token,postfix_expression);
        all_keywords.push_back(current_machine);
    }
    return all_keywords;
}



///////////////////////// fill grammar ///////////////////

// get chars only of regular definitions to store them in grammar
// without operations
vector<char> get_operands_only(vector<Node *> all_nodes)
{
    vector<char> all_operands;
    for(int i=0; i<all_nodes.size(); i++)
    {
        string name = all_nodes.at(i)->getName();
        bool is_unary = all_nodes.at(i)->iShasUnaryOperation();
        if(!is_unary && !is_binary_op(name))
        {
            char c = name[0];
            all_operands.push_back(c);
        }
    }
    return all_operands;
}

// get all regular definitions of input rules
vector <Regular_Definition> get_grammar_regdefs(vector<regularDefinition> all_definitions)
{
    vector<Regular_Definition> reg_defs;
    for(int i=0; i<all_definitions.size(); i++)
    {
        Regular_Definition reg_def;
        reg_def.name = all_definitions.at(i).definitionName;
        reg_def.values = get_operands_only(all_definitions.at(i).postfixNodes);
        reg_defs.push_back(reg_def);
    }
    return reg_defs;
}
// get all keywords of input rules
vector <string> get_grammar_keywords(vector<keyWord> all_keywords)
{
    vector <string> keywords;
    for (int i=0; i< all_keywords.size(); i++)
    {
        string keyword = all_keywords.at(i).keyWordName;
        keywords.push_back(keyword);
    }
    return keywords;
}



// the main goal in this function is to get NFAs of all used rules
// of input file (keywords , punctuations , expressions ) in this
// order to to give higher priorities to keywords and punctuations
// and form one machine only to pass the input to this machine after
// minimization
vector<NFA> get_NFA_all_rules()
{
    vector <NFA> final_NFA;
    vector <NFA> all_expressions = get_NFA_expressions(expressions);
    vector <NFA> all_keywords = get_NFA_keywords(keyWords);
    if(punctuations.size()>0)
    {
        NFA all_punctuations = get_nfa_punctuation(punctuations);
        final_NFA.push_back(all_punctuations);
    }
    final_NFA.insert(final_NFA.end(),all_keywords.begin(),all_keywords.end());
    final_NFA.insert(final_NFA.end(),all_expressions.begin(),all_expressions.end());
    return final_NFA;
}


// filling the grammar
void set_grammar()
{
    Grammar.set_keywords(get_grammar_keywords(keyWords));
    Grammar.set_punc_sym(punctuations);
    Grammar.set_reserved(reservedSymbols);
    Grammar.set_productions(get_NFA_all_rules());
    Grammar.set_reg_def(get_grammar_regdefs(definitions));
}






/////////////////////////////////  DFA  functions  //////////////////

//given a node pointer it gets all the equivalent nodes to this node
//i.e: all those an empty string transition lead to
NFA build_dfa(NFA nfa)
{
//initializing the DFA
    node * null = new node;
    null->node_number = -1;
    NFA dfa(null);
//Getting the equivalent start node
    vector<node*> equivalent_nodes;
    equivalent_nodes = get_equivalent_nodes(nfa.get_start(),equivalent_nodes);
    node *start = get_equivalent_dfa_node(dfa,equivalent_nodes);
    start->token = get_token_least(equivalent_nodes);
    dfa.set_start(start);
    if(is_final(start))
        dfa.add_final_state(start);
    else
        dfa.add_state(start);
//a map to map each dfa node to its equivalent nfa nodes
    map<int,vector<node*> > equivalent;
    equivalent[start->node_number] = equivalent_nodes;
//for each added DFA node
    for(int i = 0; i<dfa.get_states().size(); i++)
    {
        //current dfa node
        node * current_dfa = dfa.get_states()[i];
        //current equivalent set
        vector<node*> current_equivalent = equivalent[current_dfa->node_number];
        //now lets set this DFA node transitions to be same as all the transitions corresponding to the nfa set
        for(int j = 0; j < current_equivalent.size(); j++)
        {
            node * current_nfa = current_equivalent[j];
            std::map<char, vector<node*> >::iterator it = current_nfa->transition.begin();
            while(it != current_nfa->transition.end())
            {
                char input = it->first;
                if(input == NOINPUT)
                {
                    it++;
                    continue;
                }
                vector<node*> corresponding_outputs = it->second;
                for(int k = 0; k < corresponding_outputs.size(); k++)
                {
                    current_dfa->transition[input].push_back(corresponding_outputs[k]);
                }
                it++;
            }
        }
        //now lets create an equivalent dfa node for each of these transitions:
        std::map<char, vector<node*> >::iterator it = current_dfa->transition.begin();
        while(it != current_dfa->transition.end())
        {
            char input = it->first;
            vector<node*> result = it->second;
            result = remove_repetitions(result);
            //adding the equivalent nfa nodes of each node
            vector<node*> final_result;
            for(int l = 0; l < result.size(); l++)
            {
                vector<node*> temp;
                temp = get_equivalent_nodes(result[l],temp);
                for(int m = 0; m < temp.size(); m++)
                    final_result.push_back(temp[m]);
                final_result = remove_repetitions(final_result);
            }
            //getting equivalent dfa node
            node * equivalent_dfa_node = get_equivalent_dfa_node(dfa,final_result);
            equivalent_dfa_node->token = get_token_least(final_result);
            //add this dfa node to be the corresponding transition
            vector<node*> temp_trans;
            temp_trans.push_back(equivalent_dfa_node);
            current_dfa->transition[input] = temp_trans;
            if(is_final(equivalent_dfa_node))
                dfa.add_final_state(equivalent_dfa_node);
            else
                dfa.add_state(equivalent_dfa_node);
            //remove repetitions in  DFA nodes
            dfa.remove_rep();
            //setting its equivalent nfa nodes
            equivalent[equivalent_dfa_node->node_number] = final_result;
            it++;
        }
    }

    dfa.set_num_states(equivalent.size());
    return dfa;
}

//gets the token of the least numbered node to follow the precedence priority constraint
string get_token_least(vector<node*>nodes)
{
    int least = INT_MAX;
    string result = "";
    for(int i = 0; i < nodes.size(); i++)
    {
        node* current_node = nodes[i];
        if(current_node->node_number < least && is_final(current_node))
        {
            least = current_node->node_number;
            result = current_node->token;
        }

    }
    return result;
}
//returns a new node with the corresponding numerical value or the repeated DFA node
node * get_equivalent_dfa_node(NFA dfa,vector<node *>nfa_nodes)
{
    int number = equivalent_number(nfa_nodes);
    vector<node*> current_states = dfa.get_states();
    for(int i = 0; i < current_states.size(); i++)
    {
        node * current_node = current_states[i];
        if(current_node->node_number == number)
            return current_node;
    }
    node * new_node = new node;
    new_node->node_number = number;
    return new_node;
}

//returns the equivalent numerical value sorted. Eliminates repetitions too.
int equivalent_number(vector<node*> nfa_nodes)
{
    vector<node*> no_repititions = remove_repetitions(nfa_nodes);
    vector<int> numbers;
    for(int i = 0; i < no_repititions.size(); i++)
    {
        numbers.push_back(no_repititions[i]->node_number);
    }
    sort(numbers.begin(),numbers.end());
    float result = numbers[0];
    for(int i = 1; i < numbers.size(); i++)
    {

        result += (1/2)*(numbers[i-1]+numbers[i])*(numbers[i-1]+numbers[i]+1)+numbers[i-1];
        if(i<numbers.size()-1)
            result += (1/2)*(numbers[i]+numbers[i+1])*(numbers[i]+numbers[i+1]+1)+numbers[i+1];
        if(i<numbers.size()-2){
            result += (1/2)*(numbers[i+1]+numbers[i+2])*(numbers[i+1]+numbers[i+2]+1)+numbers[i+2];
            result -= (1/2)*(numbers[i]+numbers[i+2])*(numbers[i]+numbers[i+2]+1)+numbers[i];
        }
        if(i<numbers.size()-3){
            result += (1/2)*(numbers[i+2]+numbers[i+3])*(numbers[i+2]+numbers[i+3]+1)+numbers[i+3];
            result -= (1/2)*(numbers[i+1]+numbers[i+3])*(numbers[i+1]+numbers[i+3]+1)+numbers[i+1];
            result += (1/2)*(numbers[i]+numbers[i+3])*(numbers[i]+numbers[i+3]+1)+numbers[i-1];
        }
    }

    result += (1/2)*(result +numbers.size())*(result+numbers.size()+1)+result;
    return (int)result;
}


vector<node*> get_equivalent_nodes(node* n,vector<node*>result)
{
    result.push_back(n);
    if(n->transition.find(NOINPUT) == n->transition.end())
    {
        result.push_back(n);
        return result;
    }
    vector<node*> transitions_on_empty = n->transition[NOINPUT];
    for(int i = 0; i < transitions_on_empty.size(); i++)
    {
        vector<node*> temp;
        //to prevent self loops
        if(transitions_on_empty[i]->node_number == n->node_number)
            continue;
        //to prevent cycles
        bool there = false;
        for(int j = 0; j < result.size(); j++)
        {
            if(result[j]->node_number == transitions_on_empty[i]->node_number)
                there = true;
        }
        if(there)
            continue;
        temp = get_equivalent_nodes(transitions_on_empty[i],result);
        for(int j = 0; j < temp.size(); j++)
            result.push_back(temp[j]);
    }
    return remove_repetitions(result);
}
//Takes a node vector and removes all the repetitions in it
vector<node*> remove_repetitions(vector<node*>nodes)
{
    bool is_there = false;
    vector<node*> result;
    for(int i = 0; i < nodes.size(); i++)
    {
        is_there = false;
        node* current_node = nodes[i];
        for(int j = 0; j < result.size(); j++)
            if(result[j]->node_number == current_node->node_number)
                is_there = true;
        if(is_there)
            continue;
        result.push_back(current_node);
    }
    return result;
}


////////////////////////////////  DFA minimized functions ///////////



NFA minimize(NFA machine)
{
    //getting the sates from the machine.
    vector<node *> states_non_final = machine.get_non_final_states();
    vector<node *> states_final = machine.get_final_states();
    //A vector of partitions where each partition is a vector of nodes
    //We keep partitioning till no more partition is done
    //IE: when the partitions size does not change.
    //We consider a different partition for each of the accept states According to their
    //corresponding token.
    vector< vector<node*> > partitions;
    //first partitioning the final states according to their tokens
    vector< vector<node*> >final_partitions = partition_final(states_final);
    for(int i = 0; i < final_partitions.size(); i++)
    {
        partitions.push_back(final_partitions[i]);
    }
    //Next we add the first partition of non-final states
    //We need to change their node numbers to represent their partition.
    change_numbers(states_non_final,partitions.size());
    if(states_non_final.size() != 0)
    {
        partitions.push_back(states_non_final);
    }
    //Now we begin the iterative partitioning algorithm till no more partitions are found
    // we need to keep the old number of partitions
    int old_number = partitions.size();
    //we keep checking till no more partitions
    bool finished = false;
    //shows if we need to partition this node or not
    bool need_partition = false;
    //to show if a new group was found
    bool found_group = false;
    while(!finished)
    {
        old_number = partitions.size();
        //Each partition is checked
        for(int i = 0; i < partitions.size(); i++)
        {
            vector<node*> current_partition = partitions[i];
            //each node in this partition is checked
            for(int j = 0; j < current_partition.size(); j++)
            {
                node* current_node = current_partition[j];
                if(current_node->node_number != i)
                    continue;
                int node_index = j;
                need_partition = false;
                //it is checked with each other node
                for(int k = 0; k < current_partition.size(); k++)
                {
                    if(current_partition[k]->node_number!=i)
                        continue;
                    if(!equivalent(current_partition[k],current_node))
                    {
                        need_partition = true;
                        break;
                    }
                }
                //partition this node if needed and continue to the next iteration
                if(need_partition)
                {
                    found_group = false;
                    //look in the new partitions if a partition suits this node or not
                    for(int l = old_number; l < partitions.size(); l++)
                    {
                        int correct_loc = 0;
                        vector<node*> looking_in = partitions[l];
                        while(looking_in[correct_loc]->node_number != l)
                        {

                            correct_loc++;
                        }
                        if(equivalent(current_node,looking_in[correct_loc]))
                        {
                            //add the node to this group and remove it from the old one
                            current_node->node_number = l;
                            looking_in.push_back(current_node);
                            found_group = true;
                            break;
                        }
                    }
                    //no group found so create a new one
                    if(!found_group)
                    {
                        partitions.push_back(vector<node*>());
                        partitions[partitions.size()-1].push_back(current_node);
                        current_node->node_number = partitions.size()-1;
                    }
                }
            }
        }
        //If no new partition was added
        if(old_number == partitions.size())
            finished = true;
    }

    //Having completed the partitions we now create the corresponding machine
    NFA minimized(machine.get_start());
    if(is_final(minimized.get_start()))
        minimized.add_final_state(minimized.get_start());
    else
        minimized.add_state(minimized.get_start());
    //Adding a node corresponding to each partition
    for(int i = 0; i < partitions.size(); i++)
    {
        node* current_node;
        int location = 0;
        while(partitions[i][location]->node_number != i)
        {
            location++;
        }
        current_node = partitions[i][location];
        if(current_node->node_number != minimized.get_start()->node_number)
        {
            if(is_final(current_node))
                minimized.add_final_state(current_node);
            else
                minimized.add_state(current_node);
        }
    }
    minimized.set_num_states(partitions.size());
    return minimized;
}
//checks whether this node is a final state or not
bool is_final(node* n)
{
    return n->token!="";
}
//checks that the two vectors have the same node numbers
bool have_same_nodes(vector<node*> nodes1,vector<node*> nodes2)
{
    if(nodes1.size()!=nodes2.size())
        return false;
    bool is_there = false;
    for(int i = 0; i < nodes1.size(); i++)
    {
        is_there = false;
        for(int j = 0; j < nodes2.size(); j++)
        {
            if(nodes1[i]->node_number == nodes2[j]->node_number)
                is_there = true;
        }
        if(!is_there)
            return false;
    }
    return true;
}
//checks if two nodes from the same group have the same transitions and if so can be considered
// equivalent
bool equivalent(node* node1,node* node2)
{
    if(node1->transition.size() != node2->transition.size())
    {
        return false;
    }
    if(node1->token!=node2->token)
        return false;
    std::map<char, vector<node*> >::iterator it = node1->transition.begin();
    while(it!= node1->transition.end())
    {
        char input = it -> first;
        vector<node*> outputs1 = it -> second;
        if(node2->transition.find(input) == node2->transition.end())
            return false;
        vector<node*> outputs2 = node2->transition[input];
        if(outputs1[0]!=outputs2[0])
            return false;
        it++;
    }
    return true;
}

//Takes a vector of nodes and a number and sets their numbers accordingly
void change_numbers(vector<node*> nodes,int number)
{
    for(int i = 0; i < nodes.size(); i++)
        nodes[i] ->node_number= number;
}
//partitions the final states according to their tokens.
//Each node is assigned a new number corresponding to it's partition.
//notice that their numbering is from 0 as they will be in the first partitions.
vector< vector<node*> >partition_final(vector<node*> final_states)
{
    vector < vector <node*> >result;
    bool partition_found = false;
    for(int i = 0; i < final_states.size(); i++)
    {
        node* current_node = final_states[i];
        partition_found = false;
        for(int j = 0; j < result.size(); j++)
        {
            if(result[j][0] -> token == current_node->token)
            {
                partition_found = true;
                result[j].push_back(current_node);
                current_node->node_number = j;
                break;
            }
        }
        if(!partition_found)
        {
            result.push_back(vector<node*>());
            result[result.size()-1].push_back(current_node);
            current_node->node_number = result.size()-1;
        }
    }
    return result;
}



//////////////////////////////////// functions of final parser ///////
vector<string> get_all_tokens()
{
    return all_tokens;
}

vector<string> get_all_tokens_values()
{
    return all_tokens_values;
}

pair<string,string> get_next_token(string input,NFA dfa)
{
    dfa.set_current_state(dfa.get_start());
    pair<string,string> result;
    string token = "Error";
    char current_input;
    string att_value = "";
    for(int i = 0; i < input.size(); i++)
    {
        node * current_node = dfa.get_current_state();
        current_input = input[i];
        //ignore spaces and end characters.
        if(current_input == ' '||current_input == '\t'||current_input=='\n')
        {
            break;
        }
        dfa.change_state(current_input);
        node * next_node = dfa.get_current_state();

        if( current_node->transition.find(current_input) ==  current_node->transition.end())
        {
            if(token == "Error")
                att_value+=current_input;
            break;
        }
        att_value+=current_input;
        if(is_final(next_node))
        {
            token = next_node->token;
        }
    }
    if(token == identifier && symbol_table.find(att_value) == symbol_table.end()){
        symbol_table[att_value].first = location_in_local_array;
        location_in_local_array++;
    }
    result.first = token;
    result.second = att_value;
    all_tokens.push_back(result.first);
    all_tokens_values.push_back(result.second);
    return result;
}


string concatenate_strs(vector<string> myinput)
{
    string concatenated = "";
    for(int i=0; i<myinput.size(); i++)
    {
        concatenated += myinput.at(i);
        concatenated += " ";
    }
    return concatenated;
}


void print_machine_file(NFA machine)
{
    int num_states = machine.get_states().size();
    outfile << endl << "MINIMIZED DFA MACHINE: " << endl;
    outfile<<"initial state: "<<machine.get_start()->node_number<<endl;
    vector<node*> nodes = machine.get_states();
    vector<int> found;
    node* current;
    for(int i = 0; i < nodes.size(); i++)
    {
        current = nodes[i];
        std::map<char, vector<node*> >::iterator it = current->transition.begin();

        if(!contains(found,current->node_number))
        {
            while(it!= current->transition.end())
            {
                outfile<<"from "<<current->node_number<<" with input "<<it->first<<" to "<<get_states(it->second)<<endl;
                it++;
            }
            found.push_back(current->node_number);

        }
        else
        {
            num_states--;
        }
    }
    outfile<<endl;
    vector<node*> final_states = machine.get_final_states();
    outfile<<"final states: " << endl;
    found.clear();
    for(int i = 0; i < final_states.size(); i++)
    {
        if(!contains(found,final_states[i]->node_number))
            outfile <<final_states[i]->node_number<<" result: "<<final_states[i] ->token<<" " << endl;
        found.push_back(final_states[i]->node_number);
    }
    outfile<<endl;
    outfile<<"number of states: "<<num_states<<endl;
    outfile<<endl;
}
bool contains(vector<int> numbers,int num)
{
    for(int i = 0; i < numbers.size(); i++)
    {
        if(numbers[i] == num)
            return true;
    }
    return false;
}
void print_output(string input,NFA dfa)
{
    outfile.open(output_filename);
    //printing the lexical analysis for the input file
    outfile<<"Lexical Output:"<<endl;
    while(input!="")
    {
        input = remove_firstspaces(input);
        if(input == "")
        {
            break;
        }
        pair<string,string> current = get_next_token(input,dfa);
        outfile<<"token: "<<current.first<<" value: "<<current.second<<endl;
        //resetting
        input = input.substr(current.second.size(),input.size());
    }
    outfile<<endl;
    outfile<<"Symbols in symbol table:"<<endl;
    for(std::map<string,pair<int,string>>::iterator it=symbol_table.begin(); it != symbol_table.end(); it++)
    {
        outfile<<"id: "<<it->first<<" location in local variable array: "<<it->second.first<<endl;
    }
    print_machine_file(dfa);
    outfile.close();
}


////////////////////////////////main //////////////////////////

void lexical_analyzer()
{
    parseInput();
    set_identifier_global();
    set_grammar();

    NFA final_NFA = combine(Grammar.get_productions());
    NFA DFA_machine = build_dfa(final_NFA);

    for(int i=0; i<final_NFA.get_num_states(); i++)
    {
        delete(final_NFA.get_states().at(i));
    }

    NFA DFA_minimized = minimize(DFA_machine);

    string in = concatenate_strs(read_file(input_filename));
    print_output(in,DFA_machine);
}

