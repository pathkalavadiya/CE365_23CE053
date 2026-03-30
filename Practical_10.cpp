/*
 * Practical 10: Evaluating Arithmetic Expressions with Bottom-Up
 * Approach Using Syntax-Directed Definitions (SDD)
 *
 * Grammar (with semantic rules):
 *   L  -> E n         { Print(E.val) }
 *   E  -> E + T       { E.val = E.val + T.val }
 *   E  -> E - T       { E.val = E.val - T.val }
 *   E  -> T           { E.val = T.val }
 *   T  -> T * F       { T.val = T.val * F.val }
 *   T  -> T / F       { T.val = T.val / F.val }
 *   T  -> F           { T.val = F.val }
 *   F  -> G ^ F       { F.val = G.val ^ F.val }  (right-associative)
 *   F  -> G           { F.val = G.val }
 *   G  -> ( E )       { G.val = E.val }
 *   G  -> digit       { G.val = digit.lexval }
 *
 * Bottom-Up Strategy: Operator-precedence / recursive-descent with an
 * explicit value stack + symbol stack to simulate shift-reduce behaviour,
 * with full SDD semantic actions applied at each reduction.
 */

#include <iostream>
#include <string>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <cstdlib>

using namespace std;

// ─────────────────────────── Lexer ───────────────────────────────────────────

enum TokenType { TOK_NUM, TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV,
                 TOK_POW, TOK_LPAREN, TOK_RPAREN, TOK_EOF, TOK_INVALID };

struct Token {
    TokenType type;
    double    lexval;   // only meaningful when type == TOK_NUM
    Token(TokenType t = TOK_INVALID, double v = 0.0) : type(t), lexval(v) {}
};

class Lexer {
    const string& src;
    size_t        pos;
public:
    explicit Lexer(const string& s) : src(s), pos(0) {}

    Token nextToken() {
        // skip whitespace
        while (pos < src.size() && isspace((unsigned char)src[pos])) ++pos;
        if (pos >= src.size()) return Token(TOK_EOF);

        char c = src[pos];
        // number (integer or decimal, no leading sign here)
        if (isdigit((unsigned char)c) || c == '.') {
            string num;
            while (pos < src.size() &&
                   (isdigit((unsigned char)src[pos]) || src[pos] == '.'))
                num += src[pos++];
            double v = atof(num.c_str());
            return Token(TOK_NUM, v);
        }
        ++pos;
        switch (c) {
            case '+': return Token(TOK_PLUS);
            case '-': return Token(TOK_MINUS);
            case '*': return Token(TOK_MUL);
            case '/': return Token(TOK_DIV);
            case '^': return Token(TOK_POW);
            case '(': return Token(TOK_LPAREN);
            case ')': return Token(TOK_RPAREN);
            default:  return Token(TOK_INVALID);
        }
    }

    // Peek at next token without consuming
    Token peek() {
        size_t saved = pos;
        Token t = nextToken();
        pos = saved;
        return t;
    }
};

// ─────────────────── Recursive-descent with explicit stacks ──────────────────
// Each function below corresponds to a non-terminal in the grammar.
// The "shift" is the call to nextToken(); the "reduce" is the return with
// a computed .val — exactly as in SDD evaluation on a parse tree.

class Parser {
    Lexer  lex;
    Token  cur;          // look-ahead (current token)

    void advance() { cur = lex.nextToken(); }

    void expect(TokenType t, const string& msg) {
        if (cur.type != t) throw runtime_error(msg);
        advance();
    }

public:
    explicit Parser(const string& expr) : lex(expr) {
        advance();   // load first token (shift)
    }

    // L → E n   { Print(E.val) }
    double parse() {
        double val = parseE();
        if (cur.type != TOK_EOF)
            throw runtime_error("Unexpected token after expression");
        return val;
    }

private:
    // E  → T { E' }
    // E' → + T { E.val = E.val + T.val } E'
    //       | - T { E.val = E.val - T.val } E'
    //       | ε
    double parseE() {
        double val = parseT();                      // E → T
        while (cur.type == TOK_PLUS || cur.type == TOK_MINUS) {
            TokenType op = cur.type;
            advance();                              // shift + / -
            double tval = parseT();                 // T
            if (op == TOK_PLUS)
                val = val + tval;                   // REDUCE: E → E + T
            else
                val = val - tval;                   // REDUCE: E → E - T
        }
        return val;
    }

    // T  → F { T' }
    // T' → * F { T.val = T.val * F.val } T'
    //       | / F { T.val = T.val / F.val } T'
    //       | ε
    double parseT() {
        double val = parseF();                      // T → F
        while (cur.type == TOK_MUL || cur.type == TOK_DIV) {
            TokenType op = cur.type;
            advance();                              // shift * / /
            double fval = parseF();                 // F
            if (op == TOK_MUL)
                val = val * fval;                   // REDUCE: T → T * F
            else {
                if (fval == 0.0)
                    throw runtime_error("Division by zero");
                val = val / fval;                   // REDUCE: T → T / F
            }
        }
        return val;
    }

    // F  → G ^ F   { F.val = G.val ^ F.val }  (right-associative)
    //     | G       { F.val = G.val }
    double parseF() {
        double gval = parseG();                     // G
        if (cur.type == TOK_POW) {
            advance();                              // shift ^
            double fval = parseF();                 // F  (right-recursive → right-associative)
            return pow(gval, fval);                 // REDUCE: F → G ^ F
        }
        return gval;                                // REDUCE: F → G
    }

    // G  → ( E )   { G.val = E.val }
    //     | digit   { G.val = digit.lexval }
    double parseG() {
        if (cur.type == TOK_LPAREN) {
            advance();                              // shift (
            double val = parseE();                  // E
            expect(TOK_RPAREN, "Expected ')'");     // shift )
            return val;                             // REDUCE: G → ( E )
        }
        if (cur.type == TOK_NUM) {
            double v = cur.lexval;                  // digit.lexval
            advance();                              // shift digit
            return v;                               // REDUCE: G → digit
        }
        throw runtime_error("Expected number or '('");
    }
};

// ─────────────────────────── Main ────────────────────────────────────────────

int main() {
    cout << "======================================================" << endl;
    cout << "  Arithmetic Expression Evaluator (Bottom-Up / SDD)   " << endl;
    cout << "======================================================" << endl;
    cout << "Operators: +  -  *  /  ^   Grouping: (  )" << endl;
    cout << "Type 'exit' to quit." << endl;
    cout << "------------------------------------------------------" << endl;

    string line;
    while (true) {
        cout << "\nEnter expression: ";
        if (!getline(cin, line)) break;
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;

        try {
            Parser p(line);
            double result = p.parse();

            // Print result: show as integer if it's a whole number
            if (result == floor(result) && !isinf(result))
                cout << "Result: " << (long long)result << endl;
            else
                cout << "Result: " << result << endl;
        }
        catch (const exception& e) {
            cout << "Invalid expression" << endl;
            // Uncomment next line for debugging:
            // cout << "  (Reason: " << e.what() << ")" << endl;
        }
    }

    cout << "\nGoodbye!" << endl;
    return 0;
}
