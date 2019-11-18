#how do we write python code again?

#Setup some global values
x = 0
y = 0
z = 0
p = 0
q = 0

NonPrefixed = False
CBPrefixed = False
Parenthesies = False
Commas = False
Curlies = False

#replace these letters with the propper values later
r = ["B", "C", "D", "E", "H", "L", "(HL)", "A"]
rp = ["BC", "DE", "HL", "SP"]
rp2 = ["BC", "DE", "HL", "AF"]
cc = ["NZ", "Z", "NC", "C", "PO", "PE", "P", "M"]
alu = ["_ADD", "_ADC", "_SUB", "_SBC", "_AND", "_XOR", "_OR", "_CP"]
rot = ["_RLC", "_RRC", "_RL", "_RR", "_SLA", "_SRA", "_SLL", "_SRL"]

#Validate user input
import sys
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
        elif str(sys.argv[arg]) == "-par":
            Parenthesies = True
            Curlies = False
        elif str(sys.argv[arg]) == "-curl":
            Curlies = True
            Parenthesies = False


#Start of user code
#Man this is badly structured...
for opcode in range(0, 256):
    #get the letter values for the opcode
    x = (opcode & 0b11000000) >> 6
    y = (opcode & 0b00111000) >> 3
    z = (opcode & 0b00000111)
    p = (opcode & 0b00110000) >> 4
    q = (opcode & 0b00001000) >> 3

    if NonPrefixed:
        if Parenthesies:
            print("( ", end="")
        elif Curlies:
            print("{ ", end = "")
        #find x
        if x == 0:
            if z == 0:
                if y == 0:
                    print("_NOP, NULL, NULL", end="")
                elif y == 1:
                    print("LD, SP, NULL", end="")
                elif y == 2:
                    print("_STOP, NULL, NULL", end="")
                elif y == 3:
                    print("_JR, NULL, NULL", end="")
                else:
                    print("_JR, %s, NULL"% cc[y-4], end="")
            elif z ==1:
                if q == 0:
                    print("_LL16, %s, NULL"% rp[p], end="")
                else:
                    print("_ADD16, HL, %s"% rp[p], end="")
            elif z ==2:
                if q == 0:
                    if p == 0:
                        print("_LL8, (BC), A", end="")
                    elif p == 1:
                        print("_LL8, (DE), A", end="")
                    elif p == 2:
                        print("_LDI, (HL), A", end="")
                    elif p == 3:
                        print("_LDD, (HL), A", end="")
                else:
                    if p == 0:
                        print("_LL8, A, (BC)", end="")
                    elif p == 1:
                        print("_LL8, A, (DE)", end="")
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
                print("_DEC, %s, NULL"% r[y], end="")
            elif z ==6:
                print("_LL8, %s, NULL"% r[y], end="")
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
            if z == 6:
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
                        print("_LDH, NULL, A", end="")  #NULL is (r8)
                    elif p == 3:
                        print("_LDH, A, NULL", end="") #NULL is (r8)
                else:
                    if p == 0:
                        print("_RET, Z, NULL", end="")
                    elif p == 1:
                        print("_RET, C, NULL", end="")
                    elif p == 2:
                        print("_ADDL, SP, NULL", end="")
                    elif p == 3:
                        print("_LLSP, HL, NULL", end="")
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
                        print("_LD, SP, HL", end="")
            elif z ==2:
                if q == 0:
                    if p == 0:
                        print("_JP, NZ, NULL", end="")
                    elif p == 1:
                        print("_JP, NC, NULL", end="")
                    elif p == 2:
                        print("_LD, (C), A", end="")
                    elif p == 3:
                        print("_LD, A, (C)", end="")
                else:
                    if p == 0:
                        print("_JP, Z, NULL", end="") #NULL = a16
                    elif p == 1:
                        print("_JP, C, NULL", end="") #NULL = a16
                    elif p == 2:
                        print("_LD, NULL, A", end="") #NULL = a16
                    elif p == 3:
                        print("_LD, A, NULL", end="") #NULL = a16
            elif z ==3:
                if y == 0:
                    print("_JP, NULL, NULL", end="")    #first NULL = a16
                elif y == 1:
                    print("_CB, NULL, NULL", end="")    #CB Prefix
                elif y == 6:
                    print("_DI, NULL, NULL", end="")
                elif y == 7:
                    print("_EI, NULL, NULL", end="")
                else:
                    print("_ERROR, NULL, NULL", end="") #something went wrong if you get here
            elif z ==4:
                print("_CALL, %s, NULL"%(cc[y]), end="")
            elif z ==5:
                if q == 0:
                    print("_PUSH, %s, NULL"%(rp2[p]), end="")
                else:
                    print("_CALL, NULL, NULL", end="")  #NULL = a16
            elif z ==6:
                print("%s, d8, NULL"%(alu[y]), end="")
            elif z ==7:
                print("_RST, %s, NULL"%(y * 8), end="") #the numarical value is an interger, not a pointer

        if Parenthesies:
            print(" )",  end = "")
        elif Curlies:
            print(" }", end = "")
        if Commas:
            print(",", end="")
        print("")

    #==============
    # CB Prefixed Opcodes
    #==============
for opcode in range(0, 256):
    #get the letter values for the opcode
    x = (opcode & 0b11000000) >> 6
    y = (opcode & 0b00111000) >> 3
    z = (opcode & 0b00000111)
    p = (opcode & 0b00110000) >> 4
    q = (opcode & 0b00001000) >> 3
#real wonkey code this right here



    if CBPrefixed:
        if Parenthesies:
            print("( ", end = "")
        elif Curlies:
            print("{ ", end = "")

        if x == 0:
            print("%s, %s, NULL"%(rot[y], r[z]), end="")
        elif x ==1:
            print("_BIT, %s, %s"%(y, r[z]), end="")
        elif x ==2:
            print("_RES, %s, %s"%(y, r[z]), end="")
        elif x ==3:
            print("_SET, %s, %s"%(y, r[z]), end="")

        if Parenthesies:
            print(" )", end = "")
        elif Curlies:
            print(" }", end = "")
        if Commas:
            print(",", end="")
        print("")
