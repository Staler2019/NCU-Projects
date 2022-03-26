import feedparser
# import json
import csv

# TOPIC = "all"
TOPIC = "subreddit"
RSS_URI = f"https://www.reddit.com/r/{TOPIC}/.rss"


def crawler():
    feed = feedparser.parse(RSS_URI)
    # with open(f"tmp_reddit_{TOPIC}.json", "w", encoding="UTF8") as f:
    #     json.dump(feed, fp=f, indent=4)

    print("found", len(feed.entries), "posts")

    with open(f"reddit_{TOPIC}.csv", "w", encoding="UTF-8", newline='') as f:
        csv_header = [
            "author",
            "tag",
            "link",
            "media_thumbnail",
            "title",
            "updated_times",
            "published_time",
        ]
        writer = csv.DictWriter(f, fieldnames=csv_header)
        writer.writeheader()

        for entry in feed.entries:
            if "author" not in entry.keys():
                continue

            json_dic = {
                "author": entry.author,
                "tag": entry.tags[0]["term"],
                "link": entry.link,
                "media_thumbnail": ""
                if "media_thumbnail" not in entry.keys()
                else entry.media_thumbnail[0]["url"],
                "title": entry.title,
                "updated_times": entry.updated,
                "published_time": entry.published,
            }
            # json_obj = json.dumps(json_dic, indent=4)
            # print(json_obj)

            # to csv
            writer.writerow(json_dic)


if __name__ == "__main__":
    crawler()
