# -*- coding: utf-8 -*-
from aip import AipOcr
import json

APP_ID = '10999413'
API_KEY = 'oNzQDojuKDSeC8xKFApL5E8x'
SECRET_KEY = 'fXhtDT47iQVO6kBneTR3GXvYtL4wkpjv'

client = AipOcr(APP_ID, API_KEY, SECRET_KEY)

def get_file_content(filePath):
    with open(filePath, 'rb') as fp:
        return fp.read()

image = get_file_content('example.jpg')

# 将json数据显示为汉字(dumps方法)
print (json.dumps (client.bankcard(image), encoding='UTF-8', ensure_ascii=False));




