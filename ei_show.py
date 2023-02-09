from serial import Serial
from serial.tools.list_ports import comports
import time
import re
import base64
from PIL import Image
from io import BytesIO
import numpy as np
import cv2


def base642img(strs):
    imgdata = base64.b64decode(strs)
    img = Image.open(BytesIO(imgdata))
    img = np.array(img)
    img = cv2.resize(img, dsize=(240, 240))
  
    cv2.imshow('img', cv2.cvtColor(img, cv2.COLOR_RGB2BGR))
    cv2.waitKey(5)


if __name__ == '__main__':
    port_list = list(comports())
    for device in port_list:
        print(device.name)
    ser = Serial(port_list[1].name, 921600, timeout=2000)

    text = 'AT+RUNIMPULSEDEBUG=y\r\n'
    result = ser.write(text.encode("utf8"))
    all = 0
    err = 0
    data = b''
    while True:
        rev_num = ser.inWaiting()
        if rev_num:
            data += ser.read(rev_num)
        if data:
            a = re.findall('Framebuffer:(.*?)Predictions', data.__str__())
            if len(a) == 0:
                continue
            all += 1
            try:
                print(a[0])
                #a = a[0].replace(' ', '').replace('\\r', '').replace('\\n', '')
                #print(a)
                base642img(a[0])
            except:
                err+=1
                print(err/all)
            data = b''
