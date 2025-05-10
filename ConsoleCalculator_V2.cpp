#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include <stack>
#include <cmath>
#include <map>
#include <limits>

int continue_ = 0;

// Function to apply an operator to two numbers
double applyOperation(double a, double b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return (b != 0) ? a / b : throw std::runtime_error("Error: Division by zero!");
    case '^': return std::pow(a, b);
    default: throw std::runtime_error("Error: Unknown operator!");
    }
}

// Define operator precedence
std::map<char, int> precedence = {
    {'+', 1}, {'-', 1}, {'*', 2}, {'/', 2}, {'^', 3}
};

// Function to evaluate the expression using stacks
double evaluateExpression(std::vector<std::string>::const_iterator& it,
    const std::vector<std::string>::const_iterator end) {
    std::stack<double> numbers;
    std::stack<char> operations;

    while (it != end && *it != ")") {
        std::string token = *it;

        if (token == "(") {
            // Skip the "(" token.
            ++it;
            // Recursively evaluate the sub-expression.
            double subExprResult = evaluateExpression(it, end);
            numbers.push(subExprResult);
            // Don't increment it here; the recursive call already advanced it.
            continue;
        }
        else if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) {
            // Process number tokens.
            numbers.push(std::stod(token));
            ++it;  // Manually advance after processing a number.
            continue;
        }
        else if (precedence.count(token[0])) {
            // Process operator tokens.
            while (!operations.empty() &&
                ((token[0] != '^' && precedence[token[0]] <= precedence[operations.top()]) ||
                    (token[0] == '^' && precedence[token[0]] < precedence[operations.top()]))) {
                double b = numbers.top();
                numbers.pop();
                if (numbers.empty())
                    throw std::runtime_error("Error: Misplaced operator.");
                double a = numbers.top();
                numbers.pop();
                char op = operations.top();
                operations.pop();
                numbers.push(applyOperation(a, b, op));
            }
            operations.push(token[0]);
            ++it;  // Manually advance after processing an operator.
            continue;
        }
        else {
            throw std::runtime_error("Error: Invalid character '" + token + "' in expression.");
        }
    }

    // If there's a closing parenthesis, consume it.
    if (it != end && *it == ")") {
        ++it;
    }

    // Apply any remaining operations.
    while (!operations.empty()) {
        double b = numbers.top();
        numbers.pop();
        if (numbers.empty())
            throw std::runtime_error("Error: Misplaced operator.");
        double a = numbers.top();
        numbers.pop();
        char op = operations.top();
        operations.pop();
        numbers.push(applyOperation(a, b, op));
    }

    return numbers.top();
}


// Function to check if parentheses are correctly balanced
bool checkParentheses(const std::string& equation) {
    int balance = 0;
    for (char ch : equation) {
        if (ch == '(') balance++;
        else if (ch == ')') balance--;
        if (balance < 0) return false; // Too many closing parentheses
    }
    return balance == 0;
}

// Tokenizer function to split input into meaningful parts
std::vector<std::string> tokenize(const std::string& equation) {
    std::vector<std::string> tokens;
    std::string current;

    auto flushCurrent = [&]() {
        if (!current.empty()) {
            tokens.push_back(current);
            current.clear();
        }
        };

    for (size_t i = 0; i < equation.size(); ++i) {
        char ch = equation[i];
        if (isspace(ch)) continue;

        // Determine if this char should be part of a number.
        // Accept digits and a dot (for decimals)
        // Also, treat '-' as part of a number if it is at the start,
        // after an opening parenthesis, or after an operator.
        if (isdigit(ch) || ch == '.' ||
            (ch == '-' && (i == 0 ||
                equation[i - 1] == '(' ||
                equation[i - 1] == '+' ||
                equation[i - 1] == '-' ||
                equation[i - 1] == '*' ||
                equation[i - 1] == '/' ||
                equation[i - 1] == '^'))) {
            current += ch;
        }
        else {
            flushCurrent();
            tokens.push_back(std::string(1, ch));
        }
    }
    flushCurrent();
    return tokens;
}

// Function to evaluate the full expression
double evaluate(const std::vector<std::string>& tokens) {
    auto it = tokens.begin();
    return evaluateExpression(it, tokens.end());
}

void askContinue() {
    char y_n;
    std::cout << "---------------------\nDo you want to continue? [y/n]:";
    std::cin >> y_n;
    // Flush the input buffer to remove the leftover newline.
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (y_n == 'y') {
        continue_ = 0;
        std::cout << "---------------------" << std::endl;
    }
    else if (y_n == 'n') {
        continue_ = 1;
        std::cout << "---------------------\nProgram exited." << std::endl;
    }
    else {
        std::cout << "Invalid input!" << std::endl;
        askContinue();
    }
}

// Main function to run the calculator
int main() {
    while (continue_ == 0) {
        std::string input;
        std::cout << "Enter an equation: ";
        std::getline(std::cin, input);

        try {
            if (!checkParentheses(input)) {
                throw std::runtime_error("Error: Mismatched parentheses.");
            }

            std::vector<std::string> tokens = tokenize(input);
            double result = evaluate(tokens);
            std::cout << "Result: " << result << std::endl;
        }
        catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        }
        askContinue();
    }
    return 0;
}