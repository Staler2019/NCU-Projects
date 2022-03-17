import queue
from bs4 import BeautifulSoup
import time
import requests
import json
import re
import codecs
from six import u
import threading
import json
from pathlib import Path
import hashlib


# def write_test_to_file(txt):
#     OUT_FILE = "./crawler_output.txt"
#     f = open(OUT_FILE, "w", encoding="UTF-8")
#     f.write(txt)
#     f.close()


def remove_edit_btn(text):  # remove '[編輯]'
    return text[:-4]


def inner_breakable(soup_item):
    # break point: find "參考" or "連結" in h2
    if soup_item.name == "h2" and (
        "參考" in soup_item.text or "連結" in soup_item.text
    ):
        return True
    return False


def inner_continuable(soup_item):
    # None this item
    if soup_item.name is None:
        return True
    elif soup_item.name == "style":
        return True
    # tables
    elif soup_item.name == "table":
        return True
    # elif (
    #     soup_item.name == "table"
    #     and soup_item["class"]
    #     == "mbox-small plainlinks sistersitebox"
    # ):
    #     return True
    elif soup_item.find("div", {"class": "toctitle"}):
        return True
    # 數學會有亂碼
    elif soup_item.find(
        "span", {"class": "mwe-math-element"}
    ):
        return True
    return False


class CrawlerDataWorker(threading.Thread):
    def __init__(self, queue, lock):
        super().__init__()
        self.queue = queue
        self.breakSign = False
        self.lock = lock

    def write_json_to_file(self, js):
        self.lock.acquire()
        # js = json.loads(_json)
        # print(_json)

        # TODO check HASH IN HASH_FILE exists OR NOT
        with open(
            HASH_TABLE_FILE, "a", encoding="UTF-8"
        ) as f:
            print(js["hash"], file=f)

        with open(
            f"{DATA_FOLDER}/{js['hash']}.json",
            "w",
            encoding="UTF-8",
        ) as f:
            print(js, file=f)

        self.lock.release()

    def break_down(self):
        self.breakSign = True

    def my_parser(self, resp):
        # use bs4 to see parser html
        url = resp.url

        soup = BeautifulSoup(resp.text, "html.parser")

        # title
        title = soup.h1.text
        # get sha256
        shaObj = hashlib.sha256()
        shaObj.update(title.encode("utf-8"))
        sha = shaObj.hexdigest()

        # inner text
        content = ""
        my_text = soup.find(
            "div", {"class": "mw-parser-output"}
        )
        for my_tt in my_text:
            if inner_continuable(my_tt):
                continue
            if inner_breakable(my_tt):
                break

            if (
                my_tt.name == "h2"
                or my_tt.name == "h3"
                or my_tt.name == "h4"
                or my_tt.name == "h5"
                or my_tt.name == "h6"
            ):
                content = content + remove_edit_btn(
                    my_tt.text
                )
            else:
                content = content + my_tt.text

        _json = {
            "url": url,
            "hash": sha,
            "title": title,
            "content": content,
        }

        self.write_json_to_file(_json)

    def run(self):
        while True:
            if self.queue.qsize() > 0:
                resp = self.queue.get()
                self.my_parser(resp)

            else:
                if self.breakSign:
                    break

            time.sleep(1)


def to_url(url):
    resp = requests.get(
        url=url,
        verify=True,
        timeout=3,
    )
    if resp.status_code != 200:
        print("Invalid url:", resp.url)
    return resp


def redirect(url):
    new_url_tmp = url.split("/")
    new_url_tmp[3] = "zh-tw"
    new_url = new_url_tmp[0]
    for i in range(1, len(new_url_tmp)):
        new_url = new_url + "/" + new_url_tmp[i]
    return new_url


class Crawler:
    def __init__(self, queue):
        super().__init__()
        self.queue = queue

    def run(self, times):
        for i in range(times):
            print(i)
            self.new_page()
            time.sleep(3)

    def new_page(self):
        # go to random page
        resp = to_url(
            "https://zh.wikipedia.org/wiki/Special:%E9%9A%8F%E6%9C%BA%E9%A1%B5%E9%9D%A2"
        )
        # redirect to page in "台灣正體"
        url = redirect(resp.url)
        resp = to_url(url)

        # put to queue
        self.queue.put(resp)


if __name__ == "__main__":
    # file using
    HASH_TABLE_FILE = "./hash_history.txt"
    DATA_FOLDER = "./data"
    Path(DATA_FOLDER).mkdir(
        parents=True, exist_ok=True
    )  # create folder if not exist


    # threading declaring
    crawler_queue = queue.Queue()
    crawler_lock = threading.Lock()
    worker1 = CrawlerDataWorker(crawler_queue, crawler_lock)
    worker2 = CrawlerDataWorker(crawler_queue, crawler_lock)
    worker1.start()
    worker2.start()

    # main declaring
    crawler = Crawler(crawler_queue)
    crawler.run(times=10000)

    # threading breakdown
    worker1.break_down()
    worker2.break_down()

    worker1.join()
    worker2.join()
