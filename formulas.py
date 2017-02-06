def formulas(parsed):
    l = len(parsed)
    pairs = l-1
    if parsed[0] == 'goto':
        for i in range(1,pairs):
            if parsed[i] == 'A' or parsed[i] == 'a':
                valueA = int(parsed[i+1])
                countsA = str(int(valueA/1.25))
            if parsed[i] == 'X' or parsed[i] == 'x':
                valueX = int(parsed[i+1])
                countsX = str(int(valueX/1.25))
            if parsed[i] == 'Y' or parsed[i] == 'y':
                valueY = int(parsed[i+1])
                countsY = str(int(valueY/1.25))
            if parsed[i] == 'Z' or parsed[i] == 'z':
                valueZ = int(parsed[i+1])
                countsZ = str(int(valueZ/1.25))
            if parsed[i] == 'B' or parsed[i] == 'b':
                valueB = int(parsed[i+1])
                countsB = str(int(valueB/1.25))
            if parsed[i] == 'C' or parsed[i] == 'c':
                valueC = int(parsed[i+1])
                countsC = str(int(valueC/1.25))
            if parsed[i] == 'S' or parsed[i] == 's':
                valueS = int(parsed[i+1])
                countsS = str(int(valueS/5))
                
                # str2 = 'X1Y8,' + countsS + '\r'
                # return "str %s" % str2

#print formulas(['goto', 'x', '2342', 's', '345'])
