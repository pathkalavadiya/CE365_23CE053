#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <stack>

using namespace std;

/* Grammar */
map<char, vector<string>> grammar;

/* FIRST & FOLLOW */
map<char, set<char>> FIRST;
map<char, set<char>> FOLLOW;

/* Parsing Table */
map<char, map<char,string>> table;

set<char> nonTerminals = {'S','A','B','C','Z'};
char startSymbol = 'S';

/* -------- FIRST -------- */

set<char> findFirst(char);

set<char> findFirstString(string str)
{
    set<char> result;

    for(int i=0;i<str.size();i++)
    {
        char c=str[i];

        if(nonTerminals.count(c)==0)
        {
            result.insert(c);
            return result;
        }

        set<char> temp=findFirst(c);

        for(char t:temp)
            if(t!='#')
                result.insert(t);

        if(temp.count('#')==0)
            return result;

        if(i==str.size()-1)
            result.insert('#');
    }

    return result;
}

set<char> findFirst(char X)
{
    if(FIRST.count(X))
        return FIRST[X];

    set<char> result;

    for(string prod:grammar[X])
    {
        if(prod=="#")
        {
            result.insert('#');
            continue;
        }

        for(int i=0;i<prod.size();i++)
        {
            char c=prod[i];

            if(nonTerminals.count(c)==0)
            {
                result.insert(c);
                break;
            }

            set<char> temp=findFirst(c);

            for(char t:temp)
                if(t!='#')
                    result.insert(t);

            if(temp.count('#')==0)
                break;

            if(i==prod.size()-1)
                result.insert('#');
        }
    }

    FIRST[X]=result;
    return result;
}

/* -------- FOLLOW -------- */

void computeFollow()
{
    FOLLOW[startSymbol].insert('$');

    bool changed=true;

    while(changed)
    {
        changed=false;

        for(auto rule:grammar)
        {
            char A=rule.first;

            for(string prod:rule.second)
            {
                for(int i=0;i<prod.size();i++)
                {
                    char B=prod[i];

                    if(nonTerminals.count(B)==0)
                        continue;

                    string beta=prod.substr(i+1);

                    set<char> firstBeta=findFirstString(beta);

                    int before=FOLLOW[B].size();

                    for(char c:firstBeta)
                        if(c!='#')
                            FOLLOW[B].insert(c);

                    if(firstBeta.count('#') || beta=="")
                        for(char c:FOLLOW[A])
                            FOLLOW[B].insert(c);

                    if(FOLLOW[B].size()>before)
                        changed=true;
                }
            }
        }
    }
}

/* -------- Parsing Table -------- */

bool buildParsingTable()
{
    bool LL1=true;

    for(auto rule:grammar)
    {
        char A=rule.first;

        for(string prod:rule.second)
        {
            set<char> firstAlpha=findFirstString(prod);

            for(char a:firstAlpha)
            {
                if(a!='#')
                {
                    if(table[A][a]!="")
                        LL1=false;

                    table[A][a]=prod;
                }
                else
                {
                    for(char b:FOLLOW[A])
                    {
                        if(table[A][b]!="")
                            LL1=false;

                        table[A][b]=prod;
                    }
                }
            }
        }
    }

    return LL1;
}

/* -------- Parsing -------- */

bool parse(string input)
{
    stack<char> st;

    st.push('$');
    st.push(startSymbol);

    input.push_back('$');

    int i=0;

    while(!st.empty())
    {
        char top=st.top();
        char cur=input[i];

        if(top==cur)
        {
            st.pop();
            i++;
        }
        else if(nonTerminals.count(top))
        {
            if(table[top][cur]=="")
                return false;

            st.pop();

            string prod=table[top][cur];

            if(prod!="#")
                for(int j=prod.size()-1;j>=0;j--)
                    st.push(prod[j]);
        }
        else
            return false;
    }

    return true;
}

/* -------- MAIN -------- */

int main()
{
    /* Correct LL(1) Grammar */
    grammar['S']={"AZC"};
    grammar['Z']={"B","#"};
    grammar['A']={"a","#"};
    grammar['B']={"b"};
    grammar['C']={"(S)","c"};

    /* FIRST */
    for(char nt:nonTerminals)
        findFirst(nt);

    /* FOLLOW */
    computeFollow();

    /* Print FIRST */
    cout<<"FIRST Sets\n";
for(auto p:FIRST)
{
    cout<<"First("<<p.first<<") = { ";
    for(char c:p.second)
    {
        if(c=='#')
            cout<<"null ";
        else
            cout<<c<<" ";
    }
    cout<<"}\n";
}

    /* Print FOLLOW */
    cout<<"\nFOLLOW Sets\n";
    for(auto p:FOLLOW)
    {
        cout<<"Follow("<<p.first<<") = { ";
        for(char c:p.second)
            cout<<c<<" ";
        cout<<"}\n";
    }

    /* Build Table */
    bool LL1=buildParsingTable();

    cout<<"\nPredictive Parsing Table\n";
    for(auto r:table)
        for(auto c:r.second)
            cout<<"M["<<r.first<<","<<c.first<<"] = "<<r.first<<" -> "<<c.second<<"\n";

    /* LL(1) Check */
    if(LL1)
        cout<<"\nGrammar is LL(1)\n";
    else
        cout<<"\nGrammar is NOT LL(1)\n";

    /* Continuous Input */
    string input;

    while(true)
    {
        cout<<"\nEnter string (type 'exit' to stop): ";
        cin>>input;

        if(input=="exit")
            break;

        if(parse(input))
            cout<<"Valid string\n";
        else
            cout<<"Invalid string\n";
    }
}