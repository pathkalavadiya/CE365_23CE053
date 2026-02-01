#include <iostream>
#include <fstream>
#include <cctype>
#include <set>
using namespace std;


string keywords[] = {
    "int","float","char","double","long","short","void","return",
    "if","else","while","for","do","break","continue","struct",
    "scanf","printf"
};

set<string>symbol;

bool iskeyword(string s){
    for(string k :keywords){
        if(k==s)
            return true;
    }
    return false;
}

bool isoperator(char ch){
    char operators[] = {'+','-','*','/','%','=','<','>','!','&','|','^','~'};
    for(char op : operators){
        if(ch==op)
            return true;
    }
    return false;
}

bool ispunctuation(char ch){
    char punctuations[] = {',',';','(',')','{','}','[',']'};
    for(char p : punctuations){
        if(ch==p)
            return true;
    }
    return false;
}

int main(){
    ifstream file("input.c");
    if(!file){
        cout<<"error in c file";
        return 0;
    }

 string line;
 int linenumber=0;
 bool comment=false;

cout<<"\n tokens \n";

while(getline(file,line)){
    linenumber++;
    int i=0;
    while(i<line.length())
    {
if (comment) {
                if (line[i] == '*' && i+1 < line.length() && line[i+1] == '/') {
                    comment = false;
                    i += 2;
                } else i++;
                continue;
            }


            if (line[i] == '/' && i+1 < line.length() && line[i+1] == '/') break;

            if (line[i] == '/' && i+1 < line.length() && line[i+1] == '*') {
                comment = true;
                i += 2;
                continue;
            }

          
            if (isspace(line[i])) {
                i++;
                continue;
            }

            if (isalpha(line[i]) || line[i] == '_') {
                string word;
                while (i < line.length() && (isalnum(line[i]) || line[i] == '_'))
                    word += line[i++];

                if (iskeyword(word))
                    cout << "Keyword: " << word << endl;
                else {
                    cout << "Identifier: " << word << endl;
                    symbol.insert(word);
                }
            }

            else if (isdigit(line[i])) {
                string num;
                bool invalid = false;

                while (i < line.length() && isalnum(line[i])) {
                    if (isalpha(line[i])) invalid = true;
                    num += line[i++];
                }

                if (invalid)
                    cout << "lexical error(Line " << linenumber << "): "
                         << num << " invalid lexeme\n";
                else
                    cout << "constant: " << num << endl;
            }

          
            else if (line[i] == '\'') {
                string ch;
                ch += line[i++];
                while (i < line.length() && line[i] != '\'')
                    ch += line[i++];
                ch += line[i++];
                cout << "constant: " << ch << endl;
            }

           
            else if (line[i] == '"') {
                string str;
                str += line[i++];
                while (i < line.length() && line[i] != '"')
                    str += line[i++];
                str += line[i++];
                cout << "constant: " << str << endl;
            }

            // Operator
            else if (isoperator(line[i])) {
                cout << "operator: " << line[i] << endl;
                i++;
            }

            // Punctuation
            else if (ispunctuation(line[i])) {
                cout << "punctuation: " << line[i] << endl;
                i++;
            }

            else {
                cout << "lexical error(Line " << linenumber << "): "
                     << line[i] << " invalid symbol\n";
                i++;
            }
        }
    }

    cout << "\n symbol table entries \n";
    int idx = 1;
    for (auto s : symbol)
        cout << idx++ << ") " << s << endl;

    file.close();
    return 0;
}
