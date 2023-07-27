n=4
if [ $# -eq 1 ]
  then
    n=$1
fi
echo "Running for $n folders"

for i in $(seq 1 $n)
do
    printf "printing for scene of folder $i\n"
    file1="./$i/scene.txt"
    file2="stage1.txt"
    file3="stage2.txt"
    file4="stage3.txt"
    g++ main.cpp -o main
    ./main $file1
    python3 main.py ./$i/$file2 $file2
    python3 main.py ./$i/$file3 $file3
    python3 main.py ./$i/$file4 $file4
done
