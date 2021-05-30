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
r = ["REG_B", "REG_C", "REG_D", "REG_E", "REG_H", "REG_L", "ADDRESS_REG_HL", "REG_A"]
rp = ["REG_BC", "REG_DE", "REG_HL", "REG_SP"]
rp2 = ["REG_BC", "REG_DE", "REG_HL", "REG_AF"]
cc = ["NZ", "Z", "NC", "C", "_err", "_err", "_err", "_err"]
alu = ["OP_ADD8", "OP_ADC", "OP_SUB", "OP_SBC", "OP_AND", "OP_XOR", "OP_OR", "OP_CP"]
rot = ["OP_RLC", "OP_RRC", "OP_RL", "OP_RR", "OP_SLA", "OP_SRA", "OP_SWAP", "OP_SRL"]

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
        print("\t", end="")
        print(_opcode, end=" ")
    print("")

#Validate user input

def userInput():
    args = len(sys.argv)
    if args < 2:
        print("Not enough arguments\n")
        print("Opcode set selection")
        print("Use -a for '-n -c -com'")
        print("Use -n for non Prefixed opcodes")
        print("Use -c for CB prefixed opcodes")
        print("\nMaking lines look pretty")
        print("Use -com to seperate each opcode with a ','")
        print("Use -num to number each opcode, mainly used for debugging")
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
                print("OP_NOP, NONE, NONE", end="")
            elif y == 1:
                print("OP_LD16, ADDRESS_16BIT, REG_SP", end="") #load SP into a16
            elif y == 2:
                print("OP_STOP, NONE, NONE", end="")
            elif y == 3:
                print("OP_JR, RELATIVE_8BIT, NONE", end="") #jump r8 ahead
            else:
                print("OP_JR_%s, RELATIVE_8BIT, NONE"% cc[y-4], end="") #jump with condition, might need to get rolled out, or filtered (like put the condition in the second spot, so we can check it depending on that)
        elif z ==1:
            if q == 0:
                print("OP_LD16, %s, IMMEDIATE_16BIT"% rp[p], end="") #load d16 into rp[p]
            else:
                print("OP_ADD16, REG_HL, %s"% rp[p], end="")
        elif z ==2:
            if q == 0:
                if p == 0:
                    print("OP_LD8, ADDRESS_REG_BC, REG_A", end="")
                elif p == 1:
                    print("OP_LD8, ADDRESS_REG_DE, REG_A", end="")
                elif p == 2:
                    print("OP_LD8, ADDRESS_REG_HLI, REG_A", end="")
                elif p == 3:
                    print("OP_LD8, ADDRESS_REG_HLD, REG_A", end="")
            else:
                if p == 0:
                    print("OP_LD8, REG_A, ADDRESS_REG_BC", end="")
                elif p == 1:
                    print("OP_LD8, REG_A, ADDRESS_REG_DE", end="")
                elif p == 2:
                    print("OP_LD8, REG_A, ADDRESS_REG_HLI", end="")
                elif p == 3:
                    print("OP_LD8, REG_A, ADDRESS_REG_HLD", end="")
        elif z ==3:
            if q == 0:
                print("OP_INC16, %s, NONE"% rp[p], end="")
            else:
                print("OP_DEC16, %s, NONE"% rp[p], end="")
        elif z ==4:
            print("OP_INC8, %s, NONE"% r[y], end="")
        elif z ==5:
            print("OP_DEC8, %s, NONE"% r[y], end="")
        elif z ==6:
            print("OP_LD8, %s, IMMEDIATE_8BIT"% r[y], end="")
        elif z ==7:
            if y == 0:
                print("OP_RLCA, NONE, NONE", end="")
            elif y == 1:
                print("OP_RRCA, NONE, NONE", end="")
            elif y == 2:
                print("OP_RLA, NONE, NONE", end="")
            elif y == 3:
                print("OP_RRA, NONE, NONE", end="")
            elif y == 4:
                print("OP_DAA, NONE, NONE", end="")
            elif y == 5:
                print("OP_CPL, NONE, NONE", end="")
            elif y == 6:
                print("OP_SCF, NONE, NONE", end="")
            elif y == 7:
                print("OP_CCF, NONE, NONE", end="")
            
    elif x == 1:
        if z == 6 and y == 6:
            print("OP_HALT, NONE, NONE", end="")
        else:
            print("OP_LD8, %s, %s"% (r[y], r[z]), end="")
    elif x == 2:
        print("%s, REG_A, %s"% (alu[y], r[z]), end="")
    elif x == 3:
        if z == 0:
            if q == 0:
                if p == 0:
                    print("OP_RET_NZ, NONE, NONE", end="")
                elif p == 1:
                    print("OP_RET_NC, NONE, NONE", end="")
                elif p == 2:
                    print("OP_LD8, ADDRESS_8BIT, REG_A", end="")  #NONE is (a8)
                elif p == 3:
                    print("OP_LD8, REG_A, ADDRESS_8BIT", end="") #NONE is (a8)
            else:
                if p == 0:
                    print("OP_RET_Z, NONE, NONE", end="")
                elif p == 1:
                    print("OP_RET_C, NONE, NONE", end="")
                elif p == 2:
                    print("OP_ADD16, REG_SP, RELATIVE_8BIT", end="")
                elif p == 3:
                    print("OP_LDHL, REG_HL, REG_SP", end="") #NONE is SP+r8, look into this
        elif z ==1:
            if q == 0:
                print("OP_POP, %s ,NONE"%(rp2[p]), end="")
            else:
                if p == 0:
                    print("OP_RET, NONE, NONE", end="")
                elif p == 1:
                    print("OP_RETI, NONE, NONE", end="")
                elif p == 2:
                    print("OP_JP, ADDRESS_REG_HL, NONE", end="")
                elif p == 3:
                    print("OP_LD16, REG_SP, REG_HL", end="")
        elif z ==2:
            if q == 0:
                if p == 0:
                    print("OP_JP_NZ, IMMEDIATE_16BIT, NONE", end="")
                elif p == 1:
                    print("OP_JP_NC, IMMEDIATE_16BIT, NONE", end="")
                elif p == 2:
                    print("OP_LD8, RELATIVE_REG_C, REG_A", end="")
                elif p == 3:
                    print("OP_LD8, REG_A, RELATIVE_REG_C", end="")
            else:
                if p == 0:
                    print("OP_JP_Z, IMMEDIATE_16BIT, NONE", end="") #NONE = a16
                elif p == 1:
                    print("OP_JP_C, IMMEDIATE_16BIT, NONE", end="") #NONE = a16
                elif p == 2:
                    print("OP_LD8, ADDRESS_16BIT, REG_A", end="") #NONE = (a16)
                elif p == 3:
                    print("OP_LD8, REG_A, ADDRESS_16BIT", end="") #NONE = (a16)
        elif z ==3:
            if y == 0:
                print("OP_JP, IMMEDIATE_16BIT, NONE", end="")    #first NONE = a16
            elif y == 1:
                print("OP_CBpref, NONE, NONE", end="")    #CB Prefix
            elif y == 6:
                print("OP_DI, NONE, NONE", end="")
            elif y == 7:
                print("OP_EI, NONE, NONE", end="")
            else:
                print("OP_ERROR, NONE, NONE", end="") #something went wrong if you get here
        elif z ==4:
            if y < 4:
                print("OP_CALL_%s, IMMEDIATE_16BIT, NONE"%(cc[y]), end="")
            else:
                print("OP_ERROR, NONE, NONE", end="")
        elif z ==5:
            if q == 0:
                print("OP_PUSH, %s, NONE"%(rp2[p]), end="")
            elif p == 0:
                print("OP_CALL, IMMEDIATE_16BIT, NONE", end="") #NONE = a16s
            else:
                print("OP_ERROR, NONE, NONE", end="") 
        elif z ==6:
            print("%s, REG_A, IMMEDIATE_8BIT"%(alu[y]), end="")
        elif z ==7:
            print("OP_RST, RESET_%s, NONE"%(y), end="") #the numarical value is an interger, not a pointer. look into this.

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
        print("%s, 0, %s"%(rot[y], r[z]), end="")
    elif x ==1:
        print("OP_BIT, %s, %s"%(y, r[z]), end="")
    elif x ==2:
        print("OP_RES, %s, %s"%(y, r[z]), end="")
    elif x ==3:
        print("OP_SET, %s, %s"%(y, r[z]), end="")

main()
