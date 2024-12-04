/*****************************************************************************
TITLE: Emulator																																
AUTHOR: [Ritu Kumari Singh]
ROLL NO: 2301CS75 
*****************************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// Global variables for CPU state
vector<int> memory;
vector<int> machineCode;
int programCounter = 0;
int stackPointer = 0;
int regA = 0;
int regB = 0;

// Arrays to track memory operations
vector<int> readAddresses;
vector<int> readValues;
vector<int> writeAddresses;
vector<int> writePrevValues;
vector<int> writeNewValues;

// Convert decimal to 8-digit hex string
string convertToHex(int number) {
    string result = "";
    unsigned int num = number;
    
    for(int i = 0; i < 8; i++) {
        int digit = num % 16;
        if(digit < 10)
            result = char(digit + '0') + result;
        else
            result = char(digit - 10 + 'a') + result;
        num = num / 16;
    }
    return result;
}

// Get opcode and offset from instruction
void decodeInstruction(int instruction, int* opcode, int* offset) {
    *opcode = instruction & 0xFF;//The opcode is the last (least significant) 8 bits of the instruction.
    *offset = (instruction >> 8) & 0x7FFFFF;//Extracts the next 23 bits (offset)
    
    // Handle negative offset
    if(instruction & (1 << 31)) {
        *offset -= (1 << 23);
    }
}

// Execute a single instruction
bool executeInstruction(int opcode, int offset) {
    int temp, address;
    
    switch(opcode) {
        case 0:  // ldc
            regB = regA;
            regA = offset;
            break;
            
        case 1:  // adc
            regA += offset;
            break;
            
        case 2:  // ldl
            regB = regA;
            address = stackPointer + offset;
            regA = memory[address];
            readAddresses.push_back(address);
            readValues.push_back(regA);
            break;
            
        case 3:  // stl
            address = stackPointer + offset;
            writeAddresses.push_back(address);
            writePrevValues.push_back(memory[address]);
            memory[address] = regA;
            writeNewValues.push_back(regA);
            regA = regB;
            break;
            
        case 4:  // ldnl
            address = regA + offset;
            temp = regA;
            regA = memory[address];
            readAddresses.push_back(address);
            readValues.push_back(regA);
            break;
            
        case 5:  // stnl
            address = regA + offset;
            writeAddresses.push_back(address);
            writePrevValues.push_back(memory[address]);
            memory[address] = regB;
            writeNewValues.push_back(regB);
            break;
            
        case 6:  // add
            regA = regB + regA;
            break;
            
        case 7:  // sub
            regA = regB - regA;
            break;
            
        case 8:  // shl
            regA = regB << regA;
            break;
            
        case 9:  // shr
            regA = regB >> regA;
            break;
            
        case 10: // adj
            stackPointer += offset;
            break;
            
        case 11: // a2sp
            stackPointer = regA;
            regA = regB;
            break;
            
        case 12: // sp2a
            regB = regA;
            regA = stackPointer;
            break;
            
        case 13: // call
            regB = regA;
            regA = programCounter;
            programCounter += offset;
            break;
            
        case 14: // return
            programCounter = regA;
            regA = regB;
            break;
            
        case 15: // brz
            if(regA == 0) 
                programCounter += offset;
            break;
            
        case 16: // brlz
            if(regA < 0) 
                programCounter += offset;
            break;
            
        case 17: // br
            programCounter += offset;
            break;
            
        case 18: // HALT
            return true;
    }
    return false;
}

// Load program from file
void loadProgram(string filename) {
    ifstream file(filename, ios::binary);
    unsigned int instruction;
    
    while(file.read((char*)&instruction, sizeof(int))) {
        machineCode.push_back(instruction);
        memory.push_back(instruction);
    }
    file.close();
}

// Execute and trace program
void traceProgramExecution() {
    programCounter = 0;
    int instructionCount = 0;
    const int MAX_INSTRUCTIONS = 1 << 24;
    
    while(programCounter < machineCode.size()) {
        int opcode, offset;
        decodeInstruction(machineCode[programCounter], &opcode, &offset);
        
        cout << "PC= " << convertToHex(programCounter) 
             << " SP= " << convertToHex(stackPointer)
             << " A= " << convertToHex(regA)
             << " B= " << convertToHex(regB) << "\n";
        
        if(executeInstruction(opcode, offset))
            break;
        
        programCounter++;
        instructionCount++;
        
        if(programCounter < 0 || instructionCount > MAX_INSTRUCTIONS) {
            cout << "Error: Program exceeded maximum instruction limit\n";
            return;
        }
    }
    
    cout << "Total instructions executed: " << instructionCount << "\n";
}

// Display instruction set
void displayInstructionSet() {
    cout << "Instruction Set Architecture:\n"
         << "0:  ldc  value    Load constant\n"
         << "1:  adc  value    Add constant\n"
         << "2:  ldl  value    Load local\n"
         << "3:  stl  value    Store local\n"
         << "4:  ldnl value    Load non-local\n"
         << "5:  stnl value    Store non-local\n"
         << "6:  add           Add\n"
         << "7:  sub           Subtract\n"
         << "8:  shl           Shift left\n"
         << "9:  shr           Shift right\n"
         << "10: adj  value    Adjust stack\n"
         << "11: a2sp          A to stack pointer\n"
         << "12: sp2a          Stack pointer to A\n"
         << "13: call offset   Call\n"
         << "14: return        Return\n"
         << "15: brz  offset   Branch if zero\n"
         << "16: brlz offset   Branch if less than zero\n"
         << "17: br   offset   Branch\n"
         << "18: HALT          Halt program\n";
}

// Reset CPU state
void wipeflags() {
    programCounter = 0;
    stackPointer = 0;
    regA = 0;
    regB = 0;
    readAddresses.clear();
    readValues.clear();
    writeAddresses.clear();
    writePrevValues.clear();
    writeNewValues.clear();
}

// Display memory contents
void showMemory(bool afterExecution) {
    vector<int>& memToShow = afterExecution ? memory : machineCode;
    cout << "Memory dump " << (afterExecution ? "after" : "before") << " execution:\n";
    
    for(int i = 0; i < memToShow.size(); i += 4) {
        cout << convertToHex(i) << " ";
        for(int j = i; j < min((int)memToShow.size(), i + 4); j++) {
            cout << convertToHex(memToShow[j]) << " ";
        }
        cout << "\n";
    }
}

// Display memory reads
void showMemoryReads() {
    for(int i = 0; i < readAddresses.size(); i++) {
        cout << "Read memory[" << convertToHex(readAddresses[i]) 
             << "] = " << convertToHex(readValues[i]) << "\n";
    }
}

// Display memory writes
void showMemoryWrites() {
    for(int i = 0; i < writeAddresses.size(); i++) {
        cout << "Write memory[" << convertToHex(writeAddresses[i]) 
             << "] was " << convertToHex(writePrevValues[i])
             << " now " << convertToHex(writeNewValues[i]) << "\n";
    }
}

int main() {
    string filename;
    cout << "Enter the filename of the program to run (include extension):\n";
    cin >> filename;
    
    loadProgram(filename);
    
    while(true) {
        cout << "\nMENU\n"
             << "1: Run program trace\n"
             << "2: Show instruction set\n"
             << "3: wipe written flags before execution\n"
             << "4: memory dump before execution\n"
             << "5: memory dump after execution\n"
             << "6: Show memory reads\n"
             << "7: Show memory writes\n"
             << "8 or higher: Exit\n\n"
             << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        
        if(choice >= 8) 
            break;
            
        switch(choice) {
            case 1: traceProgramExecution(); break;
            case 2: displayInstructionSet(); break;
            case 3: wipeflags(); break;
            case 4: showMemory(false); break;
            case 5: showMemory(true); break;
            case 6: showMemoryReads(); break;
            case 7: showMemoryWrites(); break;
        }
    }
    
    return 0;
}