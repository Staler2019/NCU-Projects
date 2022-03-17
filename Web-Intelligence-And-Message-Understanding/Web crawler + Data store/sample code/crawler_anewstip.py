from selenium import webdriver
from selenium.webdriver import Chrome
from selenium.webdriver.chrome.options import Options
from bs4 import BeautifulSoup
import time

chrome_options = webdriver.ChromeOptions()
# 添加 User-Agent
chrome_options.add_argument(
    'user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36"')

# 指定瀏覽器解析度
chrome_options.add_argument('window-size=1920x1080')
# 不載入圖片，提升速度
chrome_options.add_argument('blink-settings=imagesEnabled=false')
# 瀏覽器不提供視覺化頁面
# chrome_options.add_argument('--headless')
# 禁用 JavaScript
chrome_options.add_argument("--disable-javascript")

# 禁用瀏覽器彈出視窗
prefs = {
    'profile.default_content_setting_values': {
        'notifications': 2
    }
}
chrome_options.add_experimental_option('prefs', prefs)

driver = Chrome(chrome_options=chrome_options)

# 存取 Website
driver.get('https://anewstip.com/search/tweets/?q=artificial+intelligence')
# 等待 5 秒鐘以載入頁面
time.sleep(5)

# 點擊連結
driver.find_element_by_link_text("Next").click()

# 取得網頁原始碼
html = driver.page_source

# 解析下一頁的 html
soup = BeautifulSoup(html, 'html.parser')
for target_tag in soup.find_all('span', {'class': 'info-name'}):
    name = target_tag.a.text
    print('name: {}'.format(name))

driver.close()