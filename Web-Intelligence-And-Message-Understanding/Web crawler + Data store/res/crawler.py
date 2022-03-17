from bs4 import BeautifulSoup
import time
import requests
from pathlib import Path
import hashlib
import json
import re
from six import u

TIME_WAITING_PERIOD = 0.5
DATA_FOLDER = "./data"
HASH_TABLE_FILE = "./hash_table.txt"
HASH_TABLE = []


def crawler_each():
    # random a page
    resp = to_url(
        "https://zh.wikipedia.org/wiki/Special:%E9%9A%8F%E6%9C%BA%E9%A1%B5%E9%9D%A2"
    )
    # redirect to page in "台灣正體"
    url = resp.url
    new_url_tmp = url.split("/")
    new_url_tmp[3] = "zh-tw"
    new_url = new_url_tmp[0]
    for i in range(1, len(new_url_tmp)):
        new_url = new_url + "/" + new_url_tmp[i]

    time.sleep(TIME_WAITING_PERIOD)
    resp = to_url(new_url)

    js = parse(resp)
    write_json_to_file(js)


def to_url(url):
    resp = requests.get(
        url=url,
        verify=True,
        timeout=3,
    )
    if resp.status_code != 200:
        str = "Invalid url: {}".format(resp.url)
        # print_to_logger("a", str)
        print(str)
    return resp


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


def write_json_to_file(js):
    with open(
        f"{DATA_FOLDER}/{js['hash']}.json",
        "w",
        encoding="UTF-8",
    ) as f:
        json.dump(js, f)

    if js["hash"] not in HASH_TABLE:
        HASH_TABLE.append(js["hash"])

        # hash_table
        with open(
            HASH_TABLE_FILE, "a", encoding="UTF-8"
        ) as f:
            print(js["hash"], file=f)


def content_filter(content):
    expr = re.compile(
        u(
            r"[^\u4e00-\u9fa5\u3002\uff1b\uff0c\uff1a\u201c\u201d\uff08\uff09\u3001\uff1f\u300a\u300b\s\w:/-_.?~%()]"
        )
    )

    filtered = re.sub(expr, "", content)
    filtered = [_s for _s in filtered.split("\n")]
    content = " ".join(filtered)
    content = re.sub(r"(\s)+", " ", content)

    return content


def parse(resp):
    # use bs4 to see parser html
    url = resp.url

    soup = BeautifulSoup(resp.text, "html.parser")

    # title
    title = soup.h1.text
    # print_to_logger("a", title)
    print(title)
    # get sha256
    sha_obj = hashlib.sha256()
    sha_obj.update(title.encode("utf-8"))
    sha = sha_obj.hexdigest()

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
            content = content + remove_edit_btn(my_tt.text)
        else:
            content = content + my_tt.text

    content = content_filter(content)

    _json = {
        "url": url,
        "hash": sha,
        "title": title,
        "content": content,
    }

    # print(_json)

    return _json


def crawler(times, stop):
    for i in range(times):
        str = "{}".format(i + 1)
        # print_to_logger("a", str, end=" ")
        print(str, end=" ")
        while True:
            try:
                crawler_each()
                time.sleep(TIME_WAITING_PERIOD)
                break
            except requests.exceptions.ReadTimeout:
                pass
            except requests.exceptions.ConnectionError:
                pass
            time.sleep(TIME_WAITING_PERIOD)
        if stop():
            return
        if (i + 1) % 3 == 0:
            time.sleep(TIME_WAITING_PERIOD * 4)


def crawler_run(times, stop):
    # create folder if not exist
    Path(DATA_FOLDER).mkdir(parents=True, exist_ok=True)

    try:
        # read hash table file
        with open(
            HASH_TABLE_FILE, "r", encoding="UTF-8"
        ) as f:
            HASH_TABLE = [
                line[:-1] for line in f.readlines()
            ]
            str = "Found {} data in history".format(
                len(HASH_TABLE)
            )
            # print_to_logger("w", str)
            print(str)
    except IOError:
        str = f"{HASH_TABLE_FILE} is not exist! Then it will be created this time."
        # print_to_logger("w", str)
        print(str)

    crawler(times, stop)


if __name__ == "__main__":
    stop_thread = False
    crawler_run(10000, lambda: stop_thread)
