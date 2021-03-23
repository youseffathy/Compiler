#ifndef PHASE2_PARSER_H_INCLUDED
#define PHASE2_PARSER_H_INCLUDED

using namespace std;

struct nonTerminal_parser{

	string name;
	vector<vector<string> > productions;
};

string remove_lastspaces(string);
string remove_all_spaces_out(string);
vector<string> split_string(string ,char);
vector<nonTerminal_parser> readfile_syn_rules();

#endif // PHASE2_PARSER_H_INCLUDED
