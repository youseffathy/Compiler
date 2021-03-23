#ifndef PHASE1_H_INCLUDED
#define PHASE1_H_INCLUDED

#define INT "int"
#define FLOAT "float"
using namespace std;



////////////////// Data structures definition in initial parser/////
map<string,pair<int,string>> get_symbol_table();
void set_symbol_table(map<string,pair<int,string>> temp_map);
// node holds data of expression tree
class Node
{
private:

    string name;
    bool hasUnaryOperation = false;
    int precedence = 0;
public:

    //setters

    void setName(string name)
    {
        this->name = name;
    }
    void setHasunaryOperation(bool hasUnaryOperation)
    {
        this->hasUnaryOperation = hasUnaryOperation;
    }


    void setPrecedence(int precedence)
    {
        this->precedence = precedence;
    }

    //getters
    string getName()
    {
        return this->name;
    }

    bool iShasUnaryOperation()
    {
        return this->hasUnaryOperation;
    }

    int getPrcendence()
    {
        return this->precedence;
    }

};

struct regularExpression
{
    string expressionName;
    vector <Node*> postfixNodes;
};

struct regularDefinition
{
    string definitionName;
    vector <Node*> postfixNodes;
};

struct keyWord
{
    string keyWordName;
    vector <Node*> postfixNodes;

};







//////////////Data structures definitions for NFA/////////////

//A structure for the NFA node
struct node
{
    int node_number;
    //map for transitions
    map<char,vector<node*> > transition;
    //The token associated with reaching this node
    string token = "";
};

//A Structure for the regular definitions implementation:
struct Regular_Definition
{
    //The data stored in a regular definition
    //A vector for the values in this regular definition.
    vector<char> values;
    //The name identifying this regular definition
    string name;

};

vector<node*> remove_repetitions(vector<node*>);

//A class to implement the NFA that captures a production in the grammar
class NFA
{
    //The data structures needed
private:
    //The current state this NFA is in
    node* current_state;
    //The starting state
    node* start;
    //The number of states in this NFA
    int num_states;
    //contains the final states
    vector<node*> final_states;
    //contains all the states in this machine
    vector<node*> states;
    //contains all the non final states in this machine
    vector<node*> non_final_states;
public:
    //constructor
    NFA(node* start)
    {
        this -> start = start;
        this -> current_state = start;
    }
    //setters and getters
    void set_num_states(int num_states)
    {
        this->num_states = num_states;
    }
    int get_num_states()
    {
        return num_states;
    }
    void set_current_state(node* current_state)
    {
        this->current_state = current_state;
    }
    node* get_current_state()
    {
        return current_state;
    }
    vector<node*> get_final_states()
    {
        return final_states;
    }
    vector<node*> get_states()
    {
        return states;
    }
    vector<node*> get_non_final_states()
    {
        return non_final_states;
    }
    node* get_start()
    {
        return start;
    }
    void set_start(node* start)
    {
        this ->start = start;
    }
    //some utility functions
    void add_token(string token,node* state)
    {
        state->token = token;
    }
    void add_translation(node *from,node *to,char input)
    {
        from->transition[input].push_back(to);
    }
    bool atfinal()
    {
        if(current_state->token != "")
            return true;
        return false;
    }
    string get_token()
    {
        if(atfinal())
            return current_state->token;
        return "Not at final state";

    }
    void change_state(char input)
    {
        if(current_state->transition.find(input) != current_state->transition.end() && current_state->transition[input].size() == 1)
            current_state = current_state->transition[input].front();
    }
    vector<node *> get_next_states(char input)
    {
        return current_state->transition[input];
    }
    //For adding and retrieving final states
    void add_final_state(node *final_state)
    {
        states.push_back(final_state);
        final_states.push_back(final_state);
    }
    //for adding a new node (non final node)
    void add_state(node *new_state)
    {
        states.push_back(new_state);
        non_final_states.push_back(new_state);

    }
    //for increasing the node numbers by a certain number
    void inc_nodes(int number)
    {
        for(int i = 0; i < states.size(); i++)
            states[i]->node_number += number;
    }
    void push_new_states(NFA machine)
    {
        vector<node*> new_states = machine.get_states();
        vector<node*> new_final_states = machine.get_final_states();
        vector<node*> new_non_final_states = machine.get_final_states();
        for(int i = 0; i<new_states.size(); i++)
            states.push_back(new_states[i]);
        for(int i = 0; i<new_final_states.size(); i++)
            final_states.push_back(new_final_states[i]);
        for(int i = 0; i<new_non_final_states.size(); i++)
            non_final_states.push_back(new_non_final_states[i]);
    }
    node* get_state(int node_number)
    {
        //Shows an error
        node* null = new node;
        null->node_number = -1;
        for(int i = 0; i < states.size(); i++)
        {
            if(states[i]->node_number == node_number)
                return states[i];
        }
        return null;
    }
    void remove_rep()
    {
        states = remove_repetitions(states);
        final_states = remove_repetitions(final_states);
        non_final_states = remove_repetitions(non_final_states);
    }


};







/////////////////////////// prototypes of initial parser functions////////
string remove_firstspaces(string);
vector<string> get_betweenbrackets(string);
vector<Node *> getNodes(string);
vector<string> read_file(string);
void parseInput();



////////////////////// link NFA to parser  prototypes///////

bool is_alphabet(char);
string get_identifier(vector<regularExpression>);
bool is_binary_op(string);
bool has_next_or(int, vector <Node*>);
NFA get_nfa_expression(string,vector < Node* >);
NFA get_nfa_punctuation(vector <char>);
vector <NFA> get_NFA_expressions(vector<regularExpression>);
vector <NFA> get_NFA_definitions(vector<regularDefinition>);
vector <NFA> get_NFA_keywords(vector<keyWord>);
vector<char> get_operands_only(vector<Node *>);
vector <Regular_Definition> get_grammar_regdefs(vector<regularDefinition>);
vector <string> get_grammar_keywords(vector<keyWord>);
vector<NFA> get_NFA_all_rules();
void set_grammar();
void set_identifier_global ();



/////////////////////// prototypes of NFA//////////////////////

//Utilities
//functions making  test machines corresponding to those in the slides
NFA test1();
NFA test2();
NFA test3();
//A function that tests the combination of the test NFAs
void test_combination();
//A function that prints the machine data
void print_machine(NFA);
//A function given a vector of node pointers prints their corresponding numbers
string get_states(vector<node*> state);

//Main functions
//A function given a vector of NFAs combines them into a single NFA
//It numbers the nodes in ascending order such that the NFA having higher precedence
//would have a lower node number(for final states of machines)
NFA combine(vector<NFA>);

//Utility functions used in building the NFA
NFA or_op(NFA,NFA);
NFA conc_op(NFA,NFA);
void set_token(NFA,string);
NFA create_machine(char,string);
NFA star_op(NFA);
NFA plus_op(NFA);
NFA copy_machine(NFA);





/////////////////////////////////// DFA prototypes //////////////////

NFA build_dfa(NFA);
//Utility functions used in building a DFA
vector<node*> get_equivalent_nodes(node*,vector<node*>);
node* get_equivalent_dfa_node(NFA,vector<node*>);
int equivalent_number(vector<node*>);
string get_token_least(vector<node*>);


//////////////////////////////  DFA  minimized  prototypes /////////////////////


//A function that given a DFA minimizes it
//It takes an NFA class DFA machine and returns the minimized DFA of it
NFA minimize(NFA);
//Utility functions used in the minimization process
vector< vector<node*> > partition_final(vector<node*>);
void change_numbers(vector<node*>,int);
bool equivalent(node*,node*);
bool have_same_nodes(vector<node*>,vector<node*>);
bool is_final(node*);


//////////////////// prototypes of final parser ///////////

//NEW DEFINITIONS
//Pattern parsing functions (Lexical analysis)
vector<string> get_all_tokens();
vector<string> get_all_tokens_values();
pair<string,string> get_next_token(string,NFA);
vector<string> remove_error(string);
string concatenate_strs(vector<string>);
void remove_duplicates();
void print_output(string,NFA);


//////////////////////// main////////////
void lexical_analyzer();

//Utility functions used in printing
bool contains(vector<int>,int);


#endif // PHASE1_H_INCLUDED
