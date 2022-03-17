from bs4 import BeautifulSoup
import time
import requests
import json
import re
import codecs
from six import u

def ptt_crawler():
    resp = requests.get(
        url = PTT_URL + '/bbs/Gossiping/index.html',
        cookies = {'over18': '1'},
        verify = True,
        timeout = 3
    )
    if resp.status_code != 200:
        print('Invalid url:', resp.url)

    soup = BeautifulSoup(resp.text,"html.parser")
    next_url = soup.select("div.btn-group.btn-group-paging a")
    page = next_url[1]["href"][20:25]
    
    resp = requests.get(
        url = PTT_URL + '/bbs/Gossiping/index' + str(page) + '.html',
        cookies = {'over18': '1'}
    )
    if resp.status_code != 200:
        print('Invalid url:', resp.url)
        
    soup = BeautifulSoup(resp.text, 'html.parser')
    divs = soup.find_all("div", "r-ent")        
    for div in divs:
        try:
            href = div.find('a')['href']
            link = PTT_URL + href
            article_id = re.sub('\.html', '', href.split('/')[-1])
            #print(link, article_id + '\n')
            parse_article(link, article_id)
        except:
            pass
        time.sleep(0.5)

def parse_article(link, article_id):
    resp = requests.get(
        url = link,
        cookies = {'over18': '1'},
        verify = True,
        timeout = 3
    )
    if resp.status_code != 200:
        print('Invalid url:', resp.url)
        return json.dumps({"error": "invalid url"}, sort_keys=True, ensure_ascii=False)

    soup = BeautifulSoup(resp.text, 'html.parser')
    main_content = soup.find(id="main-content")
    metas = main_content.select('div.article-metaline')
    author = ''
    title = ''
    date = ''
    if metas:
        author = metas[0].select('span.article-meta-value')[0].string if metas[0].select('span.article-meta-value')[0] else author
        title = metas[1].select('span.article-meta-value')[0].string if metas[1].select('span.article-meta-value')[0] else title
        date = metas[2].select('span.article-meta-value')[0].string if metas[2].select('span.article-meta-value')[0] else date
    
    # remove and keep push nodes
    pushes = main_content.find_all('div', class_='push')
    for push in pushes:
        push.extract()
    
    # 移除 '※ 發信站:' (starts with u'\u203b'), '◆ From:' (starts with u'\u25c6'), 空行及多餘空白
    # 保留英數字, 中文及中文標點, 網址, 部分特殊符號
    filtered = [ v for v in main_content.stripped_strings if v[0] not in [u'※', u'◆'] and v[:2] not in [u'--'] ]
    expr = re.compile(u(r'[^\u4e00-\u9fa5\u3002\uff1b\uff0c\uff1a\u201c\u201d\uff08\uff09\u3001\uff1f\u300a\u300b\s\w:/-_.?~%()]'))
    for i in range(len(filtered)):
        filtered[i] = re.sub(expr, '', filtered[i])

    filtered = [_f for _f in filtered if _f]  # remove empty strings
    filtered = [x for x in filtered if article_id not in x]  # remove last line containing the url of the article
    content = ' '.join(filtered)
    content = re.sub(r'(\s)+', ' ', content)

    data = {
        'url': link,
        'article_id': article_id,
        'article_title': title,
        'author': author,
        'date': date,
        'content': content,
    }
    print(data, '\n')

if __name__ == '__main__':
    PTT_URL = 'https://www.ptt.cc'
    filename = 'PTT_Gossiping.json'
    ptt_crawler()