#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <stdexcept>

// ─────────────────────────────────────────────
//  Token types
// ─────────────────────────────────────────────
enum TokenType { NUMBER, VARIABLE, OP, LPAREN, RPAREN, END };

struct Token {
    TokenType type;
    std::string text;   // raw text (operator symbol, variable name, number string)
    double value;       // only meaningful when type == NUMBER
};

// ─────────────────────────────────────────────
//  Lexer: turn the input string into tokens
// ─────────────────────────────────────────────
std::vector<Token> tokenize(const std::string& expr) {
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < expr.size()) {

        // skip whitespace
        if (std::isspace(expr[i])) { i++; continue; }

        // number (integer or decimal)
        if (std::isdigit(expr[i]) || expr[i] == '.') {
            std::string num;
            while (i < expr.size() && (std::isdigit(expr[i]) || expr[i] == '.'))
                num += expr[i++];
            tokens.push_back({ NUMBER, num, std::stod(num) });
            continue;
        }

        // variable (starts with a letter or underscore)
        if (std::isalpha(expr[i]) || expr[i] == '_') {
            std::string var;
            while (i < expr.size() && (std::isalnum(expr[i]) || expr[i] == '_'))
                var += expr[i++];
            tokens.push_back({ VARIABLE, var, 0.0 });
            continue;
        }

        // operators and parentheses
        char c = expr[i++];
        if (c == '(')  tokens.push_back({ LPAREN,  "(", 0.0 });
        else if (c == ')') tokens.push_back({ RPAREN,  ")", 0.0 });
        else if (c == '+' || c == '-' || c == '*' || c == '/')
            tokens.push_back({ OP, std::string(1, c), 0.0 });
        else
            throw std::runtime_error(std::string("Unknown character: ") + c);
    }

    tokens.push_back({ END, "", 0.0 });
    return tokens;
}

// ─────────────────────────────────────────────
//  AST node
// ─────────────────────────────────────────────
struct Node {
    enum Kind { NUM, VAR, BINOP } kind;
    double numVal;          // for NUM
    std::string varName;    // for VAR
    char op;                // for BINOP
    Node* left  = nullptr;  // for BINOP
    Node* right = nullptr;  // for BINOP
};

Node* makeNum(double v)             { Node* n = new Node; n->kind = Node::NUM; n->numVal = v; return n; }
Node* makeVar(const std::string& s) { Node* n = new Node; n->kind = Node::VAR; n->varName = s; return n; }
Node* makeBinop(char op, Node* l, Node* r) {
    Node* n = new Node; n->kind = Node::BINOP; n->op = op; n->left = l; n->right = r; return n;
}

// ─────────────────────────────────────────────
//  Recursive-descent parser
//  Grammar:
//    expr   → term   { ('+' | '-') term }
//    term   → factor { ('*' | '/') factor }
//    factor → NUMBER | VARIABLE | '(' expr ')'
//           | '-' factor          (unary minus)
// ─────────────────────────────────────────────
struct Parser {
    const std::vector<Token>& tokens;
    size_t pos = 0;

    Token& cur() { return const_cast<Token&>(tokens[pos]); }
    Token  consume() { return tokens[pos++]; }

    Node* parseExpr() {
        Node* node = parseTerm();
        while (cur().type == OP && (cur().text == "+" || cur().text == "-")) {
            char op = consume().text[0];
            node = makeBinop(op, node, parseTerm());
        }
        return node;
    }

    Node* parseTerm() {
        Node* node = parseFactor();
        while (cur().type == OP && (cur().text == "*" || cur().text == "/")) {
            char op = consume().text[0];
            node = makeBinop(op, node, parseFactor());
        }
        return node;
    }

    Node* parseFactor() {
        // unary minus
        if (cur().type == OP && cur().text == "-") {
            consume();
            Node* operand = parseFactor();
            // fold immediately if operand is a number
            if (operand->kind == Node::NUM)
                return makeNum(-operand->numVal);
            return makeBinop('-', makeNum(0), operand);
        }

        if (cur().type == LPAREN) {
            consume();           // eat '('
            Node* node = parseExpr();
            consume();           // eat ')'
            return node;
        }

        if (cur().type == NUMBER) {
            double v = cur().value;
            consume();
            return makeNum(v);
        }

        if (cur().type == VARIABLE) {
            std::string name = cur().text;
            consume();
            return makeVar(name);
        }

        throw std::runtime_error("Unexpected token: " + cur().text);
    }
};

// ─────────────────────────────────────────────
//  Constant folding pass
//  If both children of a BINOP are NUM, evaluate now.
// ─────────────────────────────────────────────
Node* fold(Node* node) {
    if (!node) return node;

    if (node->kind == Node::BINOP) {
        node->left  = fold(node->left);
        node->right = fold(node->right);

        if (node->left->kind == Node::NUM && node->right->kind == Node::NUM) {
            double l = node->left->numVal, r = node->right->numVal, result;
            switch (node->op) {
                case '+': result = l + r; break;
                case '-': result = l - r; break;
                case '*': result = l * r; break;
                case '/': result = l / r; break;
                default: return node;
            }
            return makeNum(result);
        }
    }
    return node;
}

// ─────────────────────────────────────────────
//  Convert AST back to a readable string
// ─────────────────────────────────────────────
// Helper: does this node need parentheses when used as a child of parentOp?
bool needsParens(Node* node, char parentOp, bool isRightChild) {
    if (!node || node->kind != Node::BINOP) return false;
    char childOp = node->op;

    int parentPrec = (parentOp == '*' || parentOp == '/') ? 2 : 1;
    int childPrec  = (childOp  == '*' || childOp  == '/') ? 2 : 1;

    if (childPrec < parentPrec) return true;
    // right-hand side of - or / with same precedence needs parens to preserve order
    if (childPrec == parentPrec && isRightChild && (parentOp == '-' || parentOp == '/'))
        return true;
    return false;
}

std::string toString(Node* node, char parentOp = 0, bool isRight = false) {
    if (!node) return "";

    if (node->kind == Node::NUM) {
        // print as integer if the value is whole, otherwise as decimal
        double v = node->numVal;
        if (v == (long long)v)
            return std::to_string((long long)v);
        // up to ~6 significant digits, strip trailing zeros
        std::ostringstream oss;
        oss << v;
        return oss.str();
    }

    if (node->kind == Node::VAR) return node->varName;

    // BINOP
    std::string inner = toString(node->left, node->op, false)
                      + " " + node->op + " "
                      + toString(node->right, node->op, true);

    if (parentOp && needsParens(node, parentOp, isRight))
        return "(" + inner + ")";
    return inner;
}

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────
int main() {
    std::string expr;
    std::cout << "Enter expression: ";
    std::getline(std::cin, expr);

    try {
        std::vector<Token> tokens = tokenize(expr);
        Parser parser{ tokens };
        Node* tree = parser.parseExpr();
        Node* optimized = fold(tree);
        std::cout << "Optimized: " << toString(optimized) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}