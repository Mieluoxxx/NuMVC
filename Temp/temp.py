with open("data/raw/C125.9.clq", "r") as fin:
    with open("data/processed/C125.9.txt", "w") as fout:
        for line in fin:
            if line.startswith("p"):
                # Assuming the line format is "p col 125 6963"
                _, _, num1, num2 = line.split()
                fout.write("%s %s\n" % (num1, num2))
            elif line.startswith("e"):
                # Assuming the line format is "e 2 1"
                _, num1, num2 = line.split()
                fout.write("%s %s\n" % (num1, num2))


