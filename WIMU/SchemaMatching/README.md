# Schema Matching

## TA said

- train & test data
  - divide dataset into training and testing by myself
- correctness
  - F1

## problems

1. language issues

   - 簡體與繁體
   - english

## tag study

### pair1

```.txt
建物名稱  1   name
名稱      1   name
地址      2   location  contain words "市/區/路/街/巷/縣"
總價      3   number
價格      3   number
單價      4   number    is less than 總價
每坪單價  4   number    is less than 價格
格局      5   string    is a description
建坪      6   number    單價 * 建坪 = 總價, a number that former has a count for something per this
總坪數    6   number    每坪單價 * 總坪數 = 價格, a number that former has a count for something per this
屋齡      7   number
類型      8   type      is a type that simple words appeared frequently
樓層      9   number
建物朝向  10  string
座向      10  string
管理費    11  number
url       12  url     contain http/https/www at beginning
```

### after tag study

3 ways:

1. use all the pairs to train directly with indexes, and get output probability as similarity of inner content
2. pre-tagging to my data type and train the paired data with my data type's tag, and get output probability as similarity of inner content
3. pre-tag some that can identify directly by people, and use originally paired and my pairs together to train my model, and get output probability as similarity of inner content

## Final way

- [ ] use word2vec to pre-classify tags (`FAILED` the tags not the words in the model)
- [x] use bert to train (`KIND OF FAILED` I use the english model to train chinese things)
- [x] use model to test original train data (`F1?`)


## Notes

### spacy

- use for 文本分析
- 句子相似度比較
  ```python
  # https://spacy.io/api/doc#similarity
  apples = nlp("I like apples")
  oranges = nlp("I like oranges")
  apples_oranges = apples.similarity(oranges)
  oranges_apples = oranges.similarity(apples)
  assert apples_oranges == oranges_apples
  ```

<!-- ## dataset storing type

- a data type: (data, type)
  - data: content of that
  - type: type like number, string, url, data ...
-

## target put the similar content together(nothing to their label)

- one pair to one training or testing data, and combine them together -->

