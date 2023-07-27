import sys

def compare_files(file1, file2):
    equal = True
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        lines1 = f1.readlines()
        lines2 = f2.readlines()

        # Compare line by line
        for line1, line2 in zip(lines1, lines2):
            line1.strip() 
            line2.strip()
            words1 = line1.split()
            words2 = line2.split()
            for word1, word2 in zip(words1, words2):
                word1 = word1.strip()
                word2 = word2.strip()
                word1 = word1[:word1.index(".")+1] + word1[word1.index(".")+1:len(word1)-2]
                word2 = word2[:word2.index(".")+1] + word2[word2.index(".")+1:len(word2)-1]
                num1 = float(word1)
                num2 = float(word2)
                if num1 != num2:
                    print("num1: ", word1, "num2: ", word2)
                    equal = False

        # Check if both files have the same number of lines
        if len(lines1) != len(lines2):
            print("len(lines1): ", len(lines1), "len(lines2): ", len(lines2))
            equal = False

        return equal

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Please provide two file paths as arguments.")
        sys.exit(1)

    file1 = sys.argv[1]
    file2 = sys.argv[2]

    if compare_files(file1, file2):
        print("Files have the same content.")
    else:
        print("Files do not have the same content.")
