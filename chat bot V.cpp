
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

const double PI = 3.14159265358979323846;

// -------------------- GENERAL UTILITIES --------------------

/*CHAT
 hello                 Greet Nova
  how are you           Talk to Nova
  joke                  Hear a joke
  fact                  Get a random fun fact
  time                  Display date and time
  about                 About Nova

MATH
 calc 2*(5+3)          Calculate an expression
  calc sqrt(144)        Square root
  calc 2^8              Powers
  calc sin(pi/2)        Trigonometry (radians)
  calc log(100)         Base-10 logarithm
  calc ln(e)            Natural logarithm

GAMES
  game guess            Guess a number
  game rps              Rock-paper-scissors
  game quiz             General knowledge quiz

PRODUCTIVITY
  task add Buy milk     Add a task
  task list             List tasks
  task done 1           Complete task number 1
  note Remember this    Save a note to a file

OTHER
  help                  Show this menu
  clear                 Clear the terminal
  exit                  Quit Nova*/

string lower(string s) {
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

string trim(string s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

string getTime() {
    time_t now = time(nullptr);
    tm local{};

#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif

    ostringstream out;
    out << put_time(&local, "%Y-%m-%d %H:%M:%S");
    return out.str();
}

// -------------------- ADVANCED CALCULATOR --------------------

class Calculator {
    string s;
    size_t pos = 0;

    void spaces() {
        while (pos < s.size() &&
               isspace(static_cast<unsigned char>(s[pos]))) {
            ++pos;
        }
    }

    bool eat(char c) {
        spaces();
        if (pos < s.size() && s[pos] == c) {
            ++pos;
            return true;
        }
        return false;
    }

    string identifier() {
        spaces();
        size_t start = pos;

        while (pos < s.size() &&
               (isalpha(static_cast<unsigned char>(s[pos])) ||
                s[pos] == '_')) {
            ++pos;
        }

        return lower(s.substr(start, pos - start));
    }

    double expression() {
        double value = term();

        while (true) {
            if (eat('+')) value += term();
            else if (eat('-')) value -= term();
            else break;
        }

        return value;
    }

    double term() {
        double value = unary();

        while (true) {
            if (eat('*')) {
                value *= unary();
            } else if (eat('/')) {
                double divisor = unary();
                if (divisor == 0)
                    throw runtime_error("Division by zero is not allowed.");
                value /= divisor;
            } else if (eat('%')) {
                double divisor = unary();
                if (divisor == 0)
                    throw runtime_error("Modulo by zero is not allowed.");
                value = fmod(value, divisor);
            } else {
                break;
            }
        }

        return value;
    }

    double unary() {
        if (eat('+')) return unary();
        if (eat('-')) return -unary();
        return power();
    }

    double power() {
        double value = primary();

        if (eat('^')) {
            value = pow(value, unary());
        }

        return value;
    }

    double primary() {
        spaces();

        if (eat('(')) {
            double value = expression();

            if (!eat(')'))
                throw runtime_error("Missing closing parenthesis.");

            return value;
        }

        if (pos < s.size() &&
            (isdigit(static_cast<unsigned char>(s[pos])) ||
             s[pos] == '.')) {
            const char* start = s.c_str() + pos;
            char* end = nullptr;
            double value = strtod(start, &end);

            if (end == start)
                throw runtime_error("Invalid number.");

            pos += static_cast<size_t>(end - start);
            return value;
        }

        string name = identifier();

        if (name.empty())
            throw runtime_error("Expected a number, constant, or function.");

        if (name == "pi") return PI;
        if (name == "e") return exp(1.0);

        if (!eat('('))
            throw runtime_error("Unknown name: " + name);

        double x = expression();

        if (!eat(')'))
            throw runtime_error("Missing closing parenthesis.");

        if (name == "sqrt") {
            if (x < 0) throw runtime_error("Square root needs x >= 0.");
            return sqrt(x);
        }
        if (name == "abs") return fabs(x);
        if (name == "sin") return sin(x);
        if (name == "cos") return cos(x);
        if (name == "tan") return tan(x);
        if (name == "log") {
            if (x <= 0) throw runtime_error("log requires x > 0.");
            return log10(x);
        }
        if (name == "ln") {
            if (x <= 0) throw runtime_error("ln requires x > 0.");
            return log(x);
        }
        if (name == "floor") return floor(x);
        if (name == "ceil") return ceil(x);

        throw runtime_error("Unknown function: " + name);
    }

public:
    double calculate(const string& input) {
        s = input;
        pos = 0;

        double result = expression();
        spaces();

        if (pos != s.size())
            throw runtime_error("Unexpected character near: " +
                                s.substr(pos));

        if (!isfinite(result))
            throw runtime_error("Result is outside the supported range.");

        return result;
    }
};

// -------------------- GAMES --------------------

mt19937 rng(random_device{}());

void guessingGame() {
    uniform_int_distribution<int> dist(1, 100);
    int secret = dist(rng);
    int guess, attempts = 0;

    cout << "\nGuess the number between 1 and 100!\n";

    while (true) {
        cout << "Your guess: ";

        if (!(cin >> guess)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a valid whole number.\n";
            continue;
        }

        cin.ignore(10000, '\n');
        ++attempts;

        if (guess == secret) {
            cout << "Correct! You won in " << attempts
                 << " attempts!\n";
            break;
        }

        cout << (guess < secret ? "Too low!\n" : "Too high!\n");
    }
}

void rpsGame() {
    vector<string> choices = {"rock", "paper", "scissors"};
    uniform_int_distribution<int> dist(0, 2);

    cout << "\nRock, paper, scissors!\n";
    cout << "Type rock, paper, scissors, or quit.\n";

    while (true) {
        cout << "You: ";
        string user;
        getline(cin, user);
        user = lower(trim(user));

        if (user == "quit") break;

        if (user != "rock" && user != "paper" &&
            user != "scissors") {
            cout << "Choose rock, paper, or scissors.\n";
            continue;
        }

        string bot = choices[dist(rng)];
        cout << "Nova: " << bot << '\n';

        if (user == bot) {
            cout << "It's a draw!\n";
        } else if (
            (user == "rock" && bot == "scissors") ||
            (user == "paper" && bot == "rock") ||
            (user == "scissors" && bot == "paper")) {
            cout << "You win!\n";
        } else {
            cout << "Nova wins!\n";
        }
    }
}

void quizGame() {
    struct Question {
        string question;
        vector<string> options;
        char answer;
    };

    vector<Question> questions = {
        {"What is the largest planet?",
         {"A. Earth", "B. Jupiter", "C. Mars", "D. Venus"}, 'B'},
        {"What is 12 squared?",
         {"A. 124", "B. 122", "C. 144", "D. 24"}, 'C'},
        {"Which language is this chatbot written in?",
         {"A. C++", "B. HTML", "C. SQL", "D. CSS"}, 'A'},
        {"How many sides does a hexagon have?",
         {"A. 5", "B. 7", "C. 8", "D. 6"}, 'D'},
        {"Which star is closest to Earth?",
         {"A. Sirius", "B. The Sun", "C. Polaris", "D. Vega"}, 'B'}
    };

    shuffle(questions.begin(), questions.end(), rng);
    int score = 0;

    cout << "\nWelcome to the Nova quiz!\n";

    for (const auto& q : questions) {
        cout << "\n" << q.question << '\n';

        for (const auto& option : q.options)
            cout << option << '\n';

        cout << "Answer (A-D): ";
        string answer;
        getline(cin, answer);
        answer = lower(trim(answer));

        if (answer.size() == 1 &&
            toupper(static_cast<unsigned char>(answer[0])) == q.answer) {
            cout << "Correct!\n";
            ++score;
        } else {
            cout << "Not quite! The answer is " << q.answer << ".\n";
        }
    }

    cout << "\nFinal score: " << score << "/"
         << questions.size() << '\n';
}

// -------------------- NOTES AND TASKS --------------------

vector<string> tasks;

void addTask(const string& task) {
    if (task.empty()) {
        cout << "Usage: task add <description>\n";
        return;
    }

    tasks.push_back(task);
    cout << "Task added: " << task << '\n';
}

void listTasks() {
    if (tasks.empty()) {
        cout << "You have no tasks yet.\n";
        return;
    }

    cout << "\nYour tasks:\n";
    for (size_t i = 0; i < tasks.size(); ++i)
        cout << i + 1 << ". " << tasks[i] << '\n';
}

void completeTask(const string& arg) {
    try {
        size_t used = 0;
        int number = stoi(arg, &used);

        if (used != arg.size() || number < 1 ||
            number > static_cast<int>(tasks.size())) {
            cout << "Invalid task number.\n";
            return;
        }

        cout << "Completed: " << tasks[number - 1] << '\n';
        tasks.erase(tasks.begin() + number - 1);
    } catch (...) {
        cout << "Usage: task done <number>\n";
    }
}

void saveNote(const string& note) {
    if (note.empty()) {
        cout << "Usage: note <your message>\n";
        return;
    }

    ofstream file("nova_notes.txt", ios::app);

    if (!file) {
        cout << "Could not open the notes file.\n";
        return;
    }

    file << "[" << getTime() << "] " << note << '\n';
    cout << "Note saved to nova_notes.txt\n";
}

// -------------------- CHATBOT --------------------

void showHelp() {
    cout << R"(
============== NOVA AI HELP ==============

CHAT
  hello                 Greet Nova
  how are you           Talk to Nova
  joke                  Hear a joke
  fact                  Get a random fun fact
  time                  Display date and time
  about                 About Nova

MATH
  calc 2*(5+3)          Calculate an expression
  calc sqrt(144)        Square root
  calc 2^8              Powers
  calc sin(pi/2)        Trigonometry (radians)
  calc log(100)         Base-10 logarithm
  calc ln(e)            Natural logarithm

GAMES
  game guess            Guess a number
  game rps              Rock-paper-scissors
  game quiz             General knowledge quiz

PRODUCTIVITY
  task add Buy milk     Add a task
  task list             List tasks
  task done 1           Complete task number 1
  note Remember this    Save a note to a file

OTHER
  help                  Show this menu
  clear                 Clear the terminal
  exit                  Quit Nova

==========================================
)";
}

void respond(string input) {
    string message = lower(trim(input));

    if (message.empty()) return;

    if (message == "help" || message == "commands") {
        showHelp();
    } else if (message == "hello" || message == "hi" ||
               message == "hey" || message == "hello nova") {
        cout << "Nova: Hello! How can I help you today?\n";
    } else if (message.find("how are you") != string::npos) {
        cout << "Nova: I'm running perfectly! What shall we do?\n";
    } else if (message.find("your name") != string::npos ||
               message == "about") {
        cout << "Nova: I'm Nova AI, a C++ chatbot with math, "
                "games, and productivity tools.\n";
    } else if (message.find("thank") != string::npos) {
        cout << "Nova: You're welcome! Happy to help.\n";
    } else if (message.find("who created you") != string::npos) {
        cout << "Nova: I'm a customizable chatbot project written in C++ created by Victor.\n";
    } else if (message.find("what can you do") != string::npos) {
        cout << "Nova: I can calculate math, play games, save notes, "
                "manage tasks, and chat about simple topics. "
                "Type help to see my commands.\n";
    } else if (message.find("joke") != string::npos) {
        vector<string> jokes = {
            "Why do programmers prefer dark mode? Because light attracts bugs!",
            "Why was the computer cold? It left its Windows open!",
            "Why do C++ programmers love nature? They enjoy seeing trees!"
        };
        uniform_int_distribution<int> dist(0, static_cast<int>(jokes.size()) - 1);
        cout << "Nova: " << jokes[dist(rng)] << '\n';
    } else if (message == "fact") {
        vector<string> facts = {
            "Octopuses have three hearts.",
            "A day on Venus is longer than its year.",
            "Honey can remain edible for an extremely long time when stored properly.",
            "Sharks existed before trees."
        };
        uniform_int_distribution<int> dist(0, static_cast<int>(facts.size()) - 1);
        cout << "Nova: Fun fact: " << facts[dist(rng)] << '\n';
    } else if (message == "time" || message == "date") {
        cout << "Nova: Current date and time: " << getTime() << '\n';
    } else if (message.rfind("calc ", 0) == 0) {
        try {
            Calculator calculator;
            double result = calculator.calculate(trim(input.substr(5)));
            cout << "Nova: Result = " << result << '\n';
        } catch (const exception& e) {
            cout << "Nova: Math error: " << e.what() << '\n';
        }
    } else if (message == "game guess") {
        guessingGame();
    } else if (message == "game rps") {
        rpsGame();
    } else if (message == "game quiz") {
        quizGame();
    } else if (message == "task list") {
        listTasks();
    } else if (message.rfind("task add ", 0) == 0) {
        addTask(trim(input.substr(9)));
    } else if (message.rfind("task done ", 0) == 0) {
        completeTask(trim(input.substr(10)));
    } else if (message.rfind("note ", 0) == 0) {
        saveNote(trim(input.substr(5)));
    } else if (message == "clear") {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    } else if (message == "exit" || message == "quit") {
        cout << "Nova: Goodbye! See you next time.\n";
        exit(0);
    } else {
        cout << "Nova: I'm not sure how to answer that yet. "
                "Try 'help', or calculate with 'calc 5*(8+2)'.\n";
    }
}

// -------------------- MAIN PROGRAM --------------------

int main() {
    cout << R"( 
 
 __   _____ ____ _  ______   __
 \ \   / /_ _/ ___| |/ / _ \ / /
  \ \ / / | | |   | ' / | | / /
   \ V /  | | |___| . \ |_| / /_
    \_/  |___\____|_|\_\___/____/

           VICKO AI
      Created by Victor
          Vicko AI CHATBOT
)";

    cout << "\nNova: Welcome! Type 'help' to explore my features.\n";

    string input;

    while (true) {
        cout << "\nYou: ";

        if (!getline(cin, input)) {
            cout << "\nNova: Goodbye!\n";
            break;
        }

        respond(input);
    }

    return 0;
}
