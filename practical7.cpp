#include <iostream>
#include <map>
#include <set>
#include <vector>
using namespace std;

map<char, vector<string>> grammar;
map<char, set<char>> firstSet;
map<char, set<char>> followSet;

set<char> nonTerminals = {'S','A','B','C','D'};
char startSymbol = 'S';

set<char> findFirst(char);

set<char> findFirstString(string str)
{
    set<char> result;

    for(int i=0;i<str.length();i++)
    {
        char c = str[i];

        if(nonTerminals.find(c)==nonTerminals.end())
        {
            result.insert(c);
            return result;
        }

        set<char> temp = findFirst(c);

        for(char t : temp)
            if(t!='#') result.insert(t);

        if(temp.find('#')==temp.end())
            return result;

        if(i==str.length()-1)
            result.insert('#');
    }

    return result;
}

set<char> findFirst(char X)
{
    if(firstSet.count(X)) return firstSet[X];

    set<char> result;

    for(string prod : grammar[X])
    {
        if(prod=="#")
        {
            result.insert('#');
            continue;
        }

        for(int i=0;i<prod.length();i++)
        {
            char c = prod[i];

            if(nonTerminals.find(c)==nonTerminals.end())
            {
                result.insert(c);
                break;
            }

            set<char> temp = findFirst(c);

            for(char t : temp)
                if(t!='#') result.insert(t);

            if(temp.find('#')==temp.end())
                break;

            if(i==prod.length()-1)
                result.insert('#');
        }
    }

    firstSet[X] = result;
    return result;
}

void computeFollow()
{
    followSet[startSymbol].insert('$');

    bool changed = true;

    while(changed)
    {
        changed = false;

        for(auto rule : grammar)
        {
            char A = rule.first;

            for(string prod : rule.second)
            {
                for(int i=0;i<prod.length();i++)
                {
                    char B = prod[i];

                    if(nonTerminals.find(B)==nonTerminals.end())
                        continue;

                    string beta = prod.substr(i+1);

                    set<char> firstBeta = findFirstString(beta);

                    int before = followSet[B].size();

                    for(char c : firstBeta)
                        if(c!='#') followSet[B].insert(c);

                    if(firstBeta.count('#') || beta=="")
                    {
                        for(char c : followSet[A])
                            followSet[B].insert(c);
                    }

                    if(followSet[B].size() > before)
                        changed = true;
                }
            }
        }
    }
}

int main()
{
    // Grammar definition
    grammar['S'] = {"ABC","D"};
    grammar['A'] = {"a","#"};
    grammar['B'] = {"b","#"};
    grammar['C'] = {"(S)","c"};
    grammar['D'] = {"AC"};

    // FIRST calculation
    for(char nt : nonTerminals)
        findFirst(nt);

    // FOLLOW calculation
    computeFollow();

    cout<<"FIRST Sets\n";
    for(auto p : firstSet)
    {
        cout<<"First("<<p.first<<") = { ";
        for(char c : p.second)
        {
            if(c=='#') cout<<"ε ";
            else cout<<c<<" ";
        }
        cout<<"}\n";
    }

    cout<<"\nFOLLOW Sets\n";
    for(auto p : followSet)
    {
        cout<<"Follow("<<p.first<<") = { ";
        for(char c : p.second)
            cout<<c<<" ";
        cout<<"}\n";
    }
}