/*****************************************************************************
TITLE: Two Pass Assembler																															
AUTHOR: [Ritu Kumari Singh]
ROLL NO: 2301CS75 
*****************************************************************************/
#include<bits/stdc++.h>
using namespace std;

// Global data structures

map<string, pair<string, int>> opcode;//stores the opcode and type of instruction
map<string, pair<int,int>> labeltable;// A map storing labels as keys and their associated values as {address, line number} pairs.
map<string,string>value;//Stores values for variables defined by SET or data directives.
map<string, vector<int>> labelloc;//Tracks the line numbers where each label is referenced
vector<string> machinecode;//stores the machine code
vector<string> input;//stores the input lines
map<int, string> warnings;//Track line-specific warnings and errors
map<int, string> errors;
map<int, pair<string, pair<string,string>>>info;//Stores details for each line, such as label, mnemonic, and operand
//The listing struct represents a line in the .lst listing file, which includes program counter, machine code, and original text. 
struct listing {
    string cnt, mcode, text;//stores the count, machine code and text
};
vector<listing> lst_info;

//check if the character is alphabet
bool is_alphabet(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
//check if the character is digit
bool is_digit(char c) {
    return c >= '0' && c <= '9';
}
//check if the string is decimal
bool is_decimal(string s) {
    if(s[0] == '0' && s.size() != 1) return false;
    for(int i = 1; i < s.size(); i++) {
        if(!is_digit(s[i])) return false;
    }
    return true;
}
//check if the string is octal
bool is_octal(string s) {
    if(s.size() < 2 || s[0] != '0') return false;
    for(int i = 1; i < s.size(); i++) {
        if(s[i] < '0' || s[i] > '7') return false;
    }
    return true;
}
//check if the string is hexadecimal
bool is_hexadecimal(string s) {
    if(s.size() < 3 || s[0] != '0' || tolower(s[1]) != 'x') return false;
    for(int i = 2; i < s.size(); i++) {
        if(!is_digit(s[i]) && (tolower(s[i]) < 'a' || tolower(s[i]) > 'f')) return false;
    }
    return true;
}
//check if the string is valid label
bool is_valid_label(string s) {
    if(s.empty() || !is_alphabet(s[0])) return false;
    for(int i = 1; i < s.size(); i++) {
        if(!is_alphabet(s[i]) && !is_digit(s[i])) return false;
    }
    return true;
}

// Base conversion functions
string decimal_to_hex(int n) {
    unsigned int num1 = n;
    string result;
    for(int i = 0; i < 8; i++) {
        int remainder = num1 % 16;
        if(remainder > 9) {
            result += (char)(remainder - 10 + 'a');
        } else {
            result += (char)(remainder + '0');
        }
        num1 /= 16;
    }
    reverse(result.begin(), result.end());
    return result;
}
string octal_to_decimal(string s) {
    int result = 0, power = 1;
    for(int i = s.size() - 1; i >= 0; i--) {
        result += (power * (s[i] - '0'));
        power *= 8;
    }
    return to_string(result);
}

string hex_to_decimal(string s) {
    int result = 0, power = 1;
    for(int i = s.size() - 1; i >= 0; i--) {
        if(is_digit(s[i])) {
            result += (power * (s[i] - '0'));
        } else {
            result += (power * ((tolower(s[i]) - 'a') + 10));
        }
        power *= 16;
    }
    return to_string(result);
}
// Label processing
void process_label(string label, int ctr, int line_no) {
    // Early returns for empty label
    if(label.empty()) return;
    
    // Early return for invalid label
    if(!is_valid_label(label)) {
        errors[line_no] = "Bogus Label name";
        return;
    }
    
    // Check if label exists and has valid value
    auto it = labeltable.find(label);
    bool label_exists = (it != labeltable.end());
    bool has_valid_value = (label_exists && it->second.first != -1);
    
    if(has_valid_value) {
        errors[line_no] = "Duplicate Label Definition";
        return;
    }
    
    // Insert or update label
    labeltable[label] = {ctr, line_no};
}

// Operand processing
string process_operand(string temp) {
    if(temp.empty()) return "";
    
    if(is_octal(temp)) {
        for(int i = 1; i < temp.size(); i++) {
            if(temp[i] >+ '8') {
                errors[0] = "Invalid Octal number";
                return "";
            }
        }
        return octal_to_decimal(temp.substr(1));
    }
    if(is_hexadecimal(temp)) {
        return hex_to_decimal(temp.substr(2));
    }
    if(is_decimal(temp)) {
        return temp;
    }
    return "";
}

// processing of a mnemonic and its associated operand
bool process_mnemonic(string mnemonic, string &operand, int prct, int linenum) {
    int type = opcode[mnemonic].second;
    
    if(type > 1) {
        string p;
        if(is_valid_label(operand)) {
            if(labeltable.find(operand) == labeltable.end()) {
                labeltable[operand] = {-1, linenum};
            }
            labelloc[operand].push_back(linenum);
            p = operand;
        } else {
            if(operand[0] == '+' || operand[0] == '-') {
                p = operand[0] + process_operand(operand.substr(1));
            } else {
                p = process_operand(operand);
            }
        }
        
        if(p.empty()) {
            errors[linenum] = "Invalid Format";
            return false;
        }
        operand = p;
        return true;
    }
    
    if(type == 1 && !operand.empty()) {
        errors[linenum] = "Unexpected operand";
        return false;
    }
    return true;
}

// Line parsing
vector<string> parse_line(string line) {
    vector<string> words;
    string currentWord;
    
    // Process each character in the line
    for(size_t i = 0; i < line.length(); i++) {
        char c = line[i];
        
        // Handle whitespace
        if(isspace(c)) {
            if(!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
            continue;
        }
        
        // Handle comments
        if(c == ';') {
            // Add the last word if it exists
            if(!currentWord.empty()) {
                words.push_back(currentWord);
            }
            break;
        }
        
        // Handle colon (label separator)
        if(c == ':') {
            // Add the label with colon
            currentWord += c;
            words.push_back(currentWord);
            currentWord.clear();
            continue;
        }
        
        // Build current word
        currentWord += c;
        
        // Special case: if we find a colon in the middle of a word
        size_t colonPos = currentWord.find(':');
        if(colonPos != string::npos && colonPos < currentWord.length() - 1) {
            // Split at colon...Splits tokens like label:ldc into ["label:", "ldc"]
            string label = currentWord.substr(0, colonPos + 1);
            words.push_back(label);
            // Start new word with remainder
            currentWord = currentWord.substr(colonPos + 1);
        }
    }
    
    // Add the last word if it exists
    if(!currentWord.empty()) {
        words.push_back(currentWord);
    }
    
    // Remove any empty strings from the result
    words.erase(
        remove_if(words.begin(), words.end(), 
            [](const string& s) { return s.empty(); }
        ),
        words.end()
    );
    
    return words;
}
void initialize_opcodes() {
    opcode["data"] = {"", 2};
    opcode["ldc"] = {"00", 2};
    opcode["adc"] = {"01", 2};
    opcode["ldl"] = {"02", 3};
    opcode["stl"] = {"03", 3};
    opcode["ldnl"] = {"04", 3};
    opcode["stnl"] = {"05", 3};
    opcode["add"] = {"06", 1};
    opcode["sub"] = {"07", 1};
    opcode["shl"] = {"08", 1};
    opcode["shr"] = {"09", 1};
    opcode["adj"] = {"0a", 2};
    opcode["a2sp"] = {"0b", 1};
    opcode["sp2a"] = {"0c", 1};
    opcode["call"] = {"0d", 3};
    opcode["return"] = {"0e", 1};
    opcode["brz"] = {"0f", 3};
    opcode["brlz"] = {"10", 3};
    opcode["br"] = {"11", 3};
    opcode["HALT"] = {"12", 1};
    opcode["SET"] = {"", 2};
}

void pass1() {
    int programCounter = 0;
    int currentLine = 0;
    
    // Process each line of input
    for(string line : input) {
        currentLine++;
        
        // Parse the line into words
        vector<string> words = parse_line(line);
        if(words.empty()) {
            continue;
        }
        
        // Extract label, mnemonic and operand
        int pos = 0;
        string label = "";
        string mnemonic = "";
        string operand = "";
        
        // Check for label (word ending with ':')
        if(pos < words.size() && words[pos].back() == ':') {
            label = words[pos];
            label.pop_back();  // Remove the colon
            pos++;
        }
        
        // Get mnemonic
        if(pos < words.size()) {
            mnemonic = words[pos];
            pos++;
        }
        
        // Get operand
        if(pos < words.size()) {
            operand = words[pos];
            pos++;
        }
        
        // Process the label if present
        if(!label.empty()) {
            process_label(label, programCounter, currentLine);
        }
        
        // Process the instruction
        int increment = 0;
        
        // Validate mnemonic if present
        if(!mnemonic.empty()) {
            // Check if mnemonic exists
            if(opcode.find(mnemonic) == opcode.end()) {
                errors[currentLine] = "Bogus Mnemonic";
            }
            // Check operand requirements
            else if(opcode[mnemonic].second > 1) {
                if(operand.empty()) {
                    errors[currentLine] = "Missing operand";
                }
                else if(pos < words.size()) {
                    errors[currentLine] = "Extra on end of line";
                }
            }
            
            // Process mnemonic and get size increment
            increment = process_mnemonic(mnemonic, operand, programCounter, currentLine);
            
            // Store instruction information
            info[programCounter] = {label, {mnemonic, operand}};
            
            // Handle SET instruction
            if(increment && mnemonic == "SET") {
                if(label.empty()) {
                    errors[currentLine] = "label name or variable name missing";
                }
                else {
                    value[label] = operand;
                }
            }
            
            programCounter += increment;
        }
        else {
            // If the line only contains a label (e.g., label:), store it without incrementing the programCounter
            info[programCounter] = {label, {mnemonic, operand}};
        }
    }
    
    // Validate all labels after processing
    for(const auto& entry : labeltable) {
        string label = entry.first;
        int address = entry.second.first;
        int declarationLine = entry.second.second;
        
        // Check for undefined labels
        if(address == -1) {
            for(int lineNum : labelloc[label]) {
                errors[lineNum] = "No such label";
            }
        }
        // Check for unused labels
        else if(labelloc.find(label) == labelloc.end()) {
            warnings[declarationLine] = "Label declared but not used";
        }
    }
}
void pass2() {
    for(auto &entry : info) {
        int pctr = entry.first;
        string label = entry.second.first;
        string mnemonic = entry.second.second.first;
        string operand = entry.second.second.second;
        
        string mcode = "       ";
        
        if(!mnemonic.empty()) {
            int type = opcode[mnemonic].second;
            
            if(type == 1) {
                mcode = "000000" + opcode[mnemonic].first;
            }
            else if(type == 2) {
                if(mnemonic == "data" || mnemonic == "SET") {
                    mcode = decimal_to_hex(stoi(operand));
                } else {
                    if(value.find(operand) != value.end()) {
                        mcode = decimal_to_hex(stoi(value[operand])).substr(2) + opcode[mnemonic].first;
                    } else {
                        int val = (labeltable.find(operand) == labeltable.end()) 
                                 ? stoi(operand) 
                                 : labeltable[operand].first;
                        mcode = decimal_to_hex(val).substr(2) + opcode[mnemonic].first;
                    }
                }
            }
            else if(type == 3) {
                int offset = (labeltable.find(operand) == labeltable.end())
                            ? stoi(operand)
                            : labeltable[operand].first - (pctr + 1);
                mcode = decimal_to_hex(offset).substr(2) + opcode[mnemonic].first;
            }
        }
        
        machinecode.push_back(mcode);
        
        if(!label.empty()) label += ": ";
        if(!mnemonic.empty()) mnemonic += " ";
        string text = label + mnemonic + operand;
        
    lst_info.push_back({decimal_to_hex(pctr), mcode, text});
    }
}
string get_base_filename(const string& filepath) {
    // Find the last occurrence of '/' or '\' to handle path
    size_t last_slash = filepath.find_last_of("/\\");
    string filename = (last_slash == string::npos) ? filepath : filepath.substr(last_slash + 1);
    
    // Find the position of the last dot
    size_t last_dot = filename.find_last_of(".");
    
    // Extract the base name (everything before the last dot)
    return (last_dot == string::npos) ? filename : filename.substr(0, last_dot);
}
void write_errors(const string& base_filename) {
    string log_file = base_filename + ".log";
    ofstream error_file(log_file);
    if(errors.empty()) {
        error_file << "HURRAY NO ERRORS\n";
        for(auto &warning : warnings) {
            error_file << "Line Number " << warning.first << " WARNING: " << warning.second << "\n";
        }
    } else {
        for(auto &error : errors) {
            error_file << "Line Number " << error.first << " ERROR: " << error.second << "\n";
        }
    }
    error_file.close();
}
void write_listing_file(const string& base_filename) {
    string lst_file = base_filename + ".lst";
    ofstream list_file(lst_file);
    for(auto &item : lst_info) {
        list_file << item.cnt << " " << item.mcode << " " << item.text << "\n";
    }
    list_file.close();
}


void write_machine_code(const string& base_filename) {
    string obj_file = base_filename + ".o";
    ofstream mcode_file(obj_file, ios::binary | ios::out);
    for(auto &code : machinecode) {
        if(code.empty() || code == "        ") continue;
        unsigned int value = (unsigned int)stoi(hex_to_decimal(code));
        mcode_file.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));
    }
    mcode_file.close();
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_filename>" << endl;
        return 1;
    }
    
    string input_filename = argv[1];
    string base_filename = get_base_filename(input_filename);
    
    ifstream input_file(input_filename);
    if(!input_file) {
        cerr << "Error: Cannot open file " << input_filename << endl;
        return 1;
    }
    
    string line;
    while(getline(input_file, line)) {
        input.push_back(line);
    }
    input_file.close();
    
    initialize_opcodes();
    pass1();
    write_errors(base_filename);
    
    if(errors.empty()) {
        pass2();
        write_listing_file(base_filename);
        write_machine_code(base_filename);
        cout << "Listing file generated as " << base_filename << ".lst\n";
        cout << "Machine code file generated as " << base_filename << ".o\n";
    }
    
    return 0;
}