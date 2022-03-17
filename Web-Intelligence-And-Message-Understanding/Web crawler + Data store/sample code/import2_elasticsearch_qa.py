# -*- coding: utf-8 -*-
# @Author : christy051424@gmail.com
# @Email : christy051424@gmail.com
# @File : import2_elasticsearch_qa.py
# @Software: PyCharm

from elasticsearch import Elasticsearch, helpers
import json

qa_mapping = {
    "properties": {
        "question_type": {"type": "keyword"},
        "asin": {"type": "text"},
        "answer_time": {"type": "text"},
        "unix_time": {"type": "date"},
        "question": {"type": "text", "analyzer": "english"},
        "answer_type": {"type": "keyword"},
        "answer": {"type": "text"},
    }
}

renames_key = {
    "questionType": "question_type",
    "asin": "asin",
    "answerTime": "answer_time",
    "unixTime": "unix_time",
    "question": "question",
    "answerType": "answer_type",
    "answer": "answer",
}

# Load Amazon QA dataset
def read_data():
    with open("qa_Appliances.json", "r") as f:
        for row in f:
            d = eval(row.strip())
            d = json.dumps(d)
            row = json.loads(d)

            for k, v in renames_key.items():
                for old_name in list(row):
                    if k == old_name:
                        row[v] = row.pop(old_name)
            yield row


def load2_elasticsearch():
    index_name = "amazon_qa"
    type = "one_to_one"
    es = Elasticsearch()

    # Create Index
    if not es.indices.exists(index=index_name):
        es.indices.create(index=index_name)
    print("Index created!")

    # Put mapping into index
    if not es.indices.exists_type(
        index=index_name, doc_type=type
    ):
        es.indices.put_mapping(
            index=index_name,
            doc_type=type,
            body=qa_mapping,
            include_type_name=True,
        )
    print("Mappings created!")

    # Import data to elasticsearch
    success, _ = helpers.bulk(
        client=es,
        actions=read_data(),
        index=index_name,
        doc_type=type,
        ignore=400,
    )
    print("success: ", success)


load2_elasticsearch()
