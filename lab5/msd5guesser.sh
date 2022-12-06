WORDS=`cat guess_words.txt`
NUMS=`cat guess_nums.txt`

for w in $WORDS; do
    for n in $NUMS; do
        echo "$w$n"
        echo $w$n | md5sum
    done
done