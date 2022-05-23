import yake
import jieba
import pandas as pd
from pathlib import Path
import re

from engines.configure import Configure

TEST_FILE = "../data/test.csv"


if __name__ == "__main__":
    # config = Configure()
    reg = re.compile("[/\n]")
    # yake setting: can add self stopword
    kw_extractor = yake.KeywordExtractor(lan="zh", n=1, top=10)

    df = pd.read_csv(TEST_FILE).astype(str)
    df.dropna()
    # tmp_df = pd.DataFrame(df[df.columns[1]].astype(str))

    predict = {"title": [], "content": [], "keywords": []}
    success = 0
    for row in df.index:
        # print(row)
        predict["title"].append(df[df.columns[0]][row])

        text = df[df.columns[1]][row]
        text = re.sub(reg, " ", text)
        # print(text)
        predict["content"].append(text)

        # yake: take keyword after tokenization
        text = " ".join(list(jieba.cut_for_search(text)))
        keywords = kw_extractor.extract_keywords(text)
        predict["keywords"].append([keyword[0] for keyword in keywords])

    df_predict = pd.DataFrame(predict)
    Path("./predict").mkdir(parents=True, exist_ok=True)
    df_predict.to_csv("./predict/test.csv")

    print(f"Total: {len(df.index)}")
    print("succeed!")
