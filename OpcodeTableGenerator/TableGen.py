#how do we write python code again?
x = 0
y = 0
z = 0
p = 0
q = 0

#replace these letters with the propper values later
r = ["B", "C", "D", "E", "H", "L", "(HL)", "A"]
rp = ["BC", "DE", "HL", "SP"]
rp2 = ["BC", "DE", "HL", "AF"]
cc = ["NZ", "Z", "NC", "C", "PO", "PE", "P", "M"]
alu = ["_ADD", "_ADC", "_SUB", "_SBC", "_AND", "_XOR", "_OR", "_CP"]
rot = ["RLC", "RRC", "RL", "RR", "SLA", "SRA", "SLL", "SRL"]

for opcode in range(0, 255):
    #get the letter values for the opcode
    x = (opcode & 0b11000000) >> 6
    y = (opcode & 0b00111000) >> 3
    z = (opcode & 0b00000111)
    p = (opcode & 0b00110000) >> 4
    q = (opcode & 0b00001000) >> 3

    #find x
    if x == 0:
        if z == 0:
            if y == 0:
                print("_NOP, NULL, NULL")
            elif y == 1:
                pass
            elif y == 2:
                print("_STOP, NULL, NULL")
            elif y == 3:
                print("_JR, NULL, NULL")
            else:
                print("_JR, %s, NULL"% cc[y-4])
        elif z ==1:
            if q == 0:
                print("_LL16, %s, NULL"% rp[p])
            else:
                print("_ADD16, HL, %s"% rp[p])
        elif z ==2:
            if q == 0:
                if p == 0:
                    print("_LL8, (BC), A")
                elif p == 1:
                    print("_LL8, (DE), A")
                elif p == 2:
                    print("_LDI, (HL), A")
                elif p == 3:
                    print("_LDD, (HL), A")
            else:
                if p == 0:
                    print("_LL8, A, (BC)")
                elif p == 1:
                    print("_LL8, A, (DE)")
                elif p == 2:
                    print("_LDI, A, (HL)")
                elif p == 3:
                    print("_LDD, A, (HL)")
        elif z ==3:
            if q == 0:
                print("_INC16, %s, NULL"% rp[p])
            else:
                print("_DEC16, %s, NULL"% rp[p])
        elif z ==4:
            print("_INC8, %s, NULL"% r[y])
        elif z ==5:
            print("_DEC, %s, NULL"% r[y])
        elif z ==6:
            print("_LL8, %s, NULL"% r[y])
        elif z ==7:
            if y == 0:
                print("_RLCA, NULL, NULL")
            elif y == 1:
                print("_RRCA, NULL, NULL")
            elif y == 2:
                print("_RLA, NULL, NULL")
            elif y == 3:
                print("_RRA, NULL, NULL")
            elif y == 4:
                print("_DAA, NULL, NULL")
            elif y == 5:
                print("_CPL, NULL, NULL")
            elif y == 6:
                print("_SCF, NULL, NULL")
            elif y == 7:
                print("_CCF, NULL, NULL")
             
    elif x == 1:
        if z == 6:
            print("_HALT, NULL, NULL")
        else:
            print("_LD8, %s, %s"% (r[y], r[z]))
    elif x == 2:
        print("%s, %s, NULL"% (alu[y], r[z]))
    elif x == 3:
        if z == 0:
            if q == 0:
                if p == 0:
                    print("_RET, NZ, NULL")
                elif p == 1:
                    print("_RET, NC, NULL")
                elif p == 2:
                    print("_LDH, NULL, A")
                elif p == 3:
                    print("_LDH, A, NULL")
            else:
                if p == 0:
                    print("_RET, Z, NULL")
                elif p == 1:
                    print("_RET, C, NULL")
                elif p == 2:
                    print("_ADDL, SP, NULL")
                elif p == 3:
                    print("_LLSP, HL, NULL")
        elif z ==1:
            if q == 0:
                pass
            else:
                if p == 0:
                    pass
                elif p == 1:
                    pass
                elif p == 2:
                    pass
                elif p == 3:
                    pass
        elif z ==2:
            if q == 0:
                if p == 0:
                    pass
                elif p == 1:
                    pass
                elif p == 2:
                    pass
                elif p == 3:
                    pass
            else:
                if p == 0:
                    pass
                elif p == 1:
                    pass
                elif p == 2:
                    pass
                elif p == 3:
                    pass 
        elif z ==3:
            if y == 0:
                pass
            elif y == 1:
                pass
            elif y == 6:
                pass
            elif y == 7:
                pass
            else:
                pass #something went wrong if you get here
        elif z ==4:
            pass
        elif z ==5:
            if q == 0:
                pass
            else:
                pass  
        elif z ==6:
            pass 
        elif z ==7:
            pass 