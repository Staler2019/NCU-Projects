from elasticsearch import Elasticsearch
import json
from pathlib import Path
from datetime import datetime

import elasticsearch


ANS_DIR = "./answer"


def query(_query):
    # create folder if not exist
    Path(ANS_DIR).mkdir(parents=True, exist_ok=True)

    try:
        es = Elasticsearch()

        index_name = "wiki_qa"
        type = "one_to_one"

        # Query DSL
        search_params = {
            "query": {
                "multi_match": {
                    "query": _query,
                    "fields": ["title", "content"],
                }
            }
        }
        # Search document
        result = es.search(
            index=index_name,
            doc_type=type,
            body=search_params,
        )
        # result = result["hits"]["hits"][0]
        result = result["hits"]["hits"]

        if result[0] is None:
            print("查無資料")
            exit()

        now = datetime.now()
        dt = now.strftime("%y-%m-%d-%H-%M-%S")
        with open(
            f"{ANS_DIR}/{dt}_query_{_query}.json",
            "w",
            encoding="UTF-8",
        ) as f:
            result = json.dumps(
                result, indent=2, ensure_ascii=False
            )
            print(result)
            print(result, file=f)

    except elasticsearch.exceptions.ConnectionError:
        print(
            "Elasticsearch connection error! Probably it didn't start!"
        )


if __name__ == "__main__":
    query(input("請輸入您要查詢的關鍵字?"))
