# use chrome driver
# class Crawler:
#     def __init__(self):
#         self.set_up_arg()

#     def run(self, url, times):  # websites: list
#         # self.driver.get(url)
#         # 等待 5 秒鐘以載入頁面
#         # time.sleep(3)
#         for _ in range(times):
#             self.run_for_each()

#     def set_up_arg(self):
#         chrome_options = webdriver.ChromeOptions()
#         # 添加 User-Agent
#         chrome_options.add_argument(
#             'user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36"'
#         )

#         # 指定瀏覽器解析度
#         chrome_options.add_argument("window-size=1920x1080")
#         # 不載入圖片，提升速度
#         chrome_options.add_argument(
#             "blink-settings=imagesEnabled=false"
#         )
#         # 瀏覽器不提供視覺化頁面
#         # chrome_options.add_argument('--headless')
#         # 禁用 JavaScript
#         chrome_options.add_argument("--disable-javascript")

#         # 禁用瀏覽器彈出視窗
#         prefs = {
#             "profile.default_content_setting_values": {
#                 "notifications": 2
#             }
#         }
#         chrome_options.add_experimental_option(
#             "prefs", prefs
#         )

#         self.driver = Chrome(chrome_options=chrome_options)

#     def remove_edit(self, text):  # remove '[編輯]'
#         return text[:-4]

#     def run_for_each(self):
#         # 存取 Website
#         # 點擊連結
#         self.driver.get(
#             "https://zh.wikipedia.org/wiki/Special:%E9%9A%8F%E6%9C%BA%E9%A1%B5%E9%9D%A2"
#         )
#         time.sleep(2)
#         self.driver.find_element(
#             by=By.LINK_TEXT, value="臺灣正體"
#         ).click()

#         # 取得網頁原始碼
#         print(self.driver.current_url)
#         html = self.driver.page_source

#         # 解析下一頁的 html
#         soup = BeautifulSoup(html, "html.parser")
#         h1 = self.remove_edit(soup.h1.text)
#         print(h1)

#         my_text = soup.find(
#             "div", {"class": "mw-parser-output"}
#         )
#         for my_tt in my_text:
#             if (
#                 my_tt.name == "p"
#                 or my_tt.name == "h2"
#                 or my_tt.name == "h3"
#             ):
#                 if (
#                     my_tt.name == "h2"
#                     and "參考" in my_tt.text
#                 ):
#                     break
#                 if my_tt.text == "":
#                     continue
#                 if my_tt.find(
#                     "span", {"class": "mwe-math-element"}
#                 ):  # 數學會有亂碼
#                     continue

#                 # print
#                 print(my_tt.name)
#                 if my_tt.name == "h2":
#                     print(self.remove_edit(my_tt.text))
#                 else:
#                     print(my_tt.text)

#         # for target_tag in soup.find_all(
#         #     "span", {"class": "info-name"}
#         # ):
#         #     name = target_tag.a.text
#         #     print("name: {}".format(name))

#     def close(self):
#         self.driver.close()
