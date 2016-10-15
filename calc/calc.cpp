#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stack>
#include <vector>
#include <iostream>


using namespace std;


/*** Enums and Print Functions for Terminals and Non-Terminals  **********************/

#define MAX_SYMBOL_NAME_SIZE 25

//all of the terminals in the language
typedef enum {
	T_eof = 0,	// 0: end of file
	T_num,		// 1: numbers
	T_plus,		// 2: +
	T_minus,	// 3: - 
	T_times,	// 4: *
	T_period,	// 5: .
	T_modulo, 	// 6: % 
	T_openparen,	// 7: (
	T_closeparen 	// 8: )
} token_type;


char* token_to_string(token_type c) {
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch( c ) {
		case T_eof: strncpy(buffer,"eof",MAX_SYMBOL_NAME_SIZE); break;
		case T_num: strncpy(buffer,"num",MAX_SYMBOL_NAME_SIZE); break;
		case T_plus: strncpy(buffer,"+",MAX_SYMBOL_NAME_SIZE); break;
		case T_minus: strncpy(buffer,"-",MAX_SYMBOL_NAME_SIZE); break;
		case T_times: strncpy(buffer,"*",MAX_SYMBOL_NAME_SIZE); break;
		case T_period: strncpy(buffer,".",MAX_SYMBOL_NAME_SIZE); break;
		case T_modulo: strncpy(buffer,"%",MAX_SYMBOL_NAME_SIZE); break;
		case T_openparen: strncpy(buffer,"(",MAX_SYMBOL_NAME_SIZE); break;
		case T_closeparen: strncpy(buffer,")",MAX_SYMBOL_NAME_SIZE); break;
		default: strncpy(buffer,"unknown_token",MAX_SYMBOL_NAME_SIZE); break;
	}
	return buffer;
}

//all of the non-terminals in the grammar (you need to add these in
//according to your grammar.. these are used for printing the thing out)
//please follow the convention we set up so that we can tell what the heck you
//are doing when we grade
typedef enum 
{
	epsilon = 100,
	NT_List,	// L
	NT_Expr,	// E
	NT_K,       // K, it is just some intermidiate non-terminal
	NT_Term,	// T
	NT_Factor,	// F
	NT_Expr_Prime,	// Ee
	NT_Term_Prime	//	Tt

} nonterm_type;


char* nonterm_to_string(nonterm_type nt) {
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	switch( nt ) {
		  case epsilon: strncpy(buffer,"e",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_List: strncpy(buffer,"List",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_Expr: strncpy(buffer,"Expression",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_K: strncpy(buffer,"K",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_Term: strncpy(buffer,"Term",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_Factor: strncpy(buffer,"Factor",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_Expr_Prime: strncpy(buffer,"Expression_Prime",MAX_SYMBOL_NAME_SIZE); break;
		  case NT_Term_Prime: strncpy(buffer,"Term_Prime",MAX_SYMBOL_NAME_SIZE); break;

		  default: strncpy(buffer,"unknown_nonterm",MAX_SYMBOL_NAME_SIZE); break;
		}
	return buffer;
}

/*** Scanner Class ***********************************************/

class scanner_t {
  public:

	void eat_token(token_type c);	//eats the next token and prints an error if it is not of type c
	token_type next_token();	//peeks at the lookahead token
	int get_line();			//return line number for errors
	scanner_t();			//constructor - inits g_next_token

	void scan_string();		// this will fill string_tokens, Ex: [1, +, 33, *, 44]
	void modify_token();	// this will modify string_tokens to real tokens, Ex:  [num, T_plus, num, T_times, num]

	void print_string_tokens();
	void print_tokens();

 // private:

	vector<string> string_tokens;   	// stores strings 
	vector<token_type> tokens;	// stores real tokens
	int line;                   // line number
	int index;                  // current (next) token index
	int s_index;				// current (next) string_token index
	
	void scan_error(char x);	//error message and exit if weird character
	void mismatch_error(token_type c);	//error message and exit for mismatch

};


token_type scanner_t::next_token() {
	//if (index == tokens.size())
    if (!(index<tokens.size()))   
		return T_eof;   // no more tokens, end of file
	else
		return tokens.at(index);	// else return current (next) token
}


void scanner_t::eat_token(token_type c) {
	/*
	cout << "***** eat:";
	cout <<	token_to_string(tokens.at(index));
	cout << token_to_string(c);
	cout << endl;	
*/

	if (c != tokens.at(index)) { mismatch_error(c); }


	else if (string_tokens.at(s_index) == "\n") {
		line++;
		s_index++;

	}

		index++;
		s_index++;

	
	//cout << "finished eating" << endl;
}


scanner_t::scanner_t() {
	this->line = 1;
	this->index = 0;
	this->s_index = 0;
	// printf("finished initializing\n");

	scan_string();	
	//printf("finished scan_string()\n");
    //print_string_tokens();
    //cout << endl;
	
	modify_token();	
	//printf("finished modify_token()\n");
	//print_tokens();
	//cout << endl;
}


void scanner_t::scan_string() {
	string token;
	char c;

	while( (c = getchar()) != EOF ) {
		while(c == ' ') {c = getchar();}	// eliminate whitespaces

		if( c=='\n' || c=='+' || c=='-' || c=='*' || c=='.' || c=='%' || c=='(' || c==')' ) {
			token += c;
			string_tokens.push_back(token);
			token = "";
		}

		else if (c == EOF) { return; }

		// if c is a number, need to determine how many digits the whole number is
		else if (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9') {
			token += c;		// first digit
			c = getchar();	// next digit
			while ( c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9') {
				// if still number, add it to the whole number
				token += c;
				c = getchar();
			}
			string_tokens.push_back(token);
			token = "";

			/*** add the extra one token ***/
		        while(c == ' ') {c = getchar();}	// eliminate whitespaces

			if( c=='\n' || c=='+' || c=='-' || c=='*' || c=='.' || c=='%' || c=='(' || c==')' ) {
			  token += c;
			  string_tokens.push_back(token);
			  token = "";
			}

			else if (c == EOF) {  // 1+4+55555
			  //string_tokens.push_back(token);
			  return;
			}

			else
			  scan_error(c);   // 111kk111
			/*** finished add extra token ***/
		}
		// end c is a number

		else 
			scan_error(c);
	} // end while

	string_tokens.push_back("EOF");	// finished reading inputs
}


void scanner_t::modify_token() {
	for(int i=0; i<string_tokens.size(); i++) {
		if(string_tokens.at(i) == "+") { tokens.push_back(T_plus); }
		else if (string_tokens.at(i) == "-") { tokens.push_back(T_minus); }
		else if (string_tokens.at(i) == "*") { tokens.push_back(T_times); }
		else if (string_tokens.at(i) == ".") { tokens.push_back(T_period); }
		else if (string_tokens.at(i) == "%") { tokens.push_back(T_modulo); }
		else if (string_tokens.at(i) == "(") { tokens.push_back(T_openparen); }
		else if (string_tokens.at(i) == ")") { tokens.push_back(T_closeparen); }
		else if (string_tokens.at(i) == "EOF") { tokens.push_back(T_eof); }
		else if (string_tokens.at(i)[0] == '0') { tokens.push_back(T_num); }	
		else if (string_tokens.at(i)[0] == '1') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] == '2') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] == '3') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] == '4') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] == '5') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] == '6') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] == '7') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] == '8') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] == '9') { tokens.push_back(T_num); }
		else if (string_tokens.at(i)[0] != '\n'){ scan_error(string_tokens.at(i)[0]); }
	}
}


int scanner_t::get_line() {
	return line;
}


void scanner_t::scan_error (char x) {
	printf("scan error: unrecognized character '%c'\n",x);  
	exit(1);
}


void scanner_t::mismatch_error (token_type x) {
	printf("syntax error: found %s ",token_to_string(next_token()) );
	printf("expecting %s - line %d\n", token_to_string(x), get_line());
	exit(1);
}


void scanner_t::print_string_tokens() {
	for (int i=0; i<string_tokens.size(); i++) {
		cout << string_tokens.at(i);
		printf(" ");
	}
	printf("\n");
}


void scanner_t::print_tokens() {
	for (int i=0; i<tokens.size(); i++) {
	  cout << token_to_string(tokens.at(i));
		printf(" ");
	}
	printf("\n");
}


/*** ParseTree Class **********************************************/

//just dumps out the tree as a dot file.  The interface is described below
//on the actual methods.  This class is full and complete.  You should not
//have to touch a thing if everything goes according to plan.  While you don't
//have to modify it, you will have to call it from your recursive decent
//parser, so read about the interface below.
class parsetree_t {
  public:
	void push(token_type t);
	void push(nonterm_type nt);
	void pop();
	void drawepsilon();
	parsetree_t();
  private:
	enum stype_t{
		TERMINAL=1,
		NONTERMINAL=0,
		UNDEF=-1
	};

	struct stuple { 
		nonterm_type nt;
		token_type t;
		stype_t stype; 
		int uniq;
	};
	void printedge(stuple temp); //prints edge from TOS->temp
	stack<stuple> stuple_stack;
	char* stuple_to_string(const stuple& s); 
	int counter;
};


//the constructer just starts by initializing a counter (used to uniquely
//name all the parse tree nodes) and by printing out the necessary dot commands
parsetree_t::parsetree_t()
{
	counter = 0;
	printf("digraph G { page=\"8.5,11\"; size=\"7.5, 10\"\n");
}

//This push function taken a non terminal and keeps it on the parsetree
//stack.  The stack keeps trace of where we are in the parse tree as
//we walk it in a depth first way.  You should call push when you start
//expanding a symbol, and call pop when you are done.  The parsetree_t
//will keep track of everything, and draw the parse tree as you go.
//This particular function should be called if you are pushing a non-terminal
void parsetree_t::push(nonterm_type nt)
{
	counter ++;
	stuple temp;
	temp.nt = nt;
	temp.stype = NONTERMINAL;
	temp.uniq = counter;
	printedge( temp );
	stuple_stack.push( temp );
}

//same as above, but for terminals
void parsetree_t::push(token_type t)
{
	counter ++;
	stuple temp;
	temp.t = t;
	temp.stype = TERMINAL;
	temp.uniq = counter;
	printedge( temp );
	stuple_stack.push( temp );
}

//when you are parsing a symbol, pop it.  That way the parsetree_t will
//know that you are now working on a higher part of the tree.
void parsetree_t::pop()
{
	if ( !stuple_stack.empty() ) {
		stuple_stack.pop();
	}

	if ( stuple_stack.empty() ) {
		printf( "}\n" );
	}
}

//draw an epsilon on the parse tree hanging off of the top of stack
void parsetree_t::drawepsilon()
{
	push(epsilon);
	pop();
}

// this private print function is called from push.  Basically it
// just prints out the command to draw an edge from the top of the stack (TOS)
// to the new symbol that was just pushed.  If it happens to be a terminal
// then it makes it a snazzy blue color so you can see your program on the leaves 
void parsetree_t::printedge(stuple temp)
{
	if ( temp.stype == TERMINAL ) {
		printf("\t\"%s%d\" [label=\"%s\",style=filled,fillcolor=powderblue]\n",
		  stuple_to_string(temp),
		  temp.uniq,
		  stuple_to_string(temp));
	} else {
		printf("\t\"%s%d\" [label=\"%s\"]\n",
		  stuple_to_string(temp),
		  temp.uniq,
		  stuple_to_string(temp));
	}

	//no edge to print if this is the first node
	if ( !stuple_stack.empty() ) {
		//print the edge
		printf( "\t\"%s%d\" ", stuple_to_string(stuple_stack.top()), stuple_stack.top().uniq ); 
		printf( "-> \"%s%d\"\n", stuple_to_string(temp), temp.uniq );
	}
}

//just a private utility for helping with the printing of the dot stuff
char* parsetree_t::stuple_to_string(const stuple& s) 
{
	static char buffer[MAX_SYMBOL_NAME_SIZE];
	if ( s.stype == TERMINAL ) {
		snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", token_to_string(s.t) );
	} else if ( s.stype == NONTERMINAL ) {
		snprintf( buffer, MAX_SYMBOL_NAME_SIZE, "%s", nonterm_to_string(s.nt) );
	} else {
		assert(0);
	}

	return buffer;
}


/*** Parser Class ***********************************************/

//the parser_t class handles everything.  It has and instance of scanner_t
//so it can peek at tokens and eat them up.  It also has access to the
//parsetree_t class so it can print out the parse tree as it figures it out.
//To make the recursive decent parser work, you will have to add some
//methods to this class.  The helper functions are described below

class parser_t {
  private:
	scanner_t scanner;
	parsetree_t parsetree;
	void eat_token(token_type t);
	void syntax_error(nonterm_type);

	void List();
	//void K();
	void E();
	void Ee();
	void T();
	void Tt();
	void F();

  public:	
	void parse();
};


//this function not only eats the token (moving the scanner forward one
//token), it also makes sure that token is drawn in the parse tree 
//properly by calling push and pop.
void parser_t::eat_token(token_type t)
{
	parsetree.push(t);
	scanner.eat_token(t);
	parsetree.pop();
}

//call this syntax error wehn you are trying to parse the
//non-terminal nt, but you fail to find a token that you need
//to make progress.  You should call this as soon as you can know
//there is a syntax_error. 
void parser_t::syntax_error(nonterm_type nt)
{
	printf("syntax error: found %s in parsing %s - line %d\n",
		token_to_string( scanner.next_token()),
		nonterm_to_string(nt),
		scanner.get_line() ); 
	exit(1); 
}


void parser_t::parse()
{
	List();
}

void parser_t::List() {
	/*
	cout << "List() ";
	cout << scanner.s_index;
	cout << scanner.index;
	cout << token_to_string(scanner.next_token());
	cout << endl;
*/

	parsetree.push(NT_List);
	switch( scanner.next_token() ) {
		case T_eof:		
			parsetree.drawepsilon();
			break;

		case T_openparen:
		case T_num:
			E();
			eat_token(T_period);
			List();
			break;

		default:
			syntax_error(NT_List);
			break;
	}
	parsetree.pop();
}
/*
void parser_t::K() {
	cout << "K() ";
	cout << scanner.s_index;
	cout << scanner.index;
	cout << token_to_string(scanner.next_token());
	cout << endl;

	parsetree.push(NT_K);

	switch( scanner.next_token() ) {
		case T_eof:
			parsetree.drawepsilon();
			eat_token(T_eof);
			break;

		case T_openparen:	// E '.' L
		case T_num:
			E();
			eat_token(T_period);
			List();
			break;

		default:
			syntax_error(NT_K);
			break;
	}

	parsetree.pop();
}
*/
void parser_t::E() {
	/*
	cout << "E() ";
	cout << scanner.s_index;
	cout << scanner.index;
	cout << token_to_string(scanner.next_token());
	cout << endl;
*/

	parsetree.push(NT_Expr);
	switch( scanner.next_token() ) {
		case T_openparen:	// T Ee
		case T_num:
			T();
			Ee();
			break;

		default:
			syntax_error(NT_Expr);
			break;
	}

	parsetree.pop();
}

void parser_t::Ee() {
	/*
	cout << "Ee() ";
	cout << scanner.s_index;
	cout << scanner.index;
	cout << token_to_string(scanner.next_token());
	cout << endl;
	*/

	parsetree.push(NT_Expr_Prime);
	switch( scanner.next_token() ) {
		case T_minus:	// '-' T Ee
			eat_token(T_minus);
			T();
			Ee();
			break;

		case T_plus:	// '+' T Ee
			eat_token(T_plus);
			T();
			Ee();
			break;

		case T_period:
		case T_closeparen:
		case T_eof:
			break;

		default:
			syntax_error(NT_Expr_Prime);
			break;
	}

	parsetree.pop();
}

void parser_t::T() {
	/*
	cout << "T() ";
	cout << scanner.s_index;
	cout << scanner.index;
	cout << token_to_string(scanner.next_token());
	cout << endl;
	*/

	parsetree.push(NT_Term);
	switch( scanner.next_token() ) {
		case T_openparen:	// F Tt
		case T_num:
			F();
			Tt();
			break;

		default:
			syntax_error(NT_Term);
			break;
	}

	parsetree.pop();
}

void parser_t::Tt() {
	/*
	cout << "Tt() ";
	cout << scanner.s_index;
	cout << scanner.index;
	cout << token_to_string(scanner.next_token());
	cout << endl;
	*/

	parsetree.push(NT_Term_Prime);
	switch( scanner.next_token() ) {
		case T_times:		// '*'	F Tt
			eat_token(T_times);
			F();
			Tt();
			break;

		case T_modulo:		// '%' F Tt
			eat_token(T_modulo);
			F();
			Tt();
			break;

		case T_minus:
		case T_plus:
		case T_period:
		case T_closeparen:
		case T_eof:
			break;

		default:
			syntax_error(NT_Term_Prime);
			break;
	}

	parsetree.pop();
}


void parser_t::F() {
	/*
	cout << "F() ";
	cout << scanner.s_index;
	cout << scanner.index;
	cout << token_to_string(scanner.next_token());
	cout << endl;
	*/

	parsetree.push(NT_Factor);
	switch( scanner.next_token() ) {
		case T_num:		// 'n'
			eat_token(T_num);
			break;

		case T_openparen:	// '(' E ')'
			eat_token(T_openparen);
			E();
			eat_token(T_closeparen);
			break;

		default:
			syntax_error(NT_Factor);
			break;
	}

	parsetree.pop();
}



/*** Main ***********************************************/

int main(int argc, char* argv[])
{
	parser_t parser;
	parser.parse();

	return 0;
}
