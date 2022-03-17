# -*- coding: utf-8 -*-
# @Author : christy051424@gmail.com
# @Email : christy051424@gmail.com
# @File : query_elasticsearch_qa.py
# @Software: PyCharm

from elasticsearch import Elasticsearch
import json


def query(query):
    es = Elasticsearch()

    index_name = "amazon_qa"
    type = "one_to_one"

    # Query DSL
    search_params = {
        "query": {"match": {"question": query}},
        "size": 5,
    }
    # Search document
    result = es.search(
        index=index_name, doc_type=type, body=search_params
    )
    result = result["hits"]["hits"][0]

    result = json.dumps(result, indent=2)
    print(result)


query(
    "will this Badger 1 install just like the original one?"
)
