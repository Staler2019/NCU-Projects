import threading
import requests
from bs4 import BeautifulSoup
from selenium.webdriver import Firefox, FirefoxOptions
from selenium.webdriver.common.by import By
import queue
from datetime import date
import csv


# def to_url(url):
#     resp = requests.get(
#         url=url,
#         verify=True,
#         timeout=3,
#     )
#     if resp.status_code != 200:
#         str = "Invalid url: {}".format(resp.url)
#         # print_to_logger("a", str)
#         print(str)
#     return resp


# class CrawlerWorker(threading.Thread):
#     def __init__(self, queue):
#         super().__init__()
#         self.queue = queue

#     def run(self, status):
#         while not status:
#             pass  # TODO crawler


class Crawler:
    # def __init__(self, url_queue):
    def __init__(self):
        super().__init__()
        # self.queue = url_queue
        # self.urls = []
        self.list_url = "https://news.ltn.com.tw/list/breakingnews"

    def setTime(self):
        self.today = date.today()

    def run(self, times):
        option = FirefoxOptions()
        option.set_preference("javascript.enabled", False)
        browser = Firefox(options=option)
        browser.get(self.list_url)

        self.setTime()

        with open("liberty_times.csv", "w", encoding="UTF-8", newline="") as f:
            csv_header = [
                "tag",
                "link",
                "media_thumbnail",
                "title",
                "published_time",
            ]
            writer = csv.DictWriter(f, fieldnames=csv_header)
            writer.writeheader()

            counter = 1
            while times - counter + 1 > 0:
                element = browser.find_element(
                    By.XPATH, f"/html/body/div[8]/section/div[3]/ul/li[{counter}]"
                )

                time = element.find_element(By.CLASS_NAME, "time").text
                if len(time) <= 5:
                    time = self.today.strftime("%Y/%m/%d ") + time
                link = element.find_element(By.CLASS_NAME, "ph").get_attribute("href")
                tag = link.split("/")[4]
                # print(tag)

                to_queue = {
                    "tag": tag,
                    "link": link,
                    "media_thumbnail": element.find_element(
                        By.TAG_NAME, "img"
                    ).get_attribute("data-src"),
                    "title": element.find_element(By.CLASS_NAME, "ph").get_attribute(
                        "title"
                    ),
                    "published_time": time,
                }

                # print(to_queue)
                # to csv
                writer.writerow(to_queue)
                counter += 1
        browser.close()

        return False


if __name__ == "__main__":
    # status = True
    # que = queue.Queue()
    # worker = CrawlerWorker(que, lambda: status)

    # crawler = Crawler(queue)
    crawler = Crawler()
    status = crawler.run(times=20) # TODO i haven't setup function of page down, so the maximum of crawler is 20

    # worker.join()

    # TODO 2 things:
    # 1. page down function
    # 2. crawler the content
