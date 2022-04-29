from src.crawler import crawler_run
from src.load import load2_elasticsearch
from src.search import query


if __name__ == "__main__":
    action = input(
        "Which do you want to do? Crawler(c), Load(l), or Search(s)?"
    )
    if action == "c":
        stop_action = False
        crawler_run(10000, lambda: stop_action)
    elif action == "l":
        load2_elasticsearch()
    elif action == "s":
        query(input("請輸入您要查詢的關鍵字?"))
