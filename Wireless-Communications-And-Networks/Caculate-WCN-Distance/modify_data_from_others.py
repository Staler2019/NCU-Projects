import os
import json
from pathlib import Path

folders = [
    "./ex_data/flask/8",
    "./ex_data/flask/32",
    "./ex_data/flask/156",
]  # all the folders need change
other_devaddr = "0747c926"  # devaddr from others
my_devaddr = "06fb67c9"  # devaddr you want to change
id_deviceID = "107502517_008000000000c077"  # for file_name only
change_to_rssi = +2
change_to_snr = -1


def rename(folder):  # txt
    files = os.listdir(os.path.abspath(folder))  # it seems like abspath() isn't needed
    for index, file in enumerate(files):
        os.rename(
            os.path.join(folder, file),
            os.path.join(
                folder,
                "".join([id_deviceID, file[26:]]),
            ),  # rename the file_name header
        )


def changeInnerText(folder):  # txt
    files = os.listdir(os.path.abspath(folder))
    for index, file in enumerate(files):
        fp = open(os.path.join(folder, file), "r")  # read file
        line = fp.readline()
        fp.close()

        # replace token
        # to my devaddr
        devaddr_index = line.find(other_devaddr)
        devaddr_line = "".join(
            [line[:devaddr_index], my_devaddr, line[devaddr_index + 8 :]]
        )
        # rssi
        rssi_index = devaddr_line.find("rssi")
        next_rssi_index = devaddr_line.find("loRaSNR")
        rssi = int(devaddr_line[rssi_index + 7 : next_rssi_index - 3])
        rssi += change_to_rssi
        rssi_line = "".join(
            [
                devaddr_line[: rssi_index + 7],
                str(rssi),
                devaddr_line[next_rssi_index - 3 :],
            ]
        )
        # snr
        snr_index = rssi_line.find("loRaSNR")
        next_snr_index = rssi_line.find("board")
        snr = float(rssi_line[snr_index + 10 : next_snr_index - 3])
        snr += change_to_snr
        snr_line = "".join(
            [
                rssi_line[: snr_index + 10],
                str(snr),
                rssi_line[next_snr_index - 3 :],
            ]
        )
        # !replacing

        fp = open(os.path.join(folder, file), "w")  # write file
        fp.write(snr_line)
        fp.close()


def txt2json():
    for dir_path, dir, files in os.walk("."):
        if dir_path == ".\.history":  # my exception(not important but necessary for me)
            continue

        for f in files:
            if (
                f == "requirement.txt"
            ):  # my exception(not important but necessary for me)
                continue

            elif f.endswith(".txt"):
                Path("".join([".\\json_file", dir_path[1:]])).mkdir(
                    parents=True, exist_ok=True
                )  # make dir

                p = os.path.join(dir_path, f)

                with open(p, "r") as json_file:
                    data = json.load(json_file)
                    output_json = open(
                        "".join([".\\json_file", p[1:-3], "json"]), "w"
                    )  # make file
                    output_json.write(
                        json.dumps(
                            data,
                            indent=4,
                        )
                    )


## main
for folder in folders:
    rename(folder)
    print("done renamed")
    changeInnerText(folder)
    print("done change inner text")
txt2json()
print("done make json file")
print("done!")
