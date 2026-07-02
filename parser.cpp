#include "parser.h"
#include <vector>
#include <string>
#include <iomanip>

map<string, Value> defVar;
map<string, bool> defConst;
map<string, ValType> declaredType;

void ParseError(int line, string msg);
namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		LexItem tok = getNextToken(in, line);


        if(tok.GetToken() == ERR) 
        {
            ParseError(line, "Unrecognized Input Pattern.\n(" + tok.GetLexeme() + ")");
        }

        return tok;
	}


	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

/* -----------------------------------------------------------
 *                          Prog START
 * -----------------------------------------------------------
 */
bool Prog(istream& in, int& line)
{
    LexItem tok;

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != PROGRAM)
    {
        ParseError(line, "Missing Program KW");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != IDENT)
    {
        ParseError(line, "Missing ident after Program KW");
        return false;
    }
    string name = tok.GetLexeme();

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != SEMICOL)
    {
        ParseError(line, "Missing SemiCol at end");
        return false;
    }

    if(Block(in, line) == false)
    {
        ParseError(line, "Incorrect Program Body.");
        return false;
    }

    if(Parser::GetNextToken(in, line) != DOT)
    {
        ParseError(line, "Missing DOT at the end");
        return false;
    }

    cout << endl;
    cout << endl;
    cout << "DONE" << endl;

    return true;
}
/* -----------------------------------------------------------
 *                          Prog END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                          Block Start
 * -----------------------------------------------------------
 */
bool Block(istream& in, int& line)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() == VAR || tok.GetToken() == CONST)
    {
        Parser::PushBackToken(tok);
        if(DeclPart(in, line) == false)
        {
            return false;
        }
    }
    else
    {
        Parser::PushBackToken(tok);
    }

    if(CompStmt(in, line, true) == false)
    {
        ParseError(line, "Incorrect Program Body.");
        return false;
    }
    return true;
}
/* -----------------------------------------------------------
 *                          Block END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                       DeclPart START
 * -----------------------------------------------------------
 */
bool DeclPart(istream& in, int& line)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() == CONST)
    {
        Parser::PushBackToken(tok);
        if(ConstPart(in, line) == false)
        {
            ParseError(line, "Incorrect Constant Definition Part.");
            return false;
        }
        tok = Parser::GetNextToken(in, line);
    }

    if(tok.GetToken() == VAR)
    {
        Parser::PushBackToken(tok);
        if(VarPart(in, line) == false)
        {
            ParseError(line, "Incorrect Declaration Part.");
            return false;
        }
    }
    else
    {
        Parser::PushBackToken(tok);
    }

    return true;
}
/* -----------------------------------------------------------
 *                       DeclPart END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                       ConstPart Start
 * -----------------------------------------------------------
 */
bool ConstPart(istream& in, int& line)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() == CONST)
    {
        if(ConstDef(in, line) == false)
        {
            ParseError(line, "Syntactic error in Constants Definitions Part.");
            return false;
        }
    }
    else
    {
        ParseError(line, "Missing Const Part");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    while(tok.GetToken() == SEMICOL)
    {
        
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == IDENT)
        {
            Parser::PushBackToken(tok);
            if(ConstDef(in, line) == false)
            {
                ParseError(line, "Syntactic error in Constants Definitions Part.");
                return false;
            }
            tok = Parser::GetNextToken(in, line);
        }
        else
        {
            Parser::PushBackToken(tok);
            return true;
        }
    }

    ParseError(line, "Missing SemiCol");
    return false;
}
/* -----------------------------------------------------------
 *                       ConstPart END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                       ConstDef START
 * -----------------------------------------------------------
 */
bool ConstDef(istream& in, int& line)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != IDENT)
    {
        ParseError(line, "Missing IDENT");
        return false;
    }

    string name = tok.GetLexeme();
    auto it = defConst.find(name);

    if(it != defConst.end())
    {
        ParseError(line, "Constant Redefinition: " + name);
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != EQ)
    {
        ParseError(line, "Missing '='");
        return false;
    }
    
    Value constVal;
    if(Expr(in, line, constVal, true) == false)
    {
        return false;
    }

    defVar[name] = constVal;
    defConst[name] = true;

    return true;
}
/* -----------------------------------------------------------
 *                       ConstDef END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                       VarPart START
 * -----------------------------------------------------------
 */
bool VarPart(istream& in, int& line)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() == VAR)
    {
        if(DeclStmt(in, line) == false)
        {
            ParseError(line, "Syntactic error in Declaration Block.");
            return false;
        }
    }
    else
    {
        ParseError(line, "Missing Var Part");
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    while(tok.GetToken() == SEMICOL)
    {
        
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == IDENT)
        {
            Parser::PushBackToken(tok);
            if(DeclStmt(in, line) == false)
            {
                ParseError(line, "Syntactic error in Declaration Block.");
                return false;
            }
            tok = Parser::GetNextToken(in, line);
        }
        else
        {
            Parser::PushBackToken(tok);
            return true;
        }
    }

    ParseError(line, "Missing SemiCol");
    return false;
}
/* -----------------------------------------------------------
 *                       VarPart END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                       DeclStmt START
 * -----------------------------------------------------------
 */
bool DeclStmt(istream& in, int& line)
{
    vector<string> newVars;
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != IDENT)
    {
        ParseError(line, "Missing IDENT");
        return false;
    }

    Parser::PushBackToken(tok);

    if(IdentList(in, line, newVars) == false)
    {
        ParseError(line, "Incorrect identifiers list in Declaration Statement.");
        return false;
    }
    
    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != COLON)
    {
        ParseError(line, "Missing Colon");
        return false;
    }
    
    tok = Parser::GetNextToken(in, line);
    Token getTok = tok.GetToken();
    if(tok.GetToken() != INTEGER && tok.GetToken() != REAL && tok.GetToken() != BOOLEAN && tok.GetToken() != CHAR && tok.GetToken() != STRING)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Incorrect Declaration Type: " + tok.GetLexeme());
        return false;
    }

    Parser::PushBackToken(tok);

    if(Type(in, line) == false)
    {
        return false;
    }

    for(string varName : newVars)
    {
        defVar[varName] = Value(); 
        if(getTok == INTEGER)      
        {
            declaredType[varName] = VINT;
        }
        else if(getTok == REAL)    
        {
            declaredType[varName] = VREAL;
        }
        else if(getTok == STRING)  
        {
            declaredType[varName] = VSTRING;
        }
        else if(getTok == BOOLEAN) 
        {
            declaredType[varName] = VBOOL;
        }
        else if(getTok == CHAR)    
        {
            declaredType[varName] = VCHAR;
        }
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ASSOP)
    {
        Value val;
        if(Expr(in, line, val, true) == false)
        {
            ParseError(line, "Incomplete declaration");
            return false;
        }

        for(string varName : newVars) 
        {
            if (getTok == REAL && val.GetType() == VINT) 
            {
                defVar[varName] = Value((float)val.GetInt()); 
            }
            else if (getTok == INTEGER && val.GetType() == VREAL) 
            {
                ParseError(line, "Illegal expression type for the assigned variable.");
                return false;
            }
            else 
            {
                defVar[varName] = val;
            }
        }
    }
    else
    {
        Parser::PushBackToken(tok);
    }
    
    return true;
}
/* -----------------------------------------------------------
 *                       DeclStmt END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                        Type START
 * -----------------------------------------------------------
 */
bool Type(istream& in, int& line)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != INTEGER && tok.GetToken() != REAL && tok.GetToken() != BOOLEAN && tok.GetToken() != CHAR && tok.GetToken() != STRING)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Incorrect Declaration Type: " + tok.GetLexeme());
        return false;
    }
    return true;
}
/* -----------------------------------------------------------
 *                        Type END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                        Stmt START
 * -----------------------------------------------------------
 */
bool Stmt(istream& in, int& line, bool condition)
{
    string s;
    Value val;
    LexItem tok = Parser::GetNextToken(in, line);
    bool atLeastOneS = false;
    if(tok.GetToken() == ELSE)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Illegal Else-clause.");
        return false;
    }

    if(tok.GetToken() == WRITE || tok.GetToken() == WRITELN || tok.GetToken() == IDENT || tok.GetToken() == READLN)
    {
        if(tok.GetToken() == IDENT)
        {
            s = tok.GetLexeme();
        }
        Parser::PushBackToken(tok);
        if(SimpleStmt(in, line, condition) == false)
        {
            return false;
        }
        atLeastOneS = true;
    }
    else if(tok.GetToken() == IF || tok.GetToken() == BEGIN)
    {
        Parser::PushBackToken(tok);
        if(StructuredStmt(in, line, condition) == false)
        {
            ParseError(line, "Syntactic error in the statement.");
            return false;
        }
        atLeastOneS = true;
    }

    if(!atLeastOneS)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Not a stmt");
        return false;
    }
    return true;
}
/* -----------------------------------------------------------
 *                         Stmt END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                       SimpleStmt START
 * -----------------------------------------------------------
 */
bool SimpleStmt(istream& in, int& line, bool condition)
{
    LexItem tok = Parser::GetNextToken(in, line);
    bool atLeastOneS = false;

    if(tok.GetToken() == IDENT)
    {
        Parser::PushBackToken(tok);
        if(AssignStmt(in, line, condition) == false)
        {
            return false;
        }
        atLeastOneS = true;
    }
    else if(tok.GetToken() == READLN)
    {
        Parser::PushBackToken(tok);
        if(ReadLnStmt(in, line, condition) == false)
        {
            return false;
        }
        atLeastOneS = true;
    }
    else if(tok.GetToken() == WRITELN)
    {
        Parser::PushBackToken(tok);
        if(WriteLnStmt(in, line, condition) == false)
        {
            ParseError(line, "Incorrect Simple Statement.");
            ParseError(line, "Syntactic error in the statement.");
            return false;
        }
        atLeastOneS = true;
    }
    else if(tok.GetToken() == WRITE)
    {
        Parser::PushBackToken(tok);
        if(WriteStmt(in, line, condition) == false)
        {
            return false;
        }
        atLeastOneS = true;
    }

    if(!atLeastOneS)
    {
        ParseError(line, "Not simple statement");
        return false;
    }
    return true;
}
/* -----------------------------------------------------------
 *                       SimpleStmt END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                    StructuredStmt START
 * -----------------------------------------------------------
 */
bool StructuredStmt(istream& in, int& line, bool condition)
{
    LexItem tok = Parser::GetNextToken(in, line);
    bool atLeastOneS = false;

    if(tok.GetToken() == IF)
    {
        Parser::PushBackToken(tok);
        if(IfStmt(in, line, condition) == false)
        {
            ParseError(line, "Incorrect Structured Statement.");
            return false;
        }
        atLeastOneS = true;
    }
    else if(tok.GetToken() == BEGIN)
    {
        Parser::PushBackToken(tok);
        if(CompStmt(in, line, condition) == false)
        {
            return false;
        }
        atLeastOneS = true;
    }

    if(!atLeastOneS)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Not a Structured statement");
        return false;
    }
    return true;
}
/* -----------------------------------------------------------
 *                    StructuredStmt END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                       CompStmt START
 * -----------------------------------------------------------
 */
bool CompStmt(istream& in, int& line, bool condition)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != BEGIN)
    {
        ParseError(line, "Missing Begin");
        return false;
    }
    
    int expectedSemi = line;
    if(Stmt(in, line, condition) == false)
    {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != SEMICOL)
    {
        Parser::PushBackToken(tok);
        ParseError(expectedSemi, "Missing semicolon.");
        return false;
    }
    while(tok.GetToken() == SEMICOL)
    {
        LexItem peek = Parser::GetNextToken(in, line);
        if(peek.GetToken() == END || peek.GetToken() == DONE)
        {
            tok = peek;
            break;
        }

        expectedSemi = line;
        Parser::PushBackToken(peek);
        if(Stmt(in, line, condition) == false)
        {
            return false;
        }
        tok = Parser::GetNextToken(in, line);
    }
    
    if(tok.GetToken() == DONE)
    {
        ParseError(line, "Missing end of compound statement.");
        return false;
    }
    else if(tok.GetToken() == END)
    {
        return true;
    }
    Parser::PushBackToken(tok);
    ParseError(expectedSemi, "Missing semicolon.");
    return false;
}
/* -----------------------------------------------------------
 *                       CompStmt END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                     WriteLnStmt START
 * -----------------------------------------------------------
 */
bool WriteLnStmt(istream& in, int& line, bool execute)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != WRITELN)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing Write");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != LPAREN)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing '('");
        return false;
    }

    if(ExprList(in, line, execute) == false)
    {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != RPAREN)
    {
        ParseError(line, "Missing ')'");
        return false;
    }

    if(execute)
    {
        cout << endl;
    }

    return true;
}
/* -----------------------------------------------------------
 *                       WriteLnStmt END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                       WriteStmt START
 * -----------------------------------------------------------
 */
bool WriteStmt(istream& in, int& line, bool execute)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != WRITE)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing Write");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != LPAREN)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing '('");
        return false;
    }

    if(ExprList(in, line, execute) == false)
    {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != RPAREN)
    {
        ParseError(line, "Missing ')'");
        return false;
    }
    return true;
}
/* -----------------------------------------------------------
 *                       WriteStmt END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                         IFStmt START
 * -----------------------------------------------------------
 */
bool IfStmt(istream& in, int& line, bool condition)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != IF)
    {
        ParseError(line, "Missing IF");
        return false;
    }

    Value val;
    if(Expr(in, line, val, condition) == false)
    {
        ParseError(line, "Missing if statement Logic Expression.");
        return false;
    }

    bool runThen = false;
    bool runElse = false;
    if(condition)
    {    
        if(val.IsBool() == false)
        {
            return false;
        }

        if(val.GetBool() == true)
        {
            runThen = true;
        }
        else
        {
            runElse = true;
        }
    }

    tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != THEN)
    {
        ParseError(line, "If Statement Syntax Error.");
        return false;
    }

    if(Stmt(in, line, runThen) == false)
    {
        ParseError(line, "Missing Statement for If-Then-Part.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != ELSE)
    {
        Parser::PushBackToken(tok);
        return true;
    }
    else
    {
        if(Stmt(in, line, runElse) == false)
        {
            return false;
        }
        return true;
    }
}
/* -----------------------------------------------------------
 *                         IFStmt END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                       AssignStmt START
 * -----------------------------------------------------------
 */
bool AssignStmt(istream& in, int& line, bool condition)
{
    Value val;
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != IDENT)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement.");
        return false;
    }

    string s = tok.GetLexeme();
    Parser::PushBackToken(tok);

    if(Variable(in, line) == false)
    {
        ParseError(line, "Incorrect Simple Statement.");
        ParseError(line, "Syntactic error in the statement.");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != ASSOP)
    {
        ParseError(line, "Missing assignment operator");
        ParseError(line, "Incorrect Simple Statement.");
        return false;
    }

    if(Expr(in, line, val, condition) == false)
    {
        ParseError(line, "Missing Expression in Assignment Statement.");
        ParseError(line, "Incorrect Simple Statement.");
        return false;
    }

    if(condition)
    {
        if(defConst.find(s) != defConst.end())
        {
            ParseError(line, "Cannot assign to a constant.");
            return false;
        }

        ValType lhs = (declaredType.count(s)) ? declaredType[s] : defVar[s].GetType();
        ValType rhs = val.GetType();

        if(lhs == VREAL && rhs == VINT)
        {
            val = Value((double)val.GetInt());
        }
        else if(lhs != rhs)
        {
            ParseError(line, "Illegal expression type for the assigned variable.");
            return false;
        }
        defVar[s] = val;
    }
    return true;
}
/* -----------------------------------------------------------
 *                       AssignStmt END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                      Variable START
 * -----------------------------------------------------------
 */
bool Variable(istream& in, int& line)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != IDENT)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement.");
        return false;
    }

    if(defVar.find(tok.GetLexeme()) == defVar.end())
    {
        ParseError(line, "Undeclared Variable: " + tok.GetLexeme());
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement.");
        return false;
    }

    return true;
}
/* -----------------------------------------------------------
 *                       Variable END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                       ReadLnStmt START
 * -----------------------------------------------------------
 */
bool ReadLnStmt(istream& in, int& line, bool execute)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != READLN)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing READLN");
        return false;
    }

    tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != LPAREN)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing '('");
        return false;
    }

    if(VarList(in, line, execute) == false)
    {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() != RPAREN)
    {
        ParseError(line, "Missing ')'");
        return false;
    }
    return true;
}
/* -----------------------------------------------------------
 *                       ReadLnStmt END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                       ExprList START
 * -----------------------------------------------------------
 */
bool ExprList(istream& in, int& line, bool execute)
{
    Value val;
    LexItem tok;
    if(Expr(in, line, val, execute) == false)
    {
        return false;
    }

    if(execute)
    {
        cout << val;
    }

    tok = Parser::GetNextToken(in, line);
    while(tok.GetToken() == COMMA)
    {
        if(Expr(in, line, val, execute) == false)
        {
            return false;
        }

        if(execute)
        {
            cout << val; 
        }
        tok = Parser::GetNextToken(in, line);
    }
    Parser::PushBackToken(tok);

    return true;
}
/* -----------------------------------------------------------
 *                       ExprList END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                        VarList START
 * -----------------------------------------------------------
 */
bool VarList(istream& in, int& line, bool execute)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != IDENT)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement.");
        return false;
    }
    string s = tok.GetLexeme();
    Parser::PushBackToken(tok);

    if(Variable(in, line) == false)
    {
        return false;
    }

    if(execute)
    {
        if(defVar.find(s) == defVar.end()) {
            ParseError(line, "Undeclared variable in READLN");
            return false;
        }

        ValType type = (declaredType.count(s)) ? declaredType[s] : defVar[s].GetType();
        
        if(type == VINT) {
            int input;
            cin >> input;
            defVar[s] = Value(input);
        } 
        else if(type == VREAL) {
            double input;
            cin >> input;
            defVar[s] = Value(input);
        } 
        else if(type == VSTRING) {
            string input;
            getline(cin >> ws, input);
            defVar[s] = Value(input);
        }
        else if(type == VBOOL) 
        {
            string input;
            cin >> input;

            string cleanInput = "";
            for(char c : input) 
            {
                if(isalpha(c)) 
                {
                    cleanInput += tolower(c);
                }
            }
            if(cleanInput == "true") 
            {
                defVar[s] = Value(true);
            }
            else if(cleanInput == "false")
            {
                defVar[s] = Value(false);
            }
            else 
            {
                cout << "DEBUG: Line 20 boolean grabbed: [" << input << "]" << endl;
                ParseError(line, "Invalid boolean input.");
                return false;
            }
        }
        else if(type == VCHAR) {
            char input;
            cin >> input;
            defVar[s] = Value(input);
        }
    }

    tok = Parser:: GetNextToken(in, line);
    while(tok.GetToken() == COMMA)
    {
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() != IDENT)
        {
            ParseError(line, "Missing variable after comma");
            return false;
        }
        
        s = tok.GetLexeme();
        Parser::PushBackToken(tok);
        if(Variable(in, line) == false)
        {
            return false;
        }

        if(execute)
        {
            if(defVar.find(s) == defVar.end()) {
                ParseError(line, "Undeclared variable in READLN");
                return false;
            }

            ValType type = (declaredType.count(s)) ? declaredType[s] : defVar[s].GetType();
            
            if(type == VINT) {
                int input;
                cin >> input;
                defVar[s] = Value(input);
            } 
            else if(type == VREAL) {
                double input;
                cin >> input;
                defVar[s] = Value(input);
            } 
            else if(type == VSTRING) {
                string input;
                getline(cin >> ws, input);
                defVar[s] = Value(input);
            }
            else if(type == VBOOL) 
            {
                string input;
                cin >> input;

                string cleanInput = "";
                for(char c : input) 
                {
                    if(isalpha(c)) 
                    {
                        cleanInput += tolower(c);
                    }
                }
                if(cleanInput == "true") 
                {
                    defVar[s] = Value(true);
                }
                else if(cleanInput == "false")
                {
                    defVar[s] = Value(false);
                }
                else 
                {
                    cout << "DEBUG: Line 20 boolean grabbed: [" << input << "]" << endl;
                    ParseError(line, "Invalid boolean input.");
                    return false;
                }
            }
            else if(type == VCHAR) {
                char input;
                cin >> input;
                defVar[s] = Value(input);
            }
        }
        tok = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(tok);
    return true;
}
/* -----------------------------------------------------------
 *                        VarList END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                        Expr START
 * -----------------------------------------------------------
 */
bool Expr(istream& in, int& line, Value& val, bool condition)
{
    Value val1, val2;
    LexItem tok;
    if(SimpleExpr(in, line, val, condition) == false)
    {
        return false;
    }
    val1 = val;

    tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() == ERR) 
    {
        return false;
    }

    if(tok.GetToken() == EQ || tok.GetToken() == GTHAN || tok.GetToken() == LTHAN)
    {
        if(tok.GetToken() == EQ)
        {
            if(SimpleExpr(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = (val1 == val2);
        }
        else if(tok.GetToken() == GTHAN)
        {
            if(SimpleExpr(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = (val1 > val2);
        }
        else if(tok.GetToken() == LTHAN)
        {
            if(SimpleExpr(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = (val1 < val2);
        }

        if(val.IsErr())
        {
            return false;
        }
        LexItem nextTok = Parser::GetNextToken(in, line);
        if(nextTok.GetToken() == EQ || nextTok.GetToken() == GTHAN || nextTok.GetToken() == LTHAN)
        {
            ParseError(line, "Illegal Relational Expression.");
            return false;
        }
        Parser::PushBackToken(nextTok);

        return true;
    }
    else
    {
        Parser::PushBackToken(tok);
        return true;
    }
}
/* -----------------------------------------------------------
 *                        Expr END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                        SimpleExpr START
 * -----------------------------------------------------------
 */
bool SimpleExpr(istream& in, int& line, Value& val, bool condition)
{
    Value val1, val2;
    if(Term(in, line, val, condition) == false)
    {
        return false;
    }
    val1 = val;

    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() == ERR) 
    {
        return false;
    }

    while(tok.GetToken() == PLUS || tok.GetToken() == MINUS || tok.GetToken() == OR)
    {
        if(tok.GetToken() == PLUS)
        {    
            if(Term(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = val1 + val2;
        }
        else if(tok.GetToken() == MINUS)
        {    
            if(Term(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = val1 - val2;
        }
        else if(tok.GetToken() == OR)
        {    
            if(Term(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = (val1 || val2);
        }

        if(val.IsErr() && condition)
        {
            ParseError(line, "Illegal operand types or operation in SimpleExpr");
            return false;
        }

        val1 = val;
        tok = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(tok);
    return true;
}
/* -----------------------------------------------------------
 *                        SimpleExpr END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                        Term START
 * -----------------------------------------------------------
 */
bool Term(istream& in, int& line, Value& val, bool condition)
{
    Value val1, val2;
    if(SFactor(in, line, val, condition) == false)
    {
        return false;
    }
    val1 = val;

    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() == ERR) 
    {
        return false;
    }

    while(tok.GetToken() == MULT || tok.GetToken() == DIV || tok.GetToken() == IDIV || tok.GetToken() == MOD || tok.GetToken() == AND)
    {
        if(tok.GetToken() == MULT)
        {
            if(SFactor(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = val1 * val2;
        }
        else if(tok.GetToken() == DIV)
        {
            if(SFactor(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = val1 / val2;
        }
        else if(tok.GetToken() == IDIV)
        {
            if(SFactor(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = val1.idiv(val2);
        }
        else if(tok.GetToken() == MOD)
        {
            if(SFactor(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = val1 % val2;
        }
        else if(tok.GetToken() == AND)
        {
            if(SFactor(in, line, val, condition) == false)
            {
                return false;
            }
            val2 = val;
            val = (val1 && val2);
        }

        if(val.IsErr() && condition)
        {
            ParseError(line, "Illegal operand types or operation in Term");
            return false;
        }
        val1 = val;
        tok = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(tok);
    return true;
}
/* -----------------------------------------------------------
 *                        Term END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                        SFactor START
 * -----------------------------------------------------------
 */
bool SFactor(istream& in, int& line, Value& val, bool condition)
{
    LexItem tok = Parser::GetNextToken(in, line);
    Token store = tok.GetToken();

    if(tok.GetToken() == MINUS || tok.GetToken() == PLUS || tok.GetToken() == NOT)
    {
        if((tok.GetToken() == PLUS || tok.GetToken() == NOT) && Factor(in, line, 1, val, condition) == false)
        {
            return false;
        }

        if(tok.GetToken() == NOT)
        {
            store = NOT;
        }


        if(tok.GetToken() == MINUS && Factor(in, line, -1, val, condition) == false)
        {
            return false;
        }

        if(store == NOT)
        {
            val = !val;
        }

        if(val.IsErr())
        {
            ParseError(line, "Illegal Operand Type for Sign/NOT Operator.");
            return false;
        }

        if(store == PLUS && val.GetType() != VINT && val.GetType() != VREAL)
        {
            ParseError(line, "Illegal Operand Type for Sign/NOT Operator.");
            return false;
        }

        return true;
    }

    Parser::PushBackToken(tok);
    if(Factor(in, line, 1, val, condition) == false)
    {
        return false;
    }
    return true;
}
/* -----------------------------------------------------------
 *                        SFactor END
 * -----------------------------------------------------------
 */

 /* -----------------------------------------------------------
 *                        Factor START
 * -----------------------------------------------------------
 */
bool Factor(istream& in, int& line, int sign, Value& val, bool condition)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() == ERR) 
    {
        return false;
    }

    if(tok.GetToken() == IDENT)
    {
        if(defVar.find(tok.GetLexeme()) != defVar.end())
        {

            if(defVar[tok.GetLexeme()].IsErr() && condition) 
            {
                ParseError(line, "Uninitialized Variable: " + tok.GetLexeme());
                return false;
            }

            if(sign == -1)
            {
                val = -defVar[tok.GetLexeme()];
            }
            else
            {
                val = defVar[tok.GetLexeme()];
            }
        }
        else if(defConst.find(tok.GetLexeme()) != defConst.end())
        {
            if(sign == -1)
            {
                val = -defConst[tok.GetLexeme()];
            }
            else
            {
                val = defConst[tok.GetLexeme()];
            }
        }
        else
        {
            ParseError(line, "Using undefined variable");
            return false;
        }
        return true;
    }

    if(tok.GetToken() == ICONST)
    {
        val = Value(sign * stoi(tok.GetLexeme()));
        return true;
    }
    else if(tok.GetToken() == RCONST)
    {
        val = Value((double)sign * stod(tok.GetLexeme()));
        return true;
    }
    else if(tok.GetToken() == SCONST)
    {
        if(sign == -1)
        {
            val = -Value(tok.GetLexeme());
        }
        else
        {
            val = Value(tok.GetLexeme());
        }
        return true;
    }
    else if(tok.GetToken() == BCONST)
    {
        bool b;
        istringstream(tok.GetLexeme()) >> boolalpha >> b;
        val = Value(b);
        return true;
    }
    else if(tok.GetToken() == CCONST)
    {
        string s = tok.GetLexeme();
        
        if(sign == -1)
        {
            val = -Value(s.at(0));
        }
        else
        {
            val = Value(s.at(0));
        }
        return true;
    }


    if(tok.GetToken() == LPAREN)
    {
        if(Expr(in, line, val, condition) == false)
        {
            ParseError(line, "Missing expression after Left Parenthesis.");
            return false;
        }

        tok = Parser::GetNextToken(in, line);
        
        if(tok.GetToken() == RPAREN)
        {
            if(sign == -1)
            {
                val = -val;
            }
            return true;
        }
        else
        {
            ParseError(line, "Missing right parenthesis after expression.");
            return false;
        }
    }
    
    ParseError(line, "Missing Factor component");
    return false;
}
/* -----------------------------------------------------------
 *                        Factor END
 * -----------------------------------------------------------
 */

/* -----------------------------------------------------------
 *                        IdentList START
 * -----------------------------------------------------------
 */
bool IdentList(istream& in, int& line, vector<string>& varNames)
{
    LexItem tok = Parser::GetNextToken(in, line);

    if(tok.GetToken() != IDENT)
    {
        Parser::PushBackToken(tok);
        ParseError(line, "Missing IDENT");
        return false;
    }

    string name = tok.GetLexeme();
    auto it = defVar.find(name);

    if(it != defVar.end())
    {
        ParseError(line, "Variable Redefinition: " + name);
        return false;
    }

    if(defConst.find(name) != defConst.end())
    {
        ParseError(line, "Illegal use of a constant name as a variable: " + name);
        return false;
    }

    varNames.push_back(name);

    tok = Parser::GetNextToken(in, line);
    while(tok.GetToken() == COMMA)
    {
        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() != IDENT)
        {
            ParseError(line, "extra ','");
            return false;
        }

        name = tok.GetLexeme();
        auto it = defVar.find(name);

        if(it != defVar.end())
        {
            ParseError(line, "Variable Redefinition: " + name);
            return false;
        }

        if(defConst.find(name) != defConst.end())
        {
            ParseError(line, "Illegal use of a constant name as a variable: " + name);
            return false;
        }

        varNames.push_back(name);
        tok = Parser::GetNextToken(in, line);
    }

    if(tok.GetToken() == IDENT)
    {
        ParseError(line, "Missing comma in declaration statement.");
        return false;
    }
    
    Parser::PushBackToken(tok);
    return true;
}
/* -----------------------------------------------------------
 *                        IdentList END
 * -----------------------------------------------------------
 */