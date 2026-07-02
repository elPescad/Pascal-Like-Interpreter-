using namespace std;

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <sstream>

#include "val.h"


// numeric overloaded add this to op
Value Value::operator+(const Value& op) const
{
    // num addition
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value(this->GetInt() + op.GetInt());
    }
    else if(op.IsReal() && this->IsReal())
    {
        return Value(this->GetReal() + op.GetReal());
    }
    else if(op.IsReal() && this->IsInt())
    {
        return Value(this->GetInt() + op.GetReal());
    }
    else if(op.IsInt() && this->IsReal())
    {
        return Value(this->GetReal() + op.GetInt());   
    }
    // string addition
    else if(op.IsString() && this->IsChar())
    {
        return Value(this->GetChar() + op.GetString());
    }
    else if(op.IsChar() && this->IsString())
    {
        return Value(this->GetString() + op.GetChar());
    }
    else if(op.IsChar() && this->IsChar())
    {
        string result = "";
        result += this->GetChar();
        result += op.GetChar();
        return Value(result);
    }
    else if(op.IsString() && this->IsString())
    {
        return Value(this->GetString() + op.GetString());
    }
    return Value();
}

// numeric overloaded subtract op from this
Value Value::operator-(const Value& op) const
{
    // num addition
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value(this->GetInt() - op.GetInt());
    }
    else if(op.IsReal() && this->IsReal())
    {
        return Value(this->GetReal() - op.GetReal());
    }
    else if(op.IsReal() && this->IsInt())
    {
        return Value(this->GetInt() - op.GetReal());
    }
    else if(op.IsInt() && this->IsReal())
    {
        return Value(this->GetReal() - op.GetInt());   
    }
    return Value();
}

// numeric overloaded multiply this by op
Value Value::operator*(const Value& op) const
{
    // num addition
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value(this->GetInt() * op.GetInt());
    }
    else if(op.IsReal() && this->IsReal())
    {
        return Value(this->GetReal() * op.GetReal());
    }
    else if(op.IsReal() && this->IsInt())
    {
        return Value((double)this->GetInt() * op.GetReal());
    }
    else if(op.IsInt() && this->IsReal())
    {
        return Value(this->GetReal() * (double)op.GetInt());   
    }
    return Value();
}

// numeric overloaded divide this by oper
Value Value::operator/(const Value& op) const
{
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if((op.IsInt() && op.GetInt() == 0) || (op.IsReal() && op.GetReal() == 0.0))
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value((double)this->GetInt() / op.GetInt());
    }
    else if(op.IsReal() && this->IsReal())
    {
        return Value(this->GetReal() / op.GetReal());
    }
    else if(op.IsReal() && this->IsInt())
    {
        return Value((double)this->GetInt() / op.GetReal());
    }
    else if(op.IsInt() && this->IsReal())
    {
        return Value(this->GetReal() / op.GetInt());   
    }
    return Value();
}

//numeric integer division this by oper
Value Value::idiv(const Value& op) const
{
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if((op.IsInt() && op.GetInt() == 0))
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value(this->GetInt() / op.GetInt());
    }
    return Value();
}

//numeric MOD: overloaded Remainder this by op
Value Value::operator%(const Value & op) const
{
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if(op.IsInt() && op.GetInt() == 0)
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value(this->GetInt() % op.GetInt());
    }
    return Value();
}
        
//(Relational = (Equal): overloaded equality operator of this with op
Value Value::operator==(const Value& op) const
{
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value(this->GetInt() == op.GetInt());
    }
    else if(op.IsReal() && this->IsReal())
    {
        return Value(this->GetReal() == op.GetReal());
    }
    else if(op.IsReal() && this->IsInt())
    {
        return Value((double)this->GetInt() == op.GetReal());
    }
    else if(op.IsInt() && this->IsReal())
    {
        return Value(this->GetReal() == (double)op.GetInt());   
    }
    // string check
    else if(op.IsString() && this->IsChar())
    {
        return Value();
    }
    else if(op.IsChar() && this->IsString())
    {
        return Value();
    }
    else if(op.IsChar() && this->IsChar())
    {
        return Value(this->GetChar() == op.GetChar());
    }
    else if(op.IsString() && this->IsString())
    {
        return Value(this->GetString() == op.GetString());
    }
    // bool check
    else if(op.IsBool() && this->IsBool())
    {
        return Value(this->GetBool() == op.GetBool());
    }
    return Value();
}
    
//overloaded greater than operator of this with op
Value Value::operator>(const Value& op) const
{
    // num compare
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value(this->GetInt() > op.GetInt());
    }
    else if(op.IsReal() && this->IsReal())
    {
        return Value(this->GetReal() > op.GetReal());
    }
    else if(op.IsReal() && this->IsInt())
    {
        return Value(this->GetInt() > op.GetReal());
    }
    else if(op.IsInt() && this->IsReal())
    {
        return Value(this->GetReal() > op.GetInt());   
    }
    // string compare
    else if(op.IsString() && this->IsChar())
    {
        return Value();
    }
    else if(op.IsChar() && this->IsString())
    {
        return Value();
    }
    else if(op.IsChar() && this->IsChar())
    {
        return Value(this->GetChar() > op.GetChar());
    }
    else if(op.IsString() && this->IsString())
    {
        return Value(this->GetString() > op.GetString());
    }
    // bool check
    else if(op.IsBool() && this->IsBool())
    {
        return Value(this->GetBool() > op.GetBool());
    }
    return Value();
}


//overloaded less than operator of this with op
Value Value::operator<(const Value& op) const
{
    // num compare
    if(op.IsErr() || this->IsErr())
    {
        return Value();
    }
    else if(op.IsInt() && this->IsInt())
    {
        return Value(this->GetInt() < op.GetInt());
    }
    else if(op.IsReal() && this->IsReal())
    {
        return Value(this->GetReal() < op.GetReal());
    }
    else if(op.IsReal() && this->IsInt())
    {
        return Value(this->GetInt() < op.GetReal());
    }
    else if(op.IsInt() && this->IsReal())
    {
        return Value(this->GetReal() < op.GetInt());   
    }
    // string compare
    else if(op.IsString() && this->IsChar())
    {
        return Value();
    }
    else if(op.IsChar() && this->IsString())
    {
        return Value();
    }
    else if(op.IsChar() && this->IsChar())
    {
        return Value(this->GetChar() < op.GetChar());
    }
    else if(op.IsString() && this->IsString())
    {
        return Value(this->GetString() < op.GetString());
    }
    // bool check
    else if(op.IsBool() && this->IsBool())
    {
        return Value(this->GetBool() < op.GetBool());
    }
    return Value();
}

//Logical and: overloaded logical Anding operator of this with op
Value Value::operator&&(const Value& op) const
{
    if((this->IsBool() && op.IsBool()) && (this->GetBool() == true && op.GetBool() == true))
    {
        return Value(true);
    }
    else if(this->IsBool() && op.IsBool())
    {
        return Value(false);
    }
    return Value();
}

//Logical or: overloaded logical Oring operator of this with op
Value Value::operator||(const Value& op) const
{
    if((this->IsBool() && op.IsBool()) && (this->GetBool() == true || op.GetBool() == true))
    {
        return Value(true);
    }
    else if(this->IsBool() && op.IsBool())
    {
        return Value(false);
    }
    return Value();
}

//Logical not: overloaded logical Complement operator of this object
Value Value::operator!(void) const
{
    if(this->IsBool())
    {
        return Value(!this->GetBool());
    }
    return Value();
}

//unary sign operator for negative sign to be applied on value of this object
Value Value::operator-(void) const
{
    if(this->IsInt())
    {
        return Value(-1 * this->GetInt());
    }
    else if(this->IsReal())
    {
        return Value(-1.0 * this->GetReal());
    }
    else if(this->IsString() || this->IsChar())
    {
        return this->Trim();
    }
    return Value();
}

//Trimming operation: Removes trailing blanks of a string or char
Value Value::Trim(void) const
{
    if(this->IsChar())
    {
        if(this->GetChar() == ' ')
        {
            return Value("");
        }
        return Value(this->GetChar());
    }
    else if(this->IsString())
    {
        string temp = this->GetString();
        int last = temp.length() - 1;

        while(last >= 0 && temp[last] == ' ')
        {
            last--;
        }

        return Value(temp.substr(0, last + 1));
    }
    return Value();
}