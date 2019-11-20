#how do we write python code again?
import sys

#Setup some global values
NonPrefixed   = False      
CBPrefixed    = False      
Parenthesies  = False          
Commas        = False  
Curlies       = False  
Numbers       = False

#replace these letters with the propper values later
r = ["B", "C", "D", "E", "H", "L", "(HL)", "A"]
rp = ["BC", "DE", "HL", "SP"]
rp2 = ["BC", "DE", "HL", "AF"]
cc = ["NZ", "Z", "NC", "C", "PO", "PE", "P", "M"]
alu = ["_ADD8", "_ADC", "_SUB", "_SBC", "_AND", "_XOR", "_OR", "_CP"]
rot = ["_RLC", "_RRC", "_RL", "_RR", "_SLA", "_SRA", "_SLL", "_SRL"]

def main():
    #user code here
    #first checkout user input
    userInput()
    if(NonPrefixed):
        for opcode in range(0, 256):
            printBegin()
            PrintNoPrefix(opcode)
            printEnd(opcode)
    
    if(CBPrefixed):
        for opcode in range(0, 256):
            printBegin()
            printCBprefixed(opcode)
            printEnd(opcode)

def printBegin():
    if Parenthesies:
        print("( ", end="")
    elif Curlies:
        print("{ ", end = "")

def printEnd(_opcode):
    if Parenthesies:
        print(" )",  end = "")
    elif Curlies:
        print(" }", end = "")
    if Commas:
        print(",", end="")
    if Numbers:
        print(_opcode, end=" ")
    print("")

#Validate user input

def userInput():
    args = len(sys.argv)
    if args < 2:
        print("Not enough arguments")
        print("Use -a for all options (except curlies and parenthesies)")
        print("Use -n for non Prefixed opcodes")
        print("Use -c for CB prefixed opcodes")
        print("Use -com to seperate each opcode with a ','")
        print("Use -par for parenthesies '()'")
        print("Use -curl for curly brackets '{}'")
        exit(0)
    else:
        global NonPrefixed 
        global CBPrefixed  
        global Parenthesies
        global Commas      
        global Curlies     
        global Numbers
        
        for arg in range(0, args):
            if str(sys.argv[arg]) == '-a':
                NonPrefixed = True
                CBPrefixed = True
                Commas = True
            elif str(sys.argv[arg]) == '-n':
                NonPrefixed = True
            elif str(sys.argv[arg]) == "-c":
                CBPrefixed = True
            elif str(sys.argv[arg]) == "-com":
                Commas = True
            elif str(sys.argv[arg]) == "-num":
                Numbers = True
            elif str(sys.argv[arg]) == "-par":
                Parenthesies = True
                Curlies = False
            elif str(sys.argv[arg]) == "-curl":
                Curlies = True
                Parenthesies = False
#Man this is badly structured...

def PrintNoPrefix( _opcode ):
    #get the letter values for the opcode
    x = (_opcode & 0b11000000) >> 6
    y = (_opcode & 0b00111000) >> 3
    z = (_opcode & 0b00000111)
    p = (_opcode & 0b00110000) >> 4
    q = (_opcode & 0b00001000) >> 3

 #find x
    if x == 0:
        if z == 0:
            if y == 0:
                print("_NOP, NULL, NULL", end="")
            elif y == 1:
                print("_LD16, (a16), SP", end="") #load SP into a16
            elif y == 2:
                print("_STOP, NULL, NULL", end="")
            elif y == 3:
                print("_JR, r8, NULL", end="") #jump r8 ahead
            else:
                print("_JR, %s, r8"% cc[y-4], end="") #jump with condition, might need to get rolled out, or filtered (like put the condition in the second spot, so we can check it depending on that)
        elif z ==1:
            if q == 0:
                print("_LD16, %s, d16"% rp[p], end="") #load d16 into rp[p]
            else:
                print("_ADD16, HL, %s"% rp[p], end="")
        elif z ==2:
            if q == 0:
                if p == 0:
                    print("_LD8, (BC), A", end="")
                elif p == 1:
                    print("_LD8, (DE), A", end="")
                elif p == 2:
                    print("_LDI, (HL), A", end="")
                elif p == 3:
                    print("_LDD, (HL), A", end="")
            else:
                if p == 0:
                    print("_LD8, A, (BC)", end="")
                elif p == 1:
                    print("_LD8, A, (DE)", end="")
                elif p == 2:
                    print("_LDI, A, (HL)", end="")
                elif p == 3:
                    print("_LDD, A, (HL)", end="")
        elif z ==3:
            if q == 0:
                print("_INC16, %s, NULL"% rp[p], end="")
            else:
                print("_DEC16, %s, NULL"% rp[p], end="")
        elif z ==4:
            print("_INC8, %s, NULL"% r[y], end="")
        elif z ==5:
            print("_DEC8, %s, NULL"% r[y], end="")
        elif z ==6:
            print("_LD8, %s, d8"% r[y], end="")
        elif z ==7:
            if y == 0:
                print("_RLCA, NULL, NULL", end="")
            elif y == 1:
                print("_RRCA, NULL, NULL", end="")
            elif y == 2:
                print("_RLA, NULL, NULL", end="")
            elif y == 3:
                print("_RRA, NULL, NULL", end="")
            elif y == 4:
                print("_DAA, NULL, NULL", end="")
            elif y == 5:
                print("_CPL, NULL, NULL", end="")
            elif y == 6:
                print("_SCF, NULL, NULL", end="")
            elif y == 7:
                print("_CCF, NULL, NULL", end="")
            
    elif x == 1:
        if z == 6 and y == 6:
            print("_HALT, NULL, NULL", end="")
        else:
            print("_LD8, %s, %s"% (r[y], r[z]), end="")
    elif x == 2:
        print("%s, %s, NULL"% (alu[y], r[z]), end="")
    elif x == 3:
        if z == 0:
            if q == 0:
                if p == 0:
                    print("_RET, NZ, NULL", end="")
                elif p == 1:
                    print("_RET, NC, NULL", end="")
                elif p == 2:
                    print("_LDH, (a8), A", end="")  #NULL is (a8)
                elif p == 3:
                    print("_LDH, A, (a8)", end="") #NULL is (a8)
            else:
                if p == 0:
                    print("_RET, Z, NULL", end="")
                elif p == 1:
                    print("_RET, C, NULL", end="")
                elif p == 2:
                    print("_ADD16, SP, r8", end="")
                elif p == 3:
                    print("_LDHL, SP, r8", end="") #NULL is SP+r8, look into this
        elif z ==1:
            if q == 0:
                print("_POP, %s ,NULL"%(rp2[p]), end="")
            else:
                if p == 0:
                    print("_RET, NULL, NULL", end="")
                elif p == 1:
                    print("_RETI, NULL, NULL", end="")
                elif p == 2:
                    print("_JP, (HL), NULL", end="")
                elif p == 3:
                    print("_LD16, SP, HL", end="")
        elif z ==2:
            if q == 0:
                if p == 0:
                    print("_JP, NZ, a16", end="")
                elif p == 1:
                    print("_JP, NC, a16", end="")
                elif p == 2:
                    print("_LD8, (C), A", end="")
                elif p == 3:
                    print("_LD8, A, (C)", end="")
            else:
                if p == 0:
                    print("_JP, Z, a16", end="") #NULL = a16
                elif p == 1:
                    print("_JP, C, a16", end="") #NULL = a16
                elif p == 2:
                    print("_LD8, a16, A", end="") #NULL = a16
                elif p == 3:
                    print("_LD8, A, a16", end="") #NULL = a16
        elif z ==3:
            if y == 0:
                print("_JP, a16, NULL", end="")    #first NULL = a16
            elif y == 1:
                print("_CBpref, NULL, NULL", end="")    #CB Prefix
            elif y == 6:
                print("_DI, NULL, NULL", end="")
            elif y == 7:
                print("_EI, NULL, NULL", end="")
            else:
                print("_ERROR, NULL, NULL", end="") #something went wrong if you get here
        elif z ==4:
            if y < 4:
                print("_CALL, %s, NULL"%(cc[y]), end="")
            else:
                print("_ERROR, NULL, NULL", end="")
        elif z ==5:
            if q == 0:
                print("_PUSH, %s, NULL"%(rp2[p]), end="")
            elif p == 0:
                print("_CALL, a16, NULL", end="") #NULL = a16s
            else:
                print("_ERROR, NULL, NULL", end="") 
        elif z ==6:
            print("%s, d8, NULL"%(alu[y]), end="")
        elif z ==7:
            print("_RST, %s, NULL"%(y * 8), end="") #the numarical value is an interger, not a pointer. look into this.

    #==============
    # CB Prefixed Opcodes
    #==============
def printCBprefixed(_opcode):
    #get the letter values for the opcode
    x = (_opcode & 0b11000000) >> 6
    y = (_opcode & 0b00111000) >> 3
    z = (_opcode & 0b00000111)
    p = (_opcode & 0b00110000) >> 4
    q = (_opcode & 0b00001000) >> 3
#real wonkey code this right here

    if x == 0:
        print("%s, %s, NULL"%(rot[y], r[z]), end="")
    elif x ==1:
        print("_BIT, %s, %s"%(y, r[z]), end="")
    elif x ==2:
        print("_RES, %s, %s"%(y, r[z]), end="")
    elif x ==3:
        print("_SET, %s, %s"%(y, r[z]), end="")

main()